#include<iostream>
using namespace std;
typedef struct tagCOMMONTIME
{
	int   year;
	int   month;
	int   day;
	int   hour;
	int   minute;
	double   second;
}COMMONTIME;//通用时
typedef COMMONTIME *PCOMMONTIME;

typedef struct tagTimeOfDay
{
	long seconds;   //秒数的整数部分
	double tos;//秒数的小数部分
}TimeOfDay;
typedef TimeOfDay *PTimeOfDay;

typedef struct
{
	long day; 		//整数天数
	TimeOfDay tod;  //一天内的秒数
}JULIANDAY;//儒略日
typedef JULIANDAY *PJULIANDAY;

typedef struct tagMJULIANDAY
{
	long day;
	TimeOfDay  tod;
}MJULIANDAY;//新儒略日
typedef MJULIANDAY *PMJIANDAY;

typedef struct tagTimeOfWeek
{
	long seconds;	//秒整数部分
	double tos;	//秒小数部分
}TimeOfWeek;
typedef TimeOfWeek *PTimeOfWeek;

typedef struct tagGPSTIME
{
	int weeks; 		//周数
	TimeOfWeek tow;	//一周内的秒数
}GPSTIME;//GPS时
typedef GPSTIME *PGPSTIME;

typedef struct tagDayOfYear
{
	unsigned short year;
	unsigned short day;
	TimeOfDay tod;
}DayOfYear;//年积日
typedef DayOfYear *PDayOfYear;

//! 取小数部分
double FRAC(double morigin)
{
	return morigin - long(morigin);
}

//! 通用时到儒略日的转换
void CommonTimeToJulianDay(PCOMMONTIME pct, PJULIANDAY pjd) 
{
	if (pct->year<1900)
	{
		if (pct->year<80)
			pct->year += 2000;
		else pct->year += 1900;
	}
	double ut = pct->hour + pct->minute / 60.0 + pct->second / 3600.0;
	if (pct->month <= 2)
	{
		pct->year -= 1;
		pct->month += 12;
	}
	pjd->day = int(365.25*pct->year) + int(30.6001*(pct->month + 1)) + pct->day + int(ut / 24 + 1720981.5);
	pjd->tod.seconds = ((pct->hour + 12) % 24) * 3600 + pct->minute * 60 + (int)pct->second;//秒的整数部分
	pjd->tod.tos = pct->second - (int)pct->second;//秒的小数部分
}

//! 儒略日到通用时的转换
void JulianDayToCommonTime(PJULIANDAY pjd, PCOMMONTIME pct)
{
	double x = pjd->day + (pjd->tod.seconds + pjd->tod.tos) / (60.0*60.0 * 24);
	int a = int(x + 0.5);
	int b = a + 1537;
	int c = int((b - 122.1) / 365.25);
	int d = int(365.25*c);
	int e = int((b - d) / 30.6001);
	pct->day = b - d - int(30.6001*e);
	pct->month = e - 1 - 12 * int(e / 14);
	pct->year = c - 4715 - int((7 + pct->month) / 10);
	pct->hour = (pjd->tod.seconds / 3600 + 12) % 24;
	pct->minute = (pjd->tod.seconds % 3600) / 60;
	pct->second = pjd->tod.seconds % 60 + pjd->tod.tos;
	int N = a % 7;
}
//! 儒略日到GPS时的转换
void JulianDayToGPSTime(PJULIANDAY pjd, PGPSTIME pgt)
{
	double x = pjd->day + (pjd->tod.seconds + pjd->tod.tos) / (60.0*60.0 * 24);
	pgt->weeks = int((x - 2444244.5) / 7);
	pgt->tow.seconds = int(((pjd->day - 2444244) % 7 + (pjd->tod.seconds / (60.0*60.0 * 24) - 0.5)) * 86400);
	pgt->tow.tos = pjd->tod.tos;
}

//! GPS时到儒略日的转换
void GPSTimeToJulianDay(PGPSTIME pgt, PJULIANDAY pjd)
{
	pjd->day = int(pgt->weeks * 7 + double(pgt->tow.seconds) / 86400.0 + 2444244.5);
	pjd->tod.seconds = (pgt->tow.seconds + 43200) % 86400;
	pjd->tod.tos = pgt->tow.tos;
}

