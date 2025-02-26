#include "control.h"
#include "rtc.h"
#include "syn6658.h"
#include "malloc.h"
#include "esp12.h"
#include "QMsg.h"
 
char const *weekTable0[8] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun", "Null"};

char const *monthTable[12] = {"Jan", "Feb", "Mar", "Apr", "May", 
							"Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

char const *weekTable[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", 
						"Thursday", "Friday", "Saturday"};
 
char const *windDirection[8] = {"������", "����", "���Ϸ�", "�Ϸ�", "���Ϸ�", "����", "������", "����"};
char const *windClass[10] = {"һ��", "����", "����", "�ļ�", "�弶", "����", "�߼�", "�˼�", "�ż�", "ʮ��"};


__ctrl_dev ctrl_dev;
__rtc_flag rtc_flag;	
__network_flag network_flag;			
						
void TaskLcdBacklightMsg_Handle(void)
{
	static unsigned short backlight = 1000;
	
	if(backlight != ctrl_dev.lcdBacklight){
		if(backlight > ctrl_dev.lcdBacklight){
			if(backlight >= 1){
				backlight--;
			}
		}else{
			if(backlight < 1000){
				backlight++;
			}
		}
		TIM1->CCR1 = backlight;
	}
}

void TaskUpdateTime_Handle(void)
{
	if(rtc_flag.r.rtcIsUpdateTime == true){						//���rtcʱ���Ѿ�����
		ctrl_dev.cache = (char *)mymalloc(CACHE_SIZE);
		rtc_flag.r.rtcIsUpdateTime = false;						//���δ����
		
		if(rtc_flag.r.rtcIsSynAlarm == true){					//������������㲥��
			if(calendar.min == 0 && calendar.sec == 0){			//�����ʱ������
				Syn6658_SetReportPlan(0);						//����һ��������ǰʱ��ļƻ�
			}
		}
		
		if(rtc_flag.r.rtcIsUpdate != calendar.w_date){			//������ڷ����˱仯
			rtc_flag.r.rtcIsUpdate = calendar.w_date&0x1f;		//��¼�µ�����
			memset(ctrl_dev.cache, 0, sizeof(CACHE_SIZE));		//��������
			sprintf(ctrl_dev.cache, "%d/%02d/%02d", calendar.w_year, calendar.w_month, calendar.w_date);
			printf("%s\r\n", ctrl_dev.cache);
		}
		
		memset(ctrl_dev.cache, 0, sizeof(CACHE_SIZE));
		sprintf(ctrl_dev.cache, "%02d:%02d:%02d", calendar.hour, calendar.min, calendar.sec);
		printf("%s\r\n", ctrl_dev.cache);
		myfree(ctrl_dev.cache);
		
		if(rtc_flag.r.rtcIsCheck != calendar.hour){				//ÿСʱ����һ����Ϣ
			rtc_flag.r.rtcIsCheck = calendar.hour;				//��Ǹ���
			Esp12_SetRequestPlan(CURR_WEATHER);
			Esp12_SetRequestPlan(FUTURE_WEATHER);
			Esp12_SetRequestPlan(EPIDEMIC);
			Esp12_SetRequestPlan(VACCINE);
#if LV_DEBUG	
			
#endif
		}
	}
}

void ctrl_Init(void)
{
	memset(&ctrl_dev, 0, sizeof(ctrl_dev));
	ctrl_dev.lcdBacklight = 1000;		//��ʼ����
	rtc_flag.w = 0;
}
