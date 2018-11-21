/*
 * Utils.cpp
 *
 *  Created on: 2014-2-17
 *      Author: ZheWu
 */
#include <sys/time.h>
#include "Utils.hpp"
#include "TraceMacros.hpp"

int Utils:: CaculateWeekDay(int y, int m, int d)
{
	if(m==1) {m=13;y--;}
	if(m==2) {m=14;y--;}
	int week=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7+1;

	if(week == 7)
		week = 0;
	return week;
	/*string weekstr="";
	switch(week)
	{
		case 1: weekstr="星期一"; break;
		case 2: weekstr="星期二"; break;
		case 3: weekstr="星期三"; break;
		case 4: weekstr="星期四"; break;
		case 5: weekstr="星期五"; break;
		case 6: weekstr="星期六"; break;
		case 7: weekstr="星期日"; break;
	}
	return weekstr;*/
}

int Utils::getIdxOfDate(int y, int mon, int da){
	int daByMon[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int idx = 0, i = 0;

	CODE_TRACE_ENTER_METHOD();
	if((y % 4 == 0 && y %100 != 0) || y%400 == 0)
		daByMon[1] = 29;

	while(i < mon-1){
		idx += daByMon[i];
		i++;
	}
	idx += (da - 1);
	return idx;
}


tm *Utils::CurrentTimestamp() {
    timeval te;
    tm *time;
    gettimeofday(&te, NULL);
    uint32_t milliseconds = te.tv_sec*1000 + te.tv_usec/1000;
    time = localtime(&te.tv_sec);
    return time;
}

bool Utils::getDateByDiffDay(int ymd, int &newYmd){//diff day is 1
	int y, m, dt, y1, m1, dt1;
	int daByMon[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	YEAR_MON_DATE_DVT(&y, &m, &dt, ymd);
	if(__leap_year(y))
		daByMon[1] = 29;


	dt1 = dt++;
	m1 = m;
	y1 = y;
	if(dt1 > daByMon[m-1]){
		dt1 -= daByMon[m-1];
		m1 = m++;
		if(m1 > 12){
			m1 -= 12;
			y1 = y+1;
		}else
			y1 = y;
	}
	newYmd = YEAR_MON_DATE_CVT(y1, m1, dt1);
	return true;
}
