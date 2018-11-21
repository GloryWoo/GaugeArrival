
/*
 * GaugeArrival.cpp
 *
 *  Created on: 2014-1-9
 *      Author: ZheWu
 */

#include "GaugeArrival.hpp"
#include "TraceMacros.hpp"
#include "Utils.hpp"

using namespace Poco;
using namespace std;


#define GAUGE_ARRIVAL_GPS_TRACE_PATH       "/tmp/gpsTrace"//"/fs/mmc0/app/chc/gpsTrace"
#define GAUGE_ARRIVAL_BEHAVIOR_PATH        "/tmp/behavior"//"/fs/mmc0/app/chc/behavior"
#define GAUGE_ARRIVAL_BEHAVIOR_GPS_PATH    "/tmp/behavior_gps"//"/fs/mmc0/app/chc/behavior_gps"
#define FILE_PATH_LEN 32
#define MAX_DIFF_LAT_LON 0.0006//0.0002


//extern GaugeArrival * pGaugeArrive;;
GaugeArrival* GaugeArrival::mInstance = 0;
Mutex GaugeArrival::mGaugeMutex;


GaugeArrival* GaugeArrival::GetInstance()
{
   Mutex::ScopedLock lock(mGaugeMutex);
   if (!mInstance)
   {
       mInstance = new GaugeArrival();
       if (mInstance && !mInstance->initialize())
       {
          DeleteInstance();
       }
   }

   return mInstance;
}

void GaugeArrival::DeleteInstance()
{
   Mutex::ScopedLock lock(mGaugeMutex);

   if (mInstance)
   {
	   mInstance->disconnectSvcipc();
	   mInstance->deinitialize();

       delete mInstance;
       mInstance = 0;

   }
}

bool GaugeArrival::connectSvcIpc()
{
	CODE_TRACE_ENTER_METHOD();
	SVCIPC_tError rc = SVCIPC_initialize();

	if (SVCIPC_IS_ERROR(rc)) {
		printf("Failed to initialize");
	}
	rc = SVCIPC_getConnection(SVCIPC_CONNECTION_SESSION, 0, &mConn);
	if (SVCIPC_IS_ERROR(rc)) {
		printf("Failed to get connection");
	}
	rc = SVCIPC_registerService(mConn, GAUGE_SERVER,
			GAUGE_PATH, 0, GaugeArrival::onSvcipcRequestCallback, this, &mRegHnd);
	if (SVCIPC_IS_ERROR(rc)) {
		printf("Failed to register service");
	}
	subscribeServiceIPCSignal();
}

void GaugeArrival::disconnectSvcipc()
{
   CODE_TRACE_ENTER_METHOD();

   SVCIPC_tError svcipcError = SVCIPC_unregisterService(mRegHnd);
   if (SVCIPC_IS_ERROR(svcipcError))
   {
      CODE_TRACE_DBG_ERROR(("SVCIPC_unregisterService Error."));
   }

   svcipcError = SVCIPC_closeConnection(mConn);
   if (SVCIPC_IS_ERROR(svcipcError))
   {
      CODE_TRACE_DBG_ERROR(("SVCIPC_closeConnection Error."));
   }

   SVCIPC_shutdown();

   CODE_TRACE_EXIT_METHOD();
}


void GaugeArrival::lastGPSCallback(Timer& A){
	CODE_TRACE_ENTER_METHOD();
	int size = 0;
	int i = 0;
	GPS_POS posi;
	GaugeArrival *instance = GaugeArrival::GetInstance();
	double curLat, curLon;
	//CODE_TRACE_DBG_MSG(("lastGPSCallback0 pGaugeArrive addr = %d", (int)pGaugeArrive));
	CODE_TRACE_DBG_MSG(("lastGPSCallback0 instance addr = %d", (int)instance));
	CODE_TRACE_DBG_MSG(("mGpsPosVec = %d", (int)&instance->mGpsPosVec));
	CODE_TRACE_DBG_MSG(("mLastGPSTimer = %d", (int)instance->mLastGPSTimer));
	instance->mLastGPSTimer->restart(0);
	instance->mLastGPSTimer = NULL;

	instance->mLastGPS = instance->mCurGPS;
	size = instance->mGpsPosVec.size();
	CODE_TRACE_DBG_MSG(("mGpsPosVec size = %d", size));

	instance->mLastGPS.code_s = instance->mFirstGPS.code_s;
	CODE_TRACE_DBG_MSG(("lastGPSCallback1"));
	instance->getLatLon(curLat, curLon);
	for(i = 0; i < size; i++){
		posi = instance->mGpsPosVec.at(i);
		//if(DOUBLE_EQUAL(posi.lat, instance->mCurLat) && DOUBLE_EQUAL(posi.lon, instance->mCurLon)){
		if(abs(posi.lat-curLat) + abs(posi.lon-curLon) < MAX_DIFF_LAT_LON){
			break;
		}
	}
	CODE_TRACE_DBG_MSG(("lastGPSCallback2 i = %d", i));
	if(i == size){
		GPS_POS newPos = {0};
		newPos.code = i;
		newPos.lat = curLat;
		newPos.lon = curLon;
		instance->mGpsPosVec.push_back(newPos);
	}
	instance->mLastGPS.code_d = i;
	CODE_TRACE_DBG_MSG(("mLastGPS: %d, %d, %d, %d, %d, %d", instance->mLastGPS.code_s, instance->mLastGPS.code_d,
		instance->mLastGPS.date, instance->mLastGPS.time, instance->mLastGPS.date_d, instance->mLastGPS.time_d));
	CODE_TRACE_DBG_MSG(("lastGPSCallback mGpsPosVec size = %d", size));
	if(instance->mLastGPS.code_s == instance->mLastGPS.code_d)//avoid setStartPosition NDR data disturb
		return;
	instance->saveDriverBehavior();
	instance->mGetFirstGPSSig = 0;
	CODE_TRACE_EXIT_METHOD();
	//saveGPSTrace();
}
/**
 * @brief: Callback of "SEN_GPSInfo" signal, which is emitted by NDR service.
 *
 * Input Parameters:
 *    @param  sigName   [in]   	The name of the signal.
 *    @param  data    	[in]    The data of this signal.
 *    @param  token    	[in]    A user defined token to be returned with the callback.
 *
 * Return Value:
 *    @retval void
 */
