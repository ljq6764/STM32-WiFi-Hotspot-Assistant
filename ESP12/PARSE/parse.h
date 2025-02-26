#ifndef __PARSE_H
#define __PARSE_H

#include "io_bit.h"

typedef struct {
    long currentConfirmedCount;	//现有确诊
    long currentConfirmedIncr;
    long confirmedCount;		//累计确诊
    long confirmedIncr;
	long suspectedCount;		
	long suspectedIncr;
    long curedCount;			//累计治愈
    long curedIncr;
    long seriousCount;
    long seriousIncr;
    long deadCount;
    long deadIncr;
    char updateTime[20];
	
	double total_vaccinations;
    double new_vaccinations;
    double total_vaccinations_per_hundred;
}__ncov_data;
extern __ncov_data dataChina;
extern __ncov_data dataGlobal;

typedef struct {
	char currentAir;
	char currentAirName[6];
	char currentDegree[6];
	char currentHumidity[6];
	char currentWeather[8];
	char windDirection[6];
	char windPower[6];
	char updateTime[20];
	char observe1[40];
	char observe2[40];
	char todayWeather[20];
	char todayDegree[8];
	char tomorrowWeather[20];
	char tomorrowDegree[8];
}__weather_data;
extern __weather_data weather_data; 

unsigned char ParseEpidemic_Data(void);
unsigned char ParseVaccine_Data(void);
unsigned char ParseCurrentWeather_Data(void);
unsigned char ParseFutureWeather_Data(void);
unsigned char ParseTime_Data(void);

#endif
