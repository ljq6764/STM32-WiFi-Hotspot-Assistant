#include "usart1.h"
#include "rtc.h" 		    
#include "systick.h"
#include "control.h"
	   
_calendar_obj calendar;//时钟结构体 
/*********************************************************************************************************
* 函 数 名 : Rtc_Init
* 功能说明 : 实时时钟配置
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 初始化RTC时钟，同时检测时钟是否工作正常，BKP->DR1用于保存是否第一次配置的设置
*********************************************************************************************************/ 
void Rtc_Init(void)
{
	unsigned char temp = 0;
	NVIC_InitTypeDef NVIC_InitStructure = {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问  

	if(BKP_ReadBackupRegister(BKP_DR1) != 0x5050){		//从指定的后备寄存器中读出数据: 			
		BKP_DeInit();				//复位备份区域 	
		RCC_LSEConfig(RCC_LSE_ON);	//设置外部低速晶振(LSE),使用外设低速晶振
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET && temp<250){			//检查指定的RCC标志位设置与否,等待低速晶振就绪
			temp++;
			Delay_Ms(10);
		}

		if(temp >= 250){			//初始化时钟失败,晶振有问题	
			return;	
		}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);				//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);								//使能RTC时钟  
		RTC_WaitForLastTask();								//等待最近一次对RTC寄存器的写操作完成
		RTC_WaitForSynchro();								//等待RTC寄存器同步  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);					//使能RTC秒中断
		RTC_WaitForLastTask();								//等待最近一次对RTC寄存器的写操作完成
		RTC_EnterConfigMode();								// 允许配置	
		RTC_SetPrescaler(32767); 							//设置RTC预分频的值
		RTC_WaitForLastTask();								//等待最近一次对RTC寄存器的写操作完成
		RTC_SetCurrentDateTime(2021, 11, 2, 11, 59, 55); 	//设置时间	
		RTC_WaitForLastTask();								//等待最近一次对RTC寄存器的写操作完成
		RTC_ExitConfigMode(); 								//退出配置模式  
		BKP_WriteBackupRegister(BKP_DR1, 0X5050);			//向指定的后备寄存器中写入用户程序数据
	}else{													//系统继续计时
		RTC_WaitForSynchro();								//等待最近一次对RTC寄存器的写操作完成
		RTC_ITConfig(RTC_IT_SEC, ENABLE);					//使能RTC秒中断
		RTC_WaitForLastTask();								//等待最近一次对RTC寄存器的写操作完成
	}
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;				//RTC全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//先占优先级1位,从优先级3位
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//先占优先级0位,从优先级4位
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能该通道中断
	NVIC_Init(&NVIC_InitStructure);								//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	    				     
	RTC_GetCurrentDateTime();									//更新时间	
}	
/*********************************************************************************************************
* 函 数 名 : RTC_IRQHandler
* 功能说明 : RTC时钟中断
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 每秒触发一次
*********************************************************************************************************/ 
void RTC_IRQHandler(void)
{		 
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET){					
		RTC_GetCurrentDateTime();										
		rtc_flag.r.rtcIsUpdateTime = true;				//标记更新
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
* 函 数 名 : Is_Leap_Year
* 功能说明 : 判断是否是闰年函数
* 形    参 : year：年份
* 返 回 值 : 1：闰年；0：非闰年
* 备    注 : 月份   1  2  3  4  5  6  7  8  9  10 11 12
			 闰年   31 29 31 30 31 30 31 31 30 31 30 31
			 非闰年 31 28 31 30 31 30 31 31 30 31 30 31
*********************************************************************************************************/ 
unsigned char isLeapYear(unsigned short year)
{			  
	if(year%4 == 0){ 				//必须能被4整除
		if(year%100 == 0){ 
			if(year%400 == 0){
				return 1;		//如果以00结尾,还要能被400整除 	   
			}else return 0;   
		}else return 1;   
	}else return 0;	
}	 		
/*********************************************************************************************************
* 函 数 名 : dateToSecCount
* 功能说明 : 把输入的日期转换为秒钟
* 形    参 : syear：年；smon：月；sday：日；hour：时；min：分；sec：秒
* 返 回 值 : 转换后的秒数
* 备    注 : 无
*********************************************************************************************************/ 											 
unsigned char const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5}; 				//月修正数据表	  
unsigned char const mon_table[12]  = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};	//平年的月份日期表
unsigned int dateToSecCount(unsigned short syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec)
{
	unsigned int seccount = 0;
	unsigned short t;
	
	for(t=1970; t<syear; t++){			//把所有年份的秒钟相加
		if(isLeapYear(t)){
			seccount += 31622400;		//闰年的秒钟数
		}else{
			seccount += 31536000;		//平年的秒钟数
		}
	}
	smon -= 1;
	for(t=0; t<smon; t++){	   			//把前面月份的秒钟数相加
		seccount += (unsigned int)mon_table[t] * 86400;	//月份秒钟数相加
		if(isLeapYear(syear) && t==1){
			seccount+=86400;			//闰年2月份增加一天的秒钟数	   
		}
	}
	seccount += (unsigned int)(sday-1) * 86400;	//把前面日期的秒钟数相加 
	seccount += (unsigned int)hour * 3600;		//小时秒钟数
    seccount += (unsigned int)min * 60;	 		//分钟秒钟数
	seccount += sec;							//最后的秒钟加上去
	
	return seccount;
}
/*********************************************************************************************************
* 函 数 名 : RTC_Set
* 功能说明 : 设置RTC的时间
* 形    参 : syear：年；smon：月；sday：日；hour：时；min：分；sec：秒
* 返 回 值 : 0成功；其他：错误代码
* 备    注 : 1970~2099年为合法年份
*********************************************************************************************************/ 
unsigned char RTC_SetCurrentDateTime(unsigned short syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec)
{
	if(syear<1970 || syear>2099){
		return 1;	   
	}
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
	PWR_BackupAccessCmd(ENABLE);												//使能RTC和后备寄存器访问 
	RTC_ITConfig(RTC_IT_SEC, DISABLE);
	RTC_WaitForLastTask();														//等待最近一次对RTC寄存器的写操作完成
	RTC_SetCounter(dateToSecCount(syear, smon, sday, hour, min, sec));			//设置RTC计数器的值
	RTC_WaitForLastTask();														//等待最近一次对RTC寄存器的写操作完成  
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
	return 0;	    
}
/*********************************************************************************************************
* 函 数 名 : RTC_Alarm_Set
* 功能说明 : 初始化闹钟时间
* 形    参 : syear：年；smon：月；sday：日；hour：时；min：分；sec：秒
* 返 回 值 : 0成功；其他：错误代码
* 备    注 : 1970~2099年为合法年份
*********************************************************************************************************/ 
unsigned char RTC_Alarm_Set(unsigned short syear,unsigned char smon,unsigned char sday,unsigned char hour,unsigned char min,unsigned char sec)
{
	if(syear<1970 || syear>2099){
		return 1;	
	}	
			    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问  
	RTC_SetAlarm(dateToSecCount(syear, smon, sday, hour, min, sec));	//设置闹钟时间
	RTC_WaitForLastTask();			//等待最近一次对RTC寄存器的写操作完成  	
	
	return 0;	    
}/*********************************************************************************************************
* 函 数 名 : RTC_GetCurrentDateTime
* 功能说明 : 获取当前时间
* 形    参 : 无
* 返 回 值 : 0：成功；其他：错误代码
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char RTC_GetCurrentDateTime(void)
{
	static unsigned short daycnt = 0;
	unsigned int timecount; 
	unsigned int temp;
	unsigned short temp1;	  
	
    timecount = RTC_GetCounter();	 
 	temp = timecount / 86400;   //得到天数(秒钟数对应的)
	if(daycnt != temp)			//超过一天了
	{	  
		daycnt = temp;
		temp1 = 1970;			//从1970年开始
		while(temp >= 365){				 
			if(isLeapYear(temp1)){		//是闰年
				if(temp >= 366){
					temp -= 366;		//闰年的秒钟数
				}else{
					temp1++;
					break;
				}  
			}else{
				temp -= 365;	 		//平年 
			}
			temp1++;  
		}   
		calendar.w_year = temp1;		//得到年份
		temp1 = 0;
		while(temp>=28){				//超过了一个月
		
			if(isLeapYear(calendar.w_year) && temp1==1)	//当年是不是闰年/2月份
			{
				if(temp >= 29){
					temp -= 29;					//闰年的秒钟数
				}else break; 
			}else{
				if(temp >= mon_table[temp1]){
					temp -= mon_table[temp1];	//平年
				}else break;
			}
			temp1++;  
		}
		calendar.w_month = temp1 + 1;	//得到月份
		calendar.w_date = temp + 1;  	//得到日期 
	}
	temp = timecount % 86400;     		//得到秒钟数   	   
	calendar.hour = temp / 3600;     	//小时
	calendar.min = (temp%3600) / 60; 	//分钟	
	calendar.sec = (temp%3600) % 60; 	//秒钟
	calendar.week = RTC_Get_Week(calendar.w_year, calendar.w_month, calendar.w_date);//获取星期
	
	return 0;
}	 
/*********************************************************************************************************
* 函 数 名 : RTC_Get_Week
* 功能说明 : 获取当前日期的星期
* 形    参 : syear：年；smon：月；sday：日
* 返 回 值 : 星期号
* 备    注 : 1901~2099年为合法年份
*********************************************************************************************************/ 																					 
unsigned char RTC_Get_Week(unsigned short year, unsigned char month, unsigned char day)
{	
	unsigned short temp2;
	unsigned char yearH, yearL;
	
	yearH = year/100;	
	yearL = year%100; 
	// 如果为21世纪,年份数加100  
	if(yearH > 19){
		yearL += 100;
	}
	// 所过闰年数只算1900年之后的  
	temp2 = yearL + yearL / 4;
	temp2 = temp2 % 7; 
	temp2 = temp2 + day + table_week[month - 1];
	if(yearL%4==0 && month<3){
		temp2--;
	}
	
	return (temp2%7);
}			  