void  GaugeArrival::onGPSDataNDRServiceIPCSignal(SVCIPC_tConstStr sigName,
		SVCIPC_tConstStr data, SVCIPC_tUserToken token) {
	GaugeArrival* arrival = static_cast<GaugeArrival*> (token);

	Mutex::ScopedLock lock(arrival->mGetDataMutex);
	CODE_TRACE_ENTER_METHOD();


	Json::Value mParms;
	Json::Reader reader;

	int y, m, d, h, min, s;

	if (0 == strcmp("GPSInfo", sigName)) {
		string str(data);
		tm *curTm;
		curTm = Utils::CurrentTimestamp();
		int rc = reader.parse(str, mParms);
		CODE_TRACE_DBG_MSG(("receive a SEN_GPSInfo"));
		if (0 == rc) {
			printf("============ Failed to parse SEN_GPSInfo data ");
			return;
		}

		//arrival->mGPSDataEvt.wait();
		//GPSInfo* gps = new GPSInfo();
		memset(&arrival->mGpsInfo, 0 , sizeof(GPSInfo));
		//char pYYMMDD[11] = { 0 };
		//y = mParms["year"].asInt();
		//m = mParms["month"].asInt();
		//d = mParms["day"].asInt();
		y = curTm->tm_year+1900;
		m = curTm->tm_mon+1;
		d = curTm->tm_mday;
		sprintf(arrival->mGpsInfo.date, "%04d%02d%02d", y, m, d);
		//arrival->mGpsInfo.date = pYYMMDD;


		//char pHHMMSS[9] = { 0 };
		//h = mParms["hrs"].asInt();
		//min = mParms["min"].asInt();
		//s = mParms["sec"].asInt();
		h = curTm->tm_hour;
		min = curTm->tm_min;
		s = curTm->tm_sec;
		sprintf(arrival->mGpsInfo.time, "%02d%02d%02d", h, m, s);
		//arrival->mGpsInfo.time = pHHMMSS;


		double latitude = mParms["latitude"].asDouble();
		//latitude /= 1000000;
		arrival->mGpsInfo.latitude = latitude;

		double longitude = mParms["longitude"].asDouble();
		//longitude /= 1000000;
		arrival->mGpsInfo.longitude = longitude;

		/*double speed = mParms["speed"].asDouble();
		speed /= 100;
		gps->speed = speed;

		double altitude = mParms["height"].asDouble();
		gps->altitude = altitude;*/

		//pthread_mutex_lock(&sigGPSInfoMutex);
		/*fprintf(stderr, "After lock ,date = %s, time = %s, latitude = %8.6f, longitude = %9.6f, speed = %5.2f, altitude = %6.2f \n",
		 gps->date.c_str(), gps->time.c_str(), gps->latitude, gps->longitude, gps->speed, gps->altitude);
		 */
		//mGpsInfo.push_front(gps);
		//pthread_mutex_unlock(&sigGPSInfoMutex);

		//arrival->mCurGPS.code_d = arrival->getCodeByLatLon(latitude, longitude);
		CODE_TRACE_DBG_MSG(("YYMMDD: %d %d %d", y, m, d));
		CODE_TRACE_DBG_MSG(("HHMMSS: %d %d %d", h, min, s));
		arrival->mCurGPS.date_d = YEAR_MON_DATE_CVT(y, m, d);
		arrival->mCurGPS.time_d = TIME_CVT(h, min, s);
		arrival->setLatLon(latitude, longitude);
		CODE_TRACE_DBG_MSG(("latitude = %llf, longitude = %llf", latitude, longitude));
		if(arrival->mGetFirstGPSSig == 0){
			//arrival->mCurGPS.code_s = arrival->getCodeByLatLon(latitude, longitude);
			arrival->mCurGPS.date = YEAR_MON_DATE_CVT(y, m, d);
			arrival->mCurGPS.time = TIME_CVT(h, min, s);
			arrival->mGetFirstGPSSig = 1;
			arrival->mFirstGPS = arrival->mCurGPS;

			int size = arrival->mGpsPosVec.size();
			int i;
			CODE_TRACE_DBG_MSG(("mGpsPosVec size = %d", size));
			for(i = 0; i < size; i++){
				GPS_POS posi = arrival->mGpsPosVec.at(i);
				//if(DOUBLE_EQUAL(posi.lat, latitude) && DOUBLE_EQUAL(lon, longitude))
				if(abs(posi.lat-latitude) + abs(posi.lon-longitude) < MAX_DIFF_LAT_LON){
					CODE_TRACE_DBG_MSG(("get identical start Postion id = %d", i));
					CODE_TRACE_DBG_MSG(("lat1=%lf lat2=%lf lon1=%lf lon2=%lf", posi.lat, latitude,posi.lon, longitude));
					break;
				}
			}
			if(i == size){
				CODE_TRACE_DBG_MSG(("i == size %d", size));
				GPS_POS newPos = {0};
				newPos.code = i;
				newPos.lat = latitude;
				newPos.lon = longitude;
				arrival->mGpsPosVec.push_back(newPos);
			}
			arrival->mCurGPS.code_s = i;
			arrival->mFirstGPS.code_s = i;
			//arrival->getGPSTraceRow();
		}


		if(arrival->mLastGPSTimer == NULL){
			arrival->mLastGPSTimer = new Timer(1000, 3500);
			TimerCallback<GaugeArrival> cb(*arrival, &GaugeArrival::lastGPSCallback);
			arrival->mLastGPSTimer->start(cb);
		}
		else{
			arrival->mLastGPSTimer->restart(5000);
		}
		//arrival->saveGPSTrace();

		arrival->mGPSDataEvt.set();


	}
	CODE_TRACE_EXIT_METHOD();
}

