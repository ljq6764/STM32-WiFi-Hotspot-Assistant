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
	char *cache;					//������
	unsigned char  lcdTimeSlice;	//lcdʱ��Ƭ
	unsigned char  rtcTimeSlice;	//rtcʱ��Ƭ
	unsigned char  keyTimeSlice;	//keyʱ��Ƭ
	unsigned char  espTimeSlice;	//espʱ��Ƭ
	unsigned char  synTimeSlice;	//synʱ��Ƭ
	unsigned short ledTimeSlice;	//ledʱ��Ƭ
	unsigned short memTimeSlice;	//memʱ��Ƭ
	unsigned short lcdBacklight;	//lcd����ֵ
	unsigned int   system;			//sysʱ���
}__ctrl_dev;
extern __ctrl_dev ctrl_dev;

typedef union {
	struct {
		bool rtcIsSynAlarm:1;			//���㱨ʱ��ʾ
		bool rtcIsUpdateTime:1;			//ʱ����±�ʾ
		unsigned char rtcIsUpdate:5;	//���ڸ��±�ʾ
		unsigned char rtcIsCheck:5;		//��ϢУ�Ա�־
	}r;
	unsigned short w;
}__rtc_flag;
extern __rtc_flag rtc_flag;

typedef union {
	struct {
		bool is_request:1;				//�����־
		bool networkIsRequest:1;		//���������ʾ
		bool requestIsFinish:1;			//�ƻ�������ɱ�ʾ
		unsigned char is_connect:2;		//�������ӱ�־
		unsigned char request_number:3;	//������
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
