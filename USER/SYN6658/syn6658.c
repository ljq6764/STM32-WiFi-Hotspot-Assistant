#include "syn6658.h"
#include "usart3.h"
#include "systick.h"
#include "QMsg.h"
#include "rtc.h"
#include "control.h"
#include "parse.h"
const char syn6658_i[] = "[i1]";	//0 - 不识别汉语拼音,1 - 识别汉语拼音
const char syn6658_m[] = "[m3]";	//3 - 晓玲 (女声)
									//51 - 尹小坚 (男声)
									//52 - 易小强 (男声)
									//53 - 田蓓蓓 (女声)
									//54 - 唐老鸭 (效果器)
									//55 - 小燕子 (女童声																		
const char syn6658_s[] = "[s5]";	//语速值（0至10）说明：语速值越小，语速越慢
const char syn6658_t[] = "[t8]";	//语调值（0至10）语调值越小，基频值越低
const char syn6658_v[] = "[v1]";  	//音量值（0至10）音量的调节范围为静音到音频设备支持的最大值
const char syn6658_d[] = "[[d]";	//所有设置（除发音人设置外）恢复为默认值

__report_plan report_plan;
CQueue qhead;

void TaskReportMsg_Handle(void)
{
	if(report_plan.is_report_plan != true){		//如果没有播报计划
		return;
	}
	
	if(report_plan.is_syn_free != true){		//如果此时模块不是空闲
		return;
	}
	
	report_plan.current_plan = Syn6658_GetReportPlan();	//从队列中读取计划
	if(report_plan.current_plan == REPORT_IDLE_VAL){	//队列为空
		report_plan.is_report_plan = false;				//取消播报计划	
		return;	
	}

	report_plan.is_syn_free = false;	//标记模块正在繁忙
	char temp[64] = {0};
	
	switch(report_plan.current_plan){	//将需要播报的内容组合好，发送给模块即可
		case 0:	//播报时间的计划
			sprintf(temp, "北京时间，%02d:%02d:%02d，%d/%02d/%02d", calendar.hour, calendar.min, calendar.sec, calendar.w_year, calendar.w_month, calendar.w_date);
			Syn6658_SyntheticVoiceCmd(temp, SYN_MODULE_REPORT);
			break;
		case 1:	//播报天气的计划
			sprintf(temp, "南宁今天有小雨，现在的温度比较舒适，出门记得带伞。\r\n");
			Syn6658_SyntheticVoiceCmd(temp, SYN_MODULE_REPORT);
			break;
	}
}
//将计划添加到播报计划队列中
unsigned char Syn6658_SetReportPlan(unsigned char value)
{
	if(QMsg.push(&qhead, value) == true){
		printf("The queue is full!\r\n");
		return REPORT_IDLE_VAL;
	}
	
	report_plan.is_report_plan = true;			//标记有播报计划
	
	return 0;
}

unsigned char Syn6658_GetReportPlan(void)
{
	unsigned char value;
	
	if(QMsg.pop(&qhead, &value) == true){
		report_plan.is_report_plan = false;		//取消播报计划	
		printf("The queue is null!\r\n");
		return REPORT_IDLE_VAL;
	}
	
	return value;
}
/*********************************************************************************************************
* 函 数 名 : Syn6658_SyntheticVoiceCmd
* 功能说明 : 向syn6658发送命令
* 形    参 : text：需要播报的文本,此模块无法播报英文； isInit：true为初始化命令，需要等待模块响应。 false为播报文本，不等待响应。
			 非阻塞的播报情况下就传入false，阻塞播报则传入true。
* 返 回 值 : 无
* 备    注 : 非阻塞播报需要等待上一个播报计划完成后，才能启动下一次播放，否则上一次播报计划会被中断。
*********************************************************************************************************/ 
void Syn6658_SyntheticVoiceCmd(const char *text, unsigned char isInit)
{
	unsigned char headOfFrame[5];
	unsigned char length = strlen(text);
	unsigned char ErrorCounter = 0;

	headOfFrame[0] = 0xFD; 		//构建帧头
	headOfFrame[1] = 0x00; 		//数据区长度高位
	headOfFrame[2] = length + 2;//数据区长度低位
	headOfFrame[3] = 0x01; 		//命令字：合成播放命令
	headOfFrame[4] = 0x01; 		//命令参数：编码格式为 GBK
	Usart3_SendPackage(headOfFrame, 5);				//发送帧头
	Usart3_SendPackage((unsigned char *)text, length);//发送内容

	while(1){
		if(usart3.RecFlag == true){
			usart3.RecFlag = false;
			usart3.RecLen = 0;
			if(usart3.RxBuff[0] == SYN_MODULE_RECEIVE_CORRECT_CMD){	//收到正确的命令帧回传
				break;
			}else{	//失败则重新发送
				ErrorCounter++;
				if(ErrorCounter&0x8){
					printf("error1: %x\r\n", usart3.RxBuff[0]);
					break;
				}
				Usart3_SendPackage(headOfFrame, 5);					//发送帧头
				Usart3_SendPackage((unsigned char *)text, length);	//发送内容
			}
		}
	}
	if(isInit == true){
		ErrorCounter = 0;
		memset(usart3.RxBuff, 0, sizeof(usart3.RxBuff[0]));
		while(1){
			if(usart3.RecFlag == true){
				Usart_StatusClear(USART3);
				if(usart3.RxBuff[0] == SYN_MODULE_FREE){				//模块空闲
					break;
				}else{													//失败则重新发送
					ErrorCounter++;
					if(ErrorCounter&0x8){
						printf("error2: %x\r\n", usart3.RxBuff[0]);
						break;
					}
					Usart3_SendPackage(headOfFrame, 5);					//发送帧头
					Usart3_SendPackage((unsigned char *)text, length);	//发送内容
				}
			}
		}
	}
}

void Syn6658_Init(void)
{
	Usart3_Init(115200);
	Delay_Ms(500);
	Delay_Ms(500);
	/*每次上电都要重新初始化，不然就会变默认*/
	Syn6658_SyntheticVoiceCmd(syn6658_s, SYN_MODULE_INIT);			//初始化语速，设置参数时，必须阻塞等待完成，也就是SYN_MODULE_INIT
	Syn6658_SyntheticVoiceCmd(syn6658_v, SYN_MODULE_INIT);			//初始化音量
	//Syn6658_SyntheticVoiceCmd("欢迎使用，信盈达热点助手", SYN_MODULE_REPORT);  	//非阻塞播报测试样例
	//Syn6658_SyntheticVoiceCmd("欢迎使用，信盈达热点助手", SYN_MODULE_INIT);  		//阻塞播报测试样例
	
	//以下内容为非阻塞播放的支持，如果不需要则忽略
	report_plan.is_report_plan = false;	//标记无播报计划
	report_plan.is_syn_free = true;		//标记模块正在空闲
	report_plan.click_cont_index = 0;
	report_plan.current_plan = REPORT_IDLE_VAL;
	QMsg.init(&qhead);					//初始化消息处理队列
}