void GaugeArrival::getLatLon(double &lat, double &lon){
	Mutex::ScopedLock lock(mLatLonMutex);

	lat = mCurLat;
	lon = mCurLon;
}

void GaugeArrival::setLatLon(double lat, double lon){
	Mutex::ScopedLock lock(mLatLonMutex);

	mCurLat = lat;
	mCurLon = lon;
}

bool GaugeArrival::getGPSTraceRow(){
	int i;
	bool cmpRet = false;
	for(i = 0; i < 10; i++){
		if(mGpsData[i][0].size() > 0 && mGpsData[i][0].at(0).date[0] != '\0'){
			cmpRet = isIdenticalPos(mGpsInfo, mGpsData[i][0].at(0));
			if(cmpRet){
				mCurRow = i;
				return true;
			}
		}
	}
	return false;
}

bool GaugeArrival::isIdenticalPos(GPSInfo &le, GPSInfo &ri){
	double latDiff, lonDiff, sumDiff;

	latDiff = abs(le.latitude - ri.latitude);
	lonDiff = abs(le.longitude - ri.longitude);
	sumDiff = latDiff + lonDiff;

	return sumDiff < MAX_DIFF_LAT_LON ? true : false;
}

//latitude 1"=30.9m; longitude 1"=23.6m
bool GaugeArrival::compareTrace(){
	int i = 0, j = 0;
	int col = 0;
	int len = 0;
	bool breakJ = false;
	double lonDiff, latDiff, sDiff, curDiff, lastDiff = 0, minDiff;
	GPSInfo tmpInfo;

	CODE_TRACE_ENTER_METHOD();
	if(mCurRow != -1 && mCurCol!= -1){
		for(j = mCurCol; j < 10 && !breakJ; j++){
			len = mGpsData[mCurRow][mCurCol].size();
			for(i = mCurIdx; i < len; i++){
				tmpInfo = mGpsData[mCurRow][mCurCol].at(i);
				latDiff = abs(tmpInfo.latitude - mGpsInfo.latitude);
				lonDiff = abs(tmpInfo.longitude - mGpsInfo.longitude);
				curDiff = latDiff + lonDiff;
				if(i == mCurIdx){
					sDiff = curDiff;
					minDiff = curDiff;
				}
				else{
					//smaller than min diff, it is close to the point of historical track
					if(curDiff < minDiff){
						minDiff = curDiff;
						mCurIdx = i;
						mCurCol = j;
						breakJ = true;
						break;
					}
					else{
						//bigger than default max diff, car has leave the historical track
						if(curDiff > MAX_DIFF_LAT_LON){
							//mCurRow = -1;
							mCurIdx = 0;
							break;
						}
					}
				}
				lastDiff = curDiff;
			}
		}
		if(j == 10)
			mNewTrack = true;
		return true;
	}
	else if(mCurRow != -1 && mCurCol == -1){
		for(j = 0; j < 10 && !breakJ; j++){
			if(mGpsData[mCurRow][j].size() == 0)
				break;
			len = mGpsData[mCurRow][j].size();
			for(i = mCurIdx; i < len; i++){
				tmpInfo = mGpsData[mCurRow][j].at(i);
				latDiff = abs(tmpInfo.latitude - mGpsInfo.latitude);
				lonDiff = abs(tmpInfo.longitude - mGpsInfo.longitude);
				curDiff = latDiff + lonDiff;
				if(i == mCurIdx){
					sDiff = curDiff;
					minDiff = curDiff;
				}
				else{
					//smaller than min diff, it is close to the point of historical track
					if(curDiff < minDiff){
						minDiff = curDiff;
						mCurIdx = i;
						mCurCol = j;
						breakJ = true;
						break;
					}
					else{
						//bigger than default max diff, car has leave the historical track
						if(curDiff > MAX_DIFF_LAT_LON){
							//mCurRow = -1;
							mCurIdx = 0;
							break;
						}
					}
				}
				lastDiff = curDiff;
			}
		}
		if(j == 10)
			mNewTrack = true;
	}
	return false;
}