//! 通用时到GPS时的转换
void CommonTimeToGPSTime(PCOMMONTIME pct, PGPSTIME pgt)
{
	PJULIANDAY pjd = new JULIANDAY;
	CommonTimeToJulianDay(pct, pjd);
	JulianDayToGPSTime(pjd, pgt);
}

//! GPS时到通用时的转换
void GPSTimeToCommonTime(PGPSTIME pgt, PCOMMONTIME pct)
{
	PJULIANDAY pjd = new JULIANDAY;
	GPSTimeToJulianDay(pgt, pjd);
	JulianDayToCommonTime(pjd, pct);
}

//! 通用时到年内天
void CommonTimeToDOY(PCOMMONTIME pct, PDayOfYear pdoy)
{
	PCOMMONTIME pcto = new COMMONTIME;
	pcto->year = pct->year;
	pcto->month = 1;
	pcto->day = 1;
	pcto->hour = 0;
	pcto->minute = 0;
	pcto->second = 0;

	PJULIANDAY pjdo = new JULIANDAY;

	double JD, JDO;
	CommonTimeToJulianDay(pcto, pjdo);
	JDO = pjdo->day + (pjdo->tod.seconds + pjdo->tod.tos) / 86400;

	PJULIANDAY pjd = new JULIANDAY;
	CommonTimeToJulianDay(pct, pjd);

	JD = pjd->day + (pjd->tod.seconds + pjd->tod.tos) / 86400;

	pdoy->day = short(JD - JDO + 1);
	pdoy->year = pct->year;

	pdoy->tod.seconds = long(pct->hour * 3600
		+ pct->minute * 60 + pct->second);
	pdoy->tod.tos = pct->second - int(pct->second);    /*pct->hour*3600
													   +pct->minute*60+pct->second-pdoy->tod.seconds;*/
}

//! 年内天到普通时
void DOYToCommonTime(PDayOfYear pdoy, PCOMMONTIME pct)
{
	PCOMMONTIME pcto = new COMMONTIME;
	pcto->year = pdoy->year;
	pcto->month = 1;
	pcto->day = 1;
	pcto->hour = 0;
	pcto->minute = 0;
	pcto->second = 0;
	PJULIANDAY pjdo = new JULIANDAY;
	double JD, JDO;
	CommonTimeToJulianDay(pcto, pjdo);
	JDO = pjdo->day + (pjdo->tod.seconds + pjdo->tod.tos) / 86400;
	JD = JDO + pdoy->day + (pdoy->tod.seconds + pdoy->tod.tos) / 86400 - 1;
	long a, b, c, d, e;
	a = (long)(JD + 0.5);
	b = a + 1537;
	c = (long)((b - 122.1) / 365.25);
	d = (long)(365.25*c);
	e = (long)((b - d) / 30.6001);
	pct->day = short(b - d - (long)(30.6001*e) + FRAC(JD + 0.5));
	pct->month = short(e - 1 - 12 * (long)(e / 14));
	pct->year = short(c - 4715 - (long)((7 + pct->month) / 10));
	pct->hour = short((pdoy->tod.seconds + pdoy->tod.tos) / 3600);
	pct->minute = short((pdoy->tod.seconds + pdoy->tod.tos
		- pct->hour * 3600) / 60);
	pct->second = pdoy->tod.seconds + pdoy->tod.tos
		- pct->hour * 3600 - pct->minute * 60;
}
void GPSTimeToDOY(PGPSTIME pgt, PDayOfYear pdoy)
{
	PJULIANDAY pjd = new JULIANDAY;
	GPSTimeToJulianDay(pgt, pjd);
	PCOMMONTIME pct = new COMMONTIME;
	JulianDayToCommonTime(pjd, pct);
	CommonTimeToDOY(pct, pdoy);
}
void DOYToGPSTime(PDayOfYear pdoy, PGPSTIME pgt)
{
	PCOMMONTIME pct = new COMMONTIME;
	DOYToCommonTime(pdoy, pct);
	CommonTimeToGPSTime(pct, pgt);
}
void JulianDayToDOY(PJULIANDAY pjd, PDayOfYear pdoy)
{
	PCOMMONTIME pct = new COMMONTIME;
	JulianDayToCommonTime(pjd, pct);
	CommonTimeToDOY(pct, pdoy);
}
void DOYToJulianDay(PDayOfYear pdoy, PJULIANDAY pjd)
{
	PCOMMONTIME pct = new COMMONTIME;
	DOYToCommonTime(pdoy, pct);
	CommonTimeToJulianDay(pct, pjd);
}

