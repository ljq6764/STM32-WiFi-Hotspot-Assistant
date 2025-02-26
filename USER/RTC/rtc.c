#include "usart1.h"
#include "rtc.h" 		    
#include "systick.h"
#include "control.h"
	   
_calendar_obj calendar;//ʱ�ӽṹ�� 
/*********************************************************************************************************
* �� �� �� : Rtc_Init
* ����˵�� : ʵʱʱ������
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��ʼ��RTCʱ�ӣ�ͬʱ���ʱ���Ƿ���������BKP->DR1���ڱ����Ƿ��һ�����õ�����
*********************************************************************************************************/ 
void Rtc_Init(void)
{
	unsigned char temp = 0;
	NVIC_InitTypeDef NVIC_InitStructure = {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��   
	PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ�������  

	if(BKP_ReadBackupRegister(BKP_DR1) != 0x5050){		//��ָ���ĺ󱸼Ĵ����ж�������: 			
		BKP_DeInit();				//��λ�������� 	
		RCC_LSEConfig(RCC_LSE_ON);	//�����ⲿ���پ���(LSE),ʹ��������پ���
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET && temp<250){			//���ָ����RCC��־λ�������,�ȴ����پ������
			temp++;
			Delay_Ms(10);
		}

		if(temp >= 250){			//��ʼ��ʱ��ʧ��,����������	
			return;	
		}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);				//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);								//ʹ��RTCʱ��  
		RTC_WaitForLastTask();								//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForSynchro();								//�ȴ�RTC�Ĵ���ͬ��  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);					//ʹ��RTC���ж�
		RTC_WaitForLastTask();								//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_EnterConfigMode();								// ��������	
		RTC_SetPrescaler(32767); 							//����RTCԤ��Ƶ��ֵ
		RTC_WaitForLastTask();								//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_SetCurrentDateTime(2021, 11, 2, 11, 59, 55); 	//����ʱ��	
		RTC_WaitForLastTask();								//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_ExitConfigMode(); 								//�˳�����ģʽ  
		BKP_WriteBackupRegister(BKP_DR1, 0X5050);			//��ָ���ĺ󱸼Ĵ�����д���û���������
	}else{													//ϵͳ������ʱ
		RTC_WaitForSynchro();								//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_ITConfig(RTC_IT_SEC, ENABLE);					//ʹ��RTC���ж�
		RTC_WaitForLastTask();								//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	}
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;				//RTCȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�1λ,�����ȼ�3λ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ�ܸ�ͨ���ж�
	NVIC_Init(&NVIC_InitStructure);								//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	    				     
	RTC_GetCurrentDateTime();									//����ʱ��	
}	
/*********************************************************************************************************
* �� �� �� : RTC_IRQHandler
* ����˵�� : RTCʱ���ж�
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ÿ�봥��һ��
*********************************************************************************************************/ 
void RTC_IRQHandler(void)
{		 
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET){					
		RTC_GetCurrentDateTime();										
		rtc_flag.r.rtcIsUpdateTime = true;				//��Ǹ���
 	}else if(RTC_GetITStatus(RTC_IT_ALR)!= RESET){		
		RTC_ClearITPendingBit(RTC_IT_ALR);				
		RTC_GetCurrentDateTime();				
		printf("Alarm Time:%d-%d-%d %d:%d:%d\n",calendar.w_year,calendar.w_month,
						calendar.w_date,calendar.hour,calendar.min,calendar.sec);		
  	} 				  								 
	RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW);	
	RTC_WaitForLastTask();	  	    						 	   	 
}
/*********************************************************************************************************
* �� �� �� : Is_Leap_Year
* ����˵�� : �ж��Ƿ������꺯��
* ��    �� : year�����
* �� �� ֵ : 1�����ꣻ0��������
* ��    ע : �·�   1  2  3  4  5  6  7  8  9  10 11 12
			 ����   31 29 31 30 31 30 31 31 30 31 30 31
			 ������ 31 28 31 30 31 30 31 31 30 31 30 31
*********************************************************************************************************/ 
unsigned char isLeapYear(unsigned short year)
{			  
	if(year%4 == 0){ 				//�����ܱ�4����
		if(year%100 == 0){ 
			if(year%400 == 0){
				return 1;		//�����00��β,��Ҫ�ܱ�400���� 	   
			}else return 0;   
		}else return 1;   
	}else return 0;	
}	 		
/*********************************************************************************************************
* �� �� �� : dateToSecCount
* ����˵�� : �����������ת��Ϊ����
* ��    �� : syear���ꣻsmon���£�sday���գ�hour��ʱ��min���֣�sec����
* �� �� ֵ : ת���������
* ��    ע : ��
*********************************************************************************************************/ 											 
unsigned char const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}; 				//���������ݱ�	  
unsigned char const mon_table[12]  = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};	//ƽ����·����ڱ�
unsigned int dateToSecCount(unsigned short syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec)
{
	unsigned int seccount = 0;
	unsigned short t;
	
	for(t=1970; t<syear; t++){			//��������ݵ��������
		if(isLeapYear(t)){
			seccount += 31622400;		//�����������
		}else{
			seccount += 31536000;		//ƽ���������
		}
	}
	smon -= 1;
	for(t=0; t<smon; t++){	   			//��ǰ���·ݵ����������
		seccount += (unsigned int)mon_table[t] * 86400;	//�·����������
		if(isLeapYear(syear) && t==1){
			seccount+=86400;			//����2�·�����һ���������	   
		}
	}
	seccount += (unsigned int)(sday-1) * 86400;	//��ǰ�����ڵ���������� 
	seccount += (unsigned int)hour * 3600;		//Сʱ������
    seccount += (unsigned int)min * 60;	 		//����������
	seccount += sec;							//�������Ӽ���ȥ
	
	return seccount;
}
/*********************************************************************************************************
* �� �� �� : RTC_Set
* ����˵�� : ����RTC��ʱ��
* ��    �� : syear���ꣻsmon���£�sday���գ�hour��ʱ��min���֣�sec����
* �� �� ֵ : 0�ɹ����������������
* ��    ע : 1970~2099��Ϊ�Ϸ����
*********************************************************************************************************/ 
unsigned char RTC_SetCurrentDateTime(unsigned short syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec)
{
	if(syear<1970 || syear>2099){
		return 1;	   
	}
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��  
	PWR_BackupAccessCmd(ENABLE);												//ʹ��RTC�ͺ󱸼Ĵ������� 
	RTC_ITConfig(RTC_IT_SEC, DISABLE);
	RTC_WaitForLastTask();														//�ȴ����һ�ζ�RTC�Ĵ�����д�������
	RTC_SetCounter(dateToSecCount(syear, smon, sday, hour, min, sec));			//����RTC��������ֵ
	RTC_WaitForLastTask();														//�ȴ����һ�ζ�RTC�Ĵ�����д�������  
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
	return 0;	    
}
/*********************************************************************************************************
* �� �� �� : RTC_Alarm_Set
* ����˵�� : ��ʼ������ʱ��
* ��    �� : syear���ꣻsmon���£�sday���գ�hour��ʱ��min���֣�sec����
* �� �� ֵ : 0�ɹ����������������
* ��    ע : 1970~2099��Ϊ�Ϸ����
*********************************************************************************************************/ 
unsigned char RTC_Alarm_Set(unsigned short syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec)
{
	if(syear<1970 || syear>2099){
		return 1;	
	}	
			    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��   
	PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ�������  
	RTC_SetAlarm(dateToSecCount(syear, smon, sday, hour, min, sec));	//��������ʱ��
	RTC_WaitForLastTask();			//�ȴ����һ�ζ�RTC�Ĵ�����д�������  	
	
	return 0;	    
}/*********************************************************************************************************
* �� �� �� : RTC_GetCurrentDateTime
* ����˵�� : ��ȡ��ǰʱ��
* ��    �� : ��
* �� �� ֵ : 0���ɹ����������������
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char RTC_GetCurrentDateTime(void)
{
	static unsigned short daycnt = 0;
	unsigned int timecount; 
	unsigned int temp;
	unsigned short temp1;	  
	
    timecount = RTC_GetCounter();	 
 	temp = timecount / 86400;   //�õ�����(��������Ӧ��)
	if(daycnt != temp)			//����һ����
	{	  
		daycnt = temp;
		temp1 = 1970;			//��1970�꿪ʼ
		while(temp >= 365){				 
			if(isLeapYear(temp1)){		//������
				if(temp >= 366){
					temp -= 366;		//�����������
				}else{
					temp1++;
					break;
				}  
			}else{
				temp -= 365;	 		//ƽ�� 
			}
			temp1++;  
		}   
		calendar.w_year = temp1;		//�õ����
		temp1 = 0;
		while(temp>=28){				//������һ����
		
			if(isLeapYear(calendar.w_year) && temp1==1)	//�����ǲ�������/2�·�
			{
				if(temp >= 29){
					temp -= 29;					//�����������
				}else break; 
			}else{
				if(temp >= mon_table[temp1]){
					temp -= mon_table[temp1];	//ƽ��
				}else break;
			}
			temp1++;  
		}
		calendar.w_month = temp1 + 1;	//�õ��·�
		calendar.w_date = temp + 1;  	//�õ����� 
	}
	temp = timecount % 86400;     		//�õ�������   	   
	calendar.hour = temp / 3600;     	//Сʱ
	calendar.min = (temp%3600) / 60; 	//����	
	calendar.sec = (temp%3600) % 60; 	//����
	calendar.week = RTC_Get_Week(calendar.w_year, calendar.w_month, calendar.w_date);//��ȡ����
	
	return 0;
}	 
/*********************************************************************************************************
* �� �� �� : RTC_Get_Week
* ����˵�� : ��ȡ��ǰ���ڵ�����
* ��    �� : syear���ꣻsmon���£�sday����
* �� �� ֵ : ���ں�
* ��    ע : 1901~2099��Ϊ�Ϸ����
*********************************************************************************************************/ 																					 
unsigned char RTC_Get_Week(unsigned short year, unsigned char month, unsigned char day)
{	
	unsigned short temp2;
	unsigned char yearH, yearL;
	
	yearH = year/100;	
	yearL = year%100; 
	// ���Ϊ21����,�������100  
	if(yearH > 19){
		yearL += 100;
	}
	// ����������ֻ��1900��֮���  
	temp2 = yearL + yearL / 4;
	temp2 = temp2 % 7; 
	temp2 = temp2 + day + table_week[month - 1];
	if(yearL%4==0 && month<3){
		temp2--;
	}
	
	return (temp2%7);
}			  