void GaugeArrival::loadGPSTrace(){
	FILE *fp;

	int i = 0, j = 0;
	char fileNm[FILE_PATH_LEN] = {0};
	int defaultLEN = strlen(GAUGE_ARRIVAL_GPS_TRACE_PATH);
	GPSInfo info = {0};

	CODE_TRACE_ENTER_METHOD();
	strcpy(fileNm, GAUGE_ARRIVAL_GPS_TRACE_PATH);
	for(; i < 10; i++){
		for(; j < 10; j++){
			memset(fileNm+defaultLEN, 0, 4);
			sprintf(fileNm+defaultLEN, "%02d%02d", i, j);
			fp = fopen(fileNm, "r");
			if(fp != NULL){
				while(feof(fp) == 0){
					memset(&info, 0, sizeof(GPSInfo));
					fread(&info, sizeof(GPSInfo), 1, fp);
					mGpsData[i][j].push_back(info);
				}
				fclose(fp);
			}
			else
				break;
		}
	}

}

void GaugeArrival::saveGPSTrace(){
	if(!mNewTrack)
		return;

	FILE *fp;
	char fileNm[FILE_PATH_LEN] = {0};
	char newNm[FILE_PATH_LEN] = {0};
	int fileID = 0;
	int i = 0, j = 0;
	int len;
	int defaultLEN = strlen(GAUGE_ARRIVAL_GPS_TRACE_PATH);

	CODE_TRACE_ENTER_METHOD();
	strcpy(fileNm, GAUGE_ARRIVAL_GPS_TRACE_PATH);
	strcpy(newNm, GAUGE_ARRIVAL_GPS_TRACE_PATH);
	//move each file name to rear, if last one reach max size, delete the last
	if(mCurRow == -1){//it's a new Start Track

		for(j = 0; j < 10; j++){
			if(mGpsData[9][j].size() == 0)
				break;
			sprintf(fileNm+defaultLEN, "%02d%02d", 9, j);
			remove(fileNm);
		}

		for(i = 0; i < 9; i++){
			for(j = 0; j < 10; j++){
				if(mGpsData[i][j].size() == 0)
					break;
				sprintf(fileNm+defaultLEN, "%02d%02d", i, j);
				sprintf(newNm+defaultLEN, "%02d%02d", i+1, j);
				rename(fileNm, newNm);
			}
		}

		mCurRow = 0;
	}
	else{
		if(mCurCol == -1){
		}
		else if(mCurCol == 9){
			sprintf(fileNm+defaultLEN, "%02d%02d", mCurRow, 9);
			remove(fileNm);
		}
		else{
			for(i = mCurCol; i > 0; i--){
				sprintf(fileNm+defaultLEN, "%02d%02d", mCurRow, i);
				fp = fopen(fileNm, "r");
				if(fp != NULL){
					fclose(fp);
					sprintf(newNm+defaultLEN, "%02d%02d", mCurRow, i+1);
					rename(fileNm, newNm);
				}
			}
		}
	}
	len = mGpsInfoVec.size();
	sprintf(fileNm+defaultLEN, "%02d%02d", mCurRow, 0);
	fp = fopen(fileNm, "w");
	if(fp == NULL)
		return;
	i = 0;
	while(i < len){
		fwrite(&mGpsInfoVec.at(i), sizeof(GPSInfo), 1, fp);
		i++;
	}
	fclose(fp);
}

void * GaugeArrival::calcArrival(void *arg){

	//GPSInfo gpsLastInfo;
	//bool cmpRet = false;
	GaugeArrival* arrival = static_cast<GaugeArrival*> (arg);

	CODE_TRACE_ENTER_METHOD();
	//Mutex::ScopedLock runLock = new Mutex::ScopedLock(mGetDataMutex);
	while (arrival->mThreadRun){
		//sleep(3000);
		CODE_TRACE_DBG_MSG(("Enter While Run"));
		arrival->mGPSDataEvt.wait();
		arrival->mGetDataMutex.lock();

		CODE_TRACE_DBG_MSG(("Invoke"));
		//if(memcmp(&gpsLastInfo, &arrival->mGpsInfo, sizeof(GPSInfo)) == 0){
		//	arrival->mGetDataMutex.unlock();
		//	continue;
		//}

		//gpsLastInfo = arrival->mGpsInfo;

		arrival->mGpsInfoVec.push_back(arrival->mGpsInfo);
		//if(!arrival->mNewTrack && arrival->mCurRow != -1){
		//	cmpRet = arrival->compareTrace();
		//}

		arrival->mGetDataMutex.unlock();
	}
	pthread_exit(0);
	CODE_TRACE_EXIT_METHOD();
}