void main()
{
	PCOMMONTIME pct = new COMMONTIME;
	PJULIANDAY pjd = new JULIANDAY;
	PDayOfYear pdoy = new DayOfYear;
	PGPSTIME pgt = new GPSTIME;
	pct->year = 2011;
	pct->month = 1;
	pct->day = 6;
	pct->hour = 19;
	pct->minute = 38;
	pct->second = 45.26;
	cout << "转换之前的通用时（当地时间）：";
	cout << pct->year << " " << pct->month << " " << pct->day << " " << pct->hour << ":" << pct->minute << ":" << pct->second << endl;
	cout << endl;
	CommonTimeToJulianDay(pct, pjd);//通用时->儒略日的转换
	cout << "通用时->儒略日的转换:";
	cout << pjd->day << "  " << pjd->tod.seconds << "  " << pjd->tod.tos << endl;
	cout << endl;
	JulianDayToCommonTime(pjd, pct);//儒略日->通用时的转换
	cout << "儒略日->通用时的转换:";
	cout << pct->year << " " << pct->month << " " << pct->day << " " << pct->hour << ":" << pct->minute << ":" << pct->second << endl;
	cout << endl;
	JulianDayToGPSTime(pjd, pgt);//儒略日->GPS时的转换
	cout << "儒略日->GPS时的转换:";
	cout << pgt->weeks << "  " << pgt->tow.seconds << "  " << pgt->tow.tos << endl;
	cout << endl;
	GPSTimeToJulianDay(pgt, pjd);//GPS时->儒略日的转换
	cout << "GPS时->儒略日的转换:";
	cout << pjd->day << "  " << pjd->tod.seconds << "  " << pjd->tod.tos << endl;
	cout << endl;
	CommonTimeToGPSTime(pct, pgt);//通用时->GPS时的转换
	cout << "通用时->GPS时的转换:";
	cout << pgt->weeks << "  " << pgt->tow.seconds << "  " << pgt->tow.tos << endl;
	cout << endl;
	GPSTimeToCommonTime(pgt, pct);//GPS时->通用时的转换
	cout << "GPS时->通用时的转换:";
	cout << pct->year << " " << pct->month << " " << pct->day << " " << pct->hour << ":" << pct->minute << ":" << pct->second << endl;
	cout << endl;
	CommonTimeToDOY(pct, pdoy);//通用时->年积日
	cout << "通用时->年积日:";
	cout << pdoy->year << "  " << pdoy->day << "  " << pdoy->tod.seconds << "  " << pdoy->tod.tos << endl;
	cout << endl;
	DOYToCommonTime(pdoy, pct);//年积日->通用时
	cout << "年积日->通用时:";
	cout << pct->year << " " << pct->month << " " << pct->day << " " << pct->hour << ":" << pct->minute << ":" << pct->second << endl;
	cout << endl;
	GPSTimeToDOY(pgt, pdoy);//GPS时->年积日
	cout << "GPS时->年积日:";
	cout << pdoy->year << "  " << pdoy->day << "  " << pdoy->tod.seconds << "  " << pdoy->tod.tos << endl;
	cout << endl;
	DOYToGPSTime(pdoy, pgt);//年积日->GPS时
	cout << "年积日->GPS时:";
	cout << pgt->weeks << "  " << pgt->tow.seconds << "  " << pgt->tow.tos << endl;
	cout << endl;
	JulianDayToDOY(pjd, pdoy);//儒略日->年积日
	cout << "儒略日->年积日:";
	cout << pdoy->year << "  " << pdoy->day << "  " << pdoy->tod.seconds << "  " << pdoy->tod.tos << endl;
	cout << endl;
	DOYToJulianDay(pdoy, pjd);//年积日->儒略日
	cout << "年积日->儒略日:";
	cout << pjd->day << "  " << pjd->tod.seconds << "  " << pjd->tod.tos << endl;
	cout << endl;
	cout << "经过各种转换后还原得到的通用时:";
	cout << pct->year << " " << pct->month << " " << pct->day << " " << pct->hour << ":" << pct->minute << ":" << pct->second << endl;
	cout << endl;
}
