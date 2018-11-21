#ifndef UTILS_HPP
#define UTILS_HPP

#include <time.h>
#include "stdint.h"

#define DOUBLE_EQUAL(a, b) ((a)-(b) < 0.00001 && (a)-(b) > -0.00001)? true:false									  \


#define YEAR_MON_DATE_CVT(y, m, d) ((y)*10000 + (m) * 100 + (d))

#define YEAR_MON_DATE_DVT(y, m, d, YMD) do{    \
		*y = (YMD)/10000; *m = ((YMD)%10000)/100; *d = (YMD)%100; \
}while(0);

#define TIME_CVT(h, m, s) ((h)*10000 + (m) * 100 + (s))


#define TIME_DVT(h, m, s, hms) do{    \
		*h = (hms)/10000; *m = ((hms)%10000)/100; *s = (hms)%100; \
}while(0);

class Utils
{
public:
	static int CaculateWeekDay(int y,int m, int d);
	static int getIdxOfDate(int y, int mon, int da);
	static tm * CurrentTimestamp();
	static bool getDateByDiffDay(int ymd, int &newYmd);
};

#endif /* UTILS_HPP */