short GaugeArrival::getCodeByLatLon(double lat, double lon){
	CODE_TRACE_ENTER_METHOD();
	int i, len = mGpsPosVec.size();
	GPS_POS pos;
	for(i = 0; i < len; i++){
		pos = mGpsPosVec.at(i);
		//if(DOUBLE_EQUAL(lat, pos.lat) && DOUBLE_EQUAL(lon, pos.lon))
		if(abs(lat-pos.lat) + abs(lon-pos.lon) < MAX_DIFF_LAT_LON)
			return pos.code;
	}
	return -1;
}

GPS_POS GaugeArrival::getLatLonByCode(int code){
	CODE_TRACE_ENTER_METHOD();
	int i, len = mGpsPosVec.size();
	GPS_POS pos = {0};

	for(i = 0; i < len; i++){
		pos = mGpsPosVec.at(i);
		if(pos.code == code){
			return pos;
		}
	}
	return pos;
}

void GaugeArrival::subscribeServiceIPCSignal(void) {
	CODE_TRACE_ENTER_METHOD();
	SVCIPC_tError rc = SVCIPC_initialize();


	if (SVCIPC_IS_ERROR(rc)) {
		printf("Failed to initialize");
	}
	//Subscribe NDR signal
	rc = SVCIPC_subscribe(mConn, NAV_PATH, "GPSInfo",
			GaugeArrival::onGPSDataNDRServiceIPCSignal, (SVCIPC_tUserToken)this, &mSubHnd);
	if (SVCIPC_IS_ERROR(rc)) {
		printf("Failed to subscribe SEN_GPSInfo signal from NDR service");
	}
	//End subscribe NDR signal

}

GaugeArrival::GaugeArrival(){

}

GaugeArrival::~GaugeArrival(){

}

void GaugeArrival::gaugeDestinationAndArriveTime(SVCIPC_tReqContext context,
        SVCIPC_tConstStr parms,
        SVCIPC_tUserToken token){

}

void GaugeArrival::onSvcipcRequestCallback(SVCIPC_tReqContext context,
                             SVCIPC_tConstStr method,
                             SVCIPC_tConstStr parms,
                             SVCIPC_tBool noReplyExpected,
                             SVCIPC_tUserToken token){
	CODE_TRACE_ENTER_METHOD();
	GaugeArrival* arrival = static_cast<GaugeArrival*> (token);

	if (0 == strcmp(method, "gaugeDestinationAndArriveTime"))
	{
	 GaugeArrival::gaugeDestinationAndArriveTime(context, parms, (SVCIPC_tUserToken) token);
	}
	else if(0 == strcmp(method, "predictDestinationAtStart")){
		//GaugeArrival::predictDestinationAtStart(context, parms, (SVCIPC_tUserToken) token);

		Json::Value array;
		Json::FastWriter fastWriter;
		Json::Value item;
		GPSPosResLst::iterator iter;
		int lenth = arrival->mGpsRetLst.size();
		GPS_POS pos;
		int ye, mon, da, hr, min, sec;
		char dateChar[9] = {0}, timeChar[7] = {0};
		string ret;

		CODE_TRACE_DBG_MSG(("mGpsRetLst size=%d", lenth));

		item["latitude"] = 31.1620891058769;//39.9140421931169
		item["longitude"] = 121.390435776738;//116.407722911789;
		array["dest"].append(item);
		ret = fastWriter.write(array);
//		if (lenth > 0)
//		{
//			for(iter = arrival->mGpsRetLst.begin(); iter != arrival->mGpsRetLst.end(); iter++){
//				pos = arrival->getLatLonByCode(iter->code);
//				item["latitude"] = pos.lat;
//				item["longitude"] = pos.lon;
//				YEAR_MON_DATE_DVT(&ye, &mon, &da, iter->date_d);
//				sprintf(dateChar, "%04d%02d%02d", ye, mon, da);
//				TIME_DVT(&hr, &min, &sec, iter->time_d);
//				sprintf(timeChar, "%02d%02d%02d", hr, min, sec);
//				item["date"] = dateChar;//new string(dateChar);
//				item["time"] = timeChar;//new string(timeChar);
//				item["rate"] = iter->rate;
//				array["dest"].append(item);
//			}
//			ret = fastWriter.write(array);
//		} else {
//			//array["dest"].append(NULL);
//			ret = "{\"dest\":[]}";
//		}

		SVCIPC_asyncReturnResult(context, ret.c_str(), NULL, NULL);
	}
}


bool GaugeArrival::initialize(){
	CODE_TRACE_ENTER_METHOD();

	mCurRow = -1;
	mCurCol = -1;
	mCurIdx = 0;
	mGetFirstGPSSig = 0;

	//loadGPSTrace();
	connectSvcIpc();
	loadDriverBehavior();
	mThreadRun = true;
	createCalcThread();
	CODE_TRACE_EXIT_METHOD();
	return true;
}


void GaugeArrival::deinitialize(){

}

bool GaugeArrival::createCalcThread(){
	pthread_t calcThread;
	int err = pthread_create(&calcThread, 0, calcArrival, this);

	CODE_TRACE_EXIT_METHOD();
	return false;
}

/* same code_s put together as a cluster
 * item in cluster list as count descendent
 * */
