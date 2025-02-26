#ifndef __RTC_H
#define __RTC_H	    

#include "io_bit.h"

typedef struct 
{
	volatile unsigned char hour;
	volatile unsigned char min;
	volatile unsigned char sec;			

	volatile unsigned short w_year;
	volatile unsigned char  w_month;
	volatile unsigned char  w_date;
	volatile unsigned char  week;		
}_calendar_obj;					 
extern _calendar_obj calendar;	

void Rtc_Init(void);        
unsigned char isLeapYear(unsigned short year);
unsigned char RTC_Alarm_Set(unsigned short syear, unsigned char smon, unsigned char sday, 
							unsigned char hour, unsigned char min, unsigned char sec);
unsigned char RTC_GetCurrentDateTime(void);        
unsigned char RTC_Get_Week(unsigned short year, unsigned char month, unsigned char day);
unsigned char RTC_SetCurrentDateTime(unsigned short syear, unsigned char smon, unsigned char sday,
					  unsigned char hour, unsigned char min, unsigned char sec);		 
unsigned int dateToSecCount(unsigned short syear, unsigned char smon, unsigned char sday,
							unsigned char hour, unsigned char min, unsigned char sec);
#endif


