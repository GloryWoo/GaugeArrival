/*
 * GaugeArrival.hpp
 *
 *  Created on: 2014-1-9
 *      Author: ZheWu
 */

#ifndef GAUGEARRIVAL_HPP_
#define GAUGEARRIVAL_HPP_

#include <cstdlib>
#include <iostream>
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <list>
#include <map>
#include <sys/time.h>
#include <time.h>
#include "JsonHandler.hpp"
#include "jsoncpp/json.h"
#include "svcipc/svcipc.h"
#include "Poco/Event.h"
#include "Poco/Mutex.h"
#include "Poco/Timer.h"





#define SERVER_NAME "com.harman.service.Navigation"
#define SERVER_PATH "/com/harman/service/Navigation"
#define NAV_SERVER "com.harman.HMI.NavServer"
#define NAV_PATH  "/com/harman/HMI/NavServer"
#define NDR_NAME "com.harman.service.NDR"
#define NDR_PATH  "/com/harman/service/NDR"
#define GAUGE_SERVER "com.harman.service.GaugeService"
#define GAUGE_PATH  "/com/harman/service/GaugeService"

typedef struct GPSInfo_s {
	char date[9];//20140101
	char time[7];//121501
	double latitude;
	double longitude;
	//double speed;
	//double altitude;
} GPSInfo;

typedef struct Behavior_GPS_T{
	short code_s;//start position code
	short code_d;//destination positon code
	int date;
	int time;
	int date_d;
	int time_d;
	int count;
	//int countOfStart;
}Behavior_GPS;

typedef struct GPS_POS_T{
	short code;
	double lat;
	double lon;
}GPS_POS;

typedef struct GPS_POS_RES_T{
	short code_s;
	short code_d;
	//double lat;
	//double lon;
	int date_d;
	int time_d;
	int count;
	//int countOfStart;
	float rate;
}GPS_Pos_Res;

typedef struct GPS_POS_RES_RATE_T{
	short code;
	double lat;
	double lon;
	int date_d;
	int time_d;
	float rate;
}GPS_Pos_Res_Rate;

typedef std::vector <GPSInfo> GPSVec;
typedef std::vector <Behavior_GPS> BehavGPSVec;
typedef std::vector <GPS_POS> GPSPosVec;
typedef std::vector <GPS_Pos_Res> GPSPosResVec;
typedef std::list <GPS_Pos_Res_Rate> GPSPosResLst;
typedef std::map <int,int> StartCountMap;
typedef std::pair<int,int> MapItem;

class GaugeArrival
{
public:
	GaugeArrival();
	~GaugeArrival();

	static GaugeArrival *GetInstance();
	static void DeleteInstance();
	static void * calcArrival(void *arg);
	static void  onGPSDataNDRServiceIPCSignal(SVCIPC_tConstStr sigName,SVCIPC_tConstStr data, SVCIPC_tUserToken token);
	static void onSvcipcRequestCallback(SVCIPC_tReqContext context,SVCIPC_tConstStr method,SVCIPC_tConstStr parms,SVCIPC_tBool noReplyExpected,SVCIPC_tUserToken token);
	static void gaugeDestinationAndArriveTime(SVCIPC_tReqContext context,SVCIPC_tConstStr parms,SVCIPC_tUserToken token);
	static void predictDestinationAtStart(SVCIPC_tReqContext context,SVCIPC_tConstStr parms,SVCIPC_tUserToken token);
	bool connectSvcIpc();
	void disconnectSvcipc();
	bool initialize();
	void deinitialize();
	bool createCalcThread();
	bool compareTrace();
	void subscribeServiceIPCSignal();
	static bool getArriveTime(int startTm, int destTm, int newTm, int &newDestTm);
	//test
	void printResult();
private:
	static GaugeArrival* mInstance;
	static Poco::Mutex mGaugeMutex;
	bool getGPSTraceRow();
	void loadGPSTrace();
	void saveGPSTrace();
	bool isIdenticalPos(GPSInfo &le, GPSInfo &ri);

	void loadDriverBehavior();
	void saveDriverBehavior();
	void putInDrawer(BehavGPSVec* vecDraw, int idx, Behavior_GPS behav);
	short getCodeByLatLon(double lat, double lon);
	GPS_POS getLatLonByCode(int code);
	void lastGPSCallback(Poco::Timer& A);
	void getDestAndRate();
	void getDestRateFromDraw(GPSPosResVec& vecRes, BehavGPSVec* vecDraw, int idx, Behavior_GPS behav);
	void getLatLon(double &lat, double &lon);
	void setLatLon(double lat, double lon);
	Poco::Mutex mGetDataMutex;
	Poco::Mutex mLatLonMutex;
	Poco::Event mGPSDataEvt;

	bool mThreadRun;
	GPSInfo mGpsInfo;
	//deque<GPSInfo*> gpsInfo;
	GPSVec mGpsData[10][10];
	GPSVec mGpsInfoVec;
	StartCountMap mStCntMap;
	BehavGPSVec mBehavGpsVec;
	BehavGPSVec mBehavWeek[7];
	BehavGPSVec mBehavHour[24];
	BehavGPSVec mBehavDatePerMon[31];
	BehavGPSVec mBehavDatePerYear[366];
	GPSPosVec mGpsPosVec;
	//GPSPosResVec gpsPosResVec;
	Behavior_GPS mCurGPS;
	Behavior_GPS mFirstGPS;
	Behavior_GPS mLastGPS;
	GPSPosResLst mGpsRetLst;
	double mCurLat;
	double mCurLon;
	double mStartLat;
	double mStartLon;
	int mInitPosVecSize;
	int mGetFirstGPSSig;
	int mCurRow;
	int mCurCol;
	int mCurIdx;
	//int curState;//1 -- completely coincide; 2 -- no coincide; 3-- start coincide but routine not
	bool mNewTrack;
	SVCIPC_tSvcRegHnd mRegHnd;
	SVCIPC_tSvcRegHnd mSubHnd;
	SVCIPC_tHandle invokeHnd;
	SVCIPC_tConnection mConn;
	Poco::Timer *mLastGPSTimer;
};

#endif /* GAUGEARRIVAL_HPP_ */