void GaugeArrival::putInDrawer(BehavGPSVec* vecDraw, int idx, Behavior_GPS behav){
	int j, heapLen, k;
	Behavior_GPS tmpBeh;
	vector<Behavior_GPS>::iterator it;

	CODE_TRACE_ENTER_METHOD();
	heapLen = vecDraw[idx].size();

	if(heapLen == 0){
		behav.count = 1;
		vecDraw[idx].push_back(behav);
	}
	else{
		for(j = 0; j < heapLen; j++){
			if(vecDraw[idx].at(j).code_s == behav.code_s){
				//vecDraw[idx].at(j).countOfStart++;

				if(vecDraw[idx].at(j).code_d == behav.code_d){
					vecDraw[idx].at(j).count++;
					k = j-1;
					//big count stay on front
					while(k > 0 && vecDraw[idx].at(k).code_s == behav.code_s
					&& vecDraw[idx].at(k).count <= vecDraw[idx].at(j).count){
						k--;
					}
					if(k != j-1){
						tmpBeh = vecDraw[idx].at(j);
						vecDraw[idx].erase(vecDraw[idx].begin()+j);
						it = vecDraw[idx].begin()+k+1;
						vecDraw[idx].insert(it, tmpBeh);
					}
					break;
				}else{
					behav.count = 1;
					//behav.countOfStart = 1;
					it = vecDraw[idx].begin()+heapLen;
					vecDraw[idx].insert(it, behav);
					break;
				}

			}
		}
		if(j == heapLen){
			behav.count = 1;
			//behav.countOfStart = 1;
			vecDraw[idx].push_back(behav);
		}
	}
}

bool GaugeArrival::getArriveTime(int startTm, int destTm, int newTm, int &newDestTm){
	int h, m, s, h1, m1, s1, h2, m2, s2;
	bool nextDay = false;

	TIME_DVT(&h, &m, &s, startTm);
	TIME_DVT(&h1, &m1, &s1, destTm);

	if(s1>=s)
		s2 = s1-s;
	else{
		s2 = s1+60-s;
		m1--;
	}

	if(m1 >= m)
		m2 = m1-m;
	else{
		m2 = m1+60-m;
		h1--;
	}

	if(h1 >= h)
		h2 = h1-h;
	else{
		h2 = h1+24-h;
	}

	TIME_DVT(&h, &m, &s, newTm);
	s1 = s+s2;
	if(s1 >= 60){
		s1 -= 60;
		m1 = m+m2+1;
	}else
		m1 = m+m2;

	if(m1 >= 60){
		m1 -= 60;
		h2 = h+h1+1;
	}else
		h2 = h+h1;

	if(h2 >= 24){
		h2 -= 24;
		nextDay = true;
	}
	newDestTm = TIME_CVT(h2, m2, s2);

}

void GaugeArrival::getDestRateFromDraw(GPSPosResVec& vecRes, BehavGPSVec* vecDraw, int idx, Behavior_GPS behav){
	int heapLen = vecDraw[idx].size(),  j, k = 0;
	GPS_Pos_Res gpsRes = {0};
	//GPS_POS pos = {0};
	bool ret;

	CODE_TRACE_ENTER_METHOD();
	for(j = 0; j < heapLen; j++){
		if(vecDraw[idx].at(j).code_s == behav.code_s){
			CODE_TRACE_DBG_MSG(("find same code_s id = %d", j));
			gpsRes.code_s = vecDraw[idx].at(j).code_s;
			gpsRes.code_d = vecDraw[idx].at(j).code_d;
			gpsRes.date_d = behav.date;//vecDraw[idx].at(j).date_d;
			ret = GaugeArrival::getArriveTime(vecDraw[idx].at(j).time, vecDraw[idx].at(j).time_d, behav.time, gpsRes.time_d);
			if(ret)
				Utils::getDateByDiffDay(behav.date, gpsRes.date_d);
			gpsRes.time_d = vecDraw[idx].at(j).time_d;
			gpsRes.count = vecDraw[idx].at(j).count;
			//gpsRes.countOfStart = vecDraw[idx].at(j).countOfStart;
			//if(gpsRes.countOfStart != 0)
			//	gpsRes.rate = ((float)gpsRes.count)/((float)gpsRes.countOfStart);
			//pos = getLatLonByCode(gpsRes.code_d);
			//gpsRes.lat = pos.lat;
			//gpsRes.lon = pos.lon;
			vecRes.push_back(gpsRes);
			k++;
			if(k == 5)
				break;
		}
	}
}

