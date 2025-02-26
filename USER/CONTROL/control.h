#ifndef __CONTROL_H
#define __CONTROL_H

#include "io_bit.h"

#define CACHE_SIZE 40

#define LV_DEBUG 0

#define __IntervalExecute(func, curr, end) \
do { \
	if(((curr) >= (end))) func, curr = 0; \
}while(0)

typedef struct {
	char *cache;					//处理缓存
	unsigned char  lcdTimeSlice;	//lcd时间片
	unsigned char  rtcTimeSlice;	//rtc时间片
	unsigned char  keyTimeSlice;	//key时间片
	unsigned char  espTimeSlice;	//esp时间片
	unsigned char  synTimeSlice;	//syn时间片
	unsigned short ledTimeSlice;	//led时间片
	unsigned short memTimeSlice;	//mem时间片
	unsigned short lcdBacklight;	//lcd背光值
	unsigned int   system;			//sys时间戳
}__ctrl_dev;
extern __ctrl_dev ctrl_dev;

typedef union {
	struct {
		bool rtcIsSynAlarm:1;			//整点报时标示
		bool rtcIsUpdateTime:1;			//时间更新标示
		unsigned char rtcIsUpdate:5;	//日期更新标示
		unsigned char rtcIsCheck:5;		//信息校对标志
	}r;
	unsigned short w;
}__rtc_flag;
extern __rtc_flag rtc_flag;

typedef union {
	struct {
		bool is_request:1;				//请求标志
		bool networkIsRequest:1;		//网络请求标示
		bool requestIsFinish:1;			//计划处理完成标示
		unsigned char is_connect:2;		//网络连接标志
		unsigned char request_number:3;	//请求编号
	}r;
	unsigned char w;
}__network_flag;
extern __network_flag network_flag;

extern char const *weekTable0[8];
extern char const *monthTable[12];

extern char const *weekTable[7];
extern char const *windDirection[8];
extern char const *windClass[10];

void ctrl_Init(void);

void TaskUpdateTime_Handle(void);
void TaskLcdBacklightMsg_Handle(void);

#endif
