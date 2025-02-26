#ifndef __SYN6658_H
#define __SYN6658_H

#include "io_bit.h"

#define SYN_MODULE_INIT_OK 				0x4a
#define SYN_MODULE_RECEIVE_CORRECT_CMD 	0X41
#define SYN_MODULE_RECEIVE_ERROR_CMD 	0X45
#define SYN_MODULE_BUSY 				0X4e
#define SYN_MODULE_FREE 				0X4f

#define SYN_MODULE_INIT 	1
#define SYN_MODULE_REPORT 	0

#define REPORT_IDLE_VAL 0XFF

void Syn6658_Init(void);
void Syn6658_SyntheticVoiceCmd(const char *text, unsigned char isInit);
unsigned char Syn6658_SetReportPlan(unsigned char value);
unsigned char Syn6658_GetReportPlan(void);
void TaskReportMsg_Handle(void);
	
typedef struct{
	unsigned char click_cont_index;				//点击的容器序列
	unsigned char is_report_plan;				//是否有播报计划
	unsigned char current_plan;					//当前播报计划
	unsigned char is_syn_free;					//模块空闲标志
}__report_plan;
extern __report_plan report_plan;	
	
extern const char *report_list[30];

#endif