void GaugeArrival::loadDriverBehavior(){
	FILE *fp, *fp1;
	int weekDay, dayPerYear, ye, mon, da, hr,min,sec,i = 0;

	CODE_TRACE_ENTER_METHOD();
	fp = fopen(GAUGE_ARRIVAL_BEHAVIOR_PATH, "r");
	if(fp == NULL){
		CODE_TRACE_DBG_MSG(("open behavior read failed"));
		return;
	}
	Behavior_GPS behav;
	MapItem *item;
	int memSize = sizeof(Behavior_GPS)-4;
	while(feof(fp) == 0){
		memset(&behav, 0, sizeof(Behavior_GPS));
		fread(&behav, memSize, 1, fp);
		mBehavGpsVec.push_back(behav);
		YEAR_MON_DATE_DVT(&ye, &mon, &da, behav.date);
		weekDay = Utils::CaculateWeekDay(ye, mon, da);
		if(mStCntMap.find(behav.code_s) != NULL){
			mStCntMap.find(behav.code_s)->second++;
		}
		else{
			item = new MapItem(behav.code_s, 1);
			mStCntMap.insert(*item);
		}
		putInDrawer(mBehavWeek, weekDay, behav);

		TIME_DVT(&hr, &min, &sec, behav.time);
		putInDrawer(mBehavHour, hr, behav);

		putInDrawer(mBehavDatePerMon, da-1, behav);

		dayPerYear = Utils::getIdxOfDate(ye, mon, da);
		putInDrawer(mBehavDatePerYear, dayPerYear, behav);

		i++;
	}
	fclose(fp);
	GPS_POS pos;
	fp1 = fopen(GAUGE_ARRIVAL_BEHAVIOR_GPS_PATH, "r");
	if(fp1 == NULL){
		CODE_TRACE_DBG_MSG(("open behavior GPS read failed"));
		return;
	}
	int sizePos = 16;//sizeof(GPS_POS)-2;
	i = 0;
	while(feof(fp1) == 0){
		memset(&pos, 0, sizeof(GPS_POS));
		fread(((char*)&pos)+8, sizePos, 1, fp1);
		pos.code = i++;
		mGpsPosVec.push_back(pos);
	}
	mInitPosVecSize = mGpsPosVec.size();
	fclose(fp1);
}

void GaugeArrival::saveDriverBehavior(){
	FILE *fp = fopen(GAUGE_ARRIVAL_BEHAVIOR_PATH, "a+");
	int i = 0;
	int size, sizeGPSBeh, sizeGPSPos;
	//Behavior_GPS gpsBehav;
	GPS_POS posi;
	int ye, mon, da, weekDay, dayPerYear, hr, min, sec;
	MapItem *item;
	CODE_TRACE_ENTER_METHOD();
	if(fp == NULL){
		CODE_TRACE_DBG_MSG(("save Driver Behavior open file failed."));
		return;
	}

	mBehavGpsVec.push_back(mLastGPS);
	size = mBehavGpsVec.size();

	if(mStCntMap.find(mLastGPS.code_s) != NULL){
		mStCntMap.find(mLastGPS.code_s)->second++;
	}
	else{
		item = new MapItem(mLastGPS.code_s, 1);
		mStCntMap.insert(*item);
	}
	YEAR_MON_DATE_DVT(&ye, &mon, &da, mLastGPS.date);
	weekDay = Utils::CaculateWeekDay(ye, mon, da);
	putInDrawer(mBehavWeek, weekDay, mLastGPS);

	TIME_DVT(&hr, &min, &sec, mLastGPS.time);
	putInDrawer(mBehavHour, hr, mLastGPS);

	putInDrawer(mBehavDatePerMon, da-1, mLastGPS);

	dayPerYear = Utils::getIdxOfDate(ye, mon, da);
	putInDrawer(mBehavDatePerYear, dayPerYear, mLastGPS);

	sizeGPSBeh = sizeof(Behavior_GPS)-4;
	fseek(fp, 0, 2);
	fwrite(&mLastGPS, sizeGPSBeh, 1, fp);
	fclose(fp);

	sizeGPSPos = 16;
	CODE_TRACE_DBG_MSG(("sizeGPSPos = %d", sizeGPSPos));
	fp = fopen(GAUGE_ARRIVAL_BEHAVIOR_GPS_PATH, "a+");
	if(fp == NULL){
		return;
	}
	fseek(fp, 0, 2);
	CODE_TRACE_DBG_MSG(("mInitPosVecSize = %d, mGpsPosVec.size = %d", mInitPosVecSize, mGpsPosVec.size()));
	while(mInitPosVecSize < mGpsPosVec.size()){
		fwrite(((char *)&mGpsPosVec.at(mInitPosVecSize))+8, sizeGPSPos, 1, fp);
		mInitPosVecSize++;
	}
	fclose(fp);
	CODE_TRACE_EXIT_METHOD();
}
bool Float_Compare(const GPS_Pos_Res_Rate l, const GPS_Pos_Res_Rate r){
	return l.rate > r.rate ? true : false;
}

void GaugeArrival::getDestAndRate(){
	GPSPosResVec vecCate[4];
	GPS_Pos_Res_Rate destRate = {0};
	int weekDay, dayPerYear, ye, mon, da, hr, min, sec, i, j;
	float weights[4] = {0.5, 0.3, 0.15, 0.05};
	GPSPosResLst::iterator iter;
	int countOfStart = 0;

	CODE_TRACE_ENTER_METHOD();
	YEAR_MON_DATE_DVT(&ye, &mon, &da, mFirstGPS.date);
	weekDay = Utils::CaculateWeekDay(ye, mon, da);
	TIME_DVT(&hr, &min, &sec, mFirstGPS.time);
	dayPerYear = Utils::getIdxOfDate(ye, mon, da);
	CODE_TRACE_DBG_MSG(("ye=%d, mon=%d, da=%d, hr=%d, min=%d, sec=%d", ye, mon, da, hr, min, sec));
	CODE_TRACE_DBG_MSG(("weekDay=%d, dayPerYear=%d", weekDay, dayPerYear));

	getDestRateFromDraw(vecCate[0], mBehavWeek, weekDay, mFirstGPS);
	getDestRateFromDraw(vecCate[1], mBehavHour, hr, mFirstGPS);
	getDestRateFromDraw(vecCate[2], mBehavDatePerMon, da-1, mFirstGPS);
	getDestRateFromDraw(vecCate[3], mBehavDatePerYear, dayPerYear, mFirstGPS);

	GPS_Pos_Res_Rate resRate = {0};
	if(!mGpsRetLst.empty()){
		mGpsRetLst.clear();
	}
	for(i = 0; i < 4; i++){
		CODE_TRACE_DBG_MSG(("id_i=%d, vec_size=%d", i, vecCate[i].size()));
		for(j = 0; j < vecCate[i].size(); j++){
			if(mStCntMap.find(vecCate[i].at(j).code_s) != NULL)
				countOfStart = mStCntMap.find(vecCate[i].at(j).code_s)->second;
			else
				countOfStart = 0;
			for(iter = mGpsRetLst.begin(); iter != mGpsRetLst.end(); iter++){
				if(iter == NULL)
					break;
				if(iter->code == vecCate[i].at(j).code_d){
					if(countOfStart != 0)
						iter->rate += ((float)vecCate[i].at(j).count)/((float)countOfStart)*weights[i];
					break;
				}
			}
			if( iter == NULL || iter == mGpsRetLst.end()){
				CODE_TRACE_DBG_MSG(("it's a single identical code_d"));
				destRate.code = vecCate[i].at(j).code_d;
				destRate.date_d = vecCate[i].at(j).date_d;
				destRate.time_d = vecCate[i].at(j).time_d;
				//destRate.lat = vecCate[i].at(j).lat;
				//destRate.lon = vecCate[i].at(j).lon;
				if(countOfStart != 0)
					destRate.rate = ((float)vecCate[i].at(j).count)/((float)countOfStart)*weights[i];
				else
					destRate.rate = 0;
				mGpsRetLst.push_back(destRate);
				if(mGpsRetLst.size() == 3){
					mGpsRetLst.sort(Float_Compare);
					return;
				}
				//break;
			}
		}
	}
	mGpsRetLst.sort(Float_Compare);
}

void GaugeArrival::predictDestinationAtStart(SVCIPC_tReqContext context,
        SVCIPC_tConstStr parms,
        SVCIPC_tUserToken token){

	CODE_TRACE_ENTER_METHOD();
	GaugeArrival* arrival = static_cast<GaugeArrival*> (token);
	string str(parms);
	Json::Value jsonMsg = Json::nullValue;

	bool success = JsonHandler::ParseJsonString(str, jsonMsg);
	if(success){
		JsonHandler::ValidateJsonDouble("latitude", jsonMsg, arrival->mStartLat);
		JsonHandler::ValidateJsonDouble("longitude", jsonMsg, arrival->mStartLon);
	}
	CODE_TRACE_DBG_MSG(("param lat=%lf lon=%lf", arrival->mStartLat, arrival->mStartLon));

	int size = arrival->mGpsPosVec.size();
	int i;
	tm *curTm;
	curTm = Utils::CurrentTimestamp();
	int y, m, d, h, min, s;
	CODE_TRACE_DBG_MSG(("mGpsPosVec size = %d", size));
	for(i = 0; i < size; i++){
		GPS_POS posi = arrival->mGpsPosVec.at(i);
		//if(DOUBLE_EQUAL(posi.lat, latitude) && DOUBLE_EQUAL(lon, longitude))
		if(abs(posi.lat-arrival->mStartLat) + abs(posi.lon-arrival->mStartLon) < MAX_DIFF_LAT_LON){
			CODE_TRACE_DBG_MSG(("get identical start Postion id = %d", i));
			break;
		}
	}
	if(i == size){
		CODE_TRACE_DBG_MSG(("i == size %d", size));
		GPS_POS newPos = {0};
		newPos.code = i;
		newPos.lat = arrival->mStartLat;
		newPos.lon = arrival->mStartLon;
		arrival->mGpsPosVec.push_back(newPos);
	}
	arrival->mFirstGPS.code_s = i;
	y = curTm->tm_year+1900;
	m = curTm->tm_mon+1;
	d = curTm->tm_mday;

	arrival->mFirstGPS.date = YEAR_MON_DATE_CVT(y, m, d);
	h = curTm->tm_hour;
	min = curTm->tm_min;
	s = curTm->tm_sec;

	arrival->mFirstGPS.time = TIME_CVT(h, min, s);
	arrival->getDestAndRate();

}

//for test
void GaugeArrival::printResult(){
	getDestAndRate();
	CODE_TRACE_DBG_MSG(("resList size = %d", mGpsRetLst.size()));
	GPS_Pos_Res_Rate res;
	GPSPosResLst::iterator iter;
	int i = 0;
	GPS_POS pos = {0};

	for(iter = mGpsRetLst.begin(); iter != mGpsRetLst.end(); iter++, i++){
		pos = getLatLonByCode(iter->code);
		iter->lat = pos.lat;
		iter->lon = pos.lon;
		CODE_TRACE_DBG_MSG(("%d, lat=%lf, lon=%lf, rate=%f, date=%d, time=%d", i, iter->lat, iter->lon, iter->rate, iter->date_d, iter->time_d));
	}
}







