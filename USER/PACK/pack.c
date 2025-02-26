#include "led.h"
#include "key.h"
#include "beep.h"
#include "systick.h"
#include "usart1.h"
#include "syn6658.h"
#include "w25qxx.h"
#include "at24c02.h"
#include "timer.h"
#include "lcd.h"
#include "touch.h"
#include "esp12.h"
#include "malloc.h"
#include "control.h"
#include "PACK.h"
#include "rtc.h"
#include "rtc_screen.h"
#include "computer.h"

static int flag1=1;									//页位标记
unsigned short keyTime = 0;
TOUCH_TypeDef touch;
int pic=0;
int Alarm=1;
int Alarm2=1;
int hour=10;
int	min=10;
int	sec=10;
int little=1;
void one()												//时钟首页界面
{
	if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,0);				//第一页先显示图片
		pic++;
	}
	{
		rtc_screen();
		if(0 == Touch_Scan(&touch,0))
		{
			if(touch.xval<240)
			{
				flag1=2;pic=0;Syn6658_SyntheticVoiceCmd("天气",1);
			}
			if(touch.xval>240)
			{
				flag1=3;pic=0;Syn6658_SyntheticVoiceCmd("功能设置",1);
			}		
		}
	}
}

void two()													//天气界面
	{
		if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,76);				//先显示图片
		LCD_Fill(0,140,132,320,LIGHTBLUE);
		LCD_Fill(325,140,460,320,LIGHTBLUE);
		pic++;
	}
		if(0 == Touch_Scan(&touch,0))		//触碰屏幕判定
			{if(touch.xval<240 && touch.yval<160)
				{flag1=1;pic=0;Syn6658_SyntheticVoiceCmd("返回",1);}
			if(touch.xval>240 && touch.yval<160)
			{Syn6658_SyntheticVoiceCmd("南宁今天有小雨，出门记得带伞。预计明天预计小雨",1);
			}
		}
			
	}

void three()												//功能界面
{
	if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,152);				//先显示图片
		pic++;
	}
		if(0 == Touch_Scan(&touch,0))
		{
			if(touch.xval<240 && touch.yval<160) // 左上
			{
					flag1=4;pic=0;Syn6658_SyntheticVoiceCmd("播报设置",1);
			}
			if(touch.xval>240 && touch.yval<160) // 右上
			{
					flag1=5;pic=0;Syn6658_SyntheticVoiceCmd("闹钟设置",1);
			}
			if(touch.xval<240 && touch.yval>160) // 左下
			{
					flag1=6;pic=0;Syn6658_SyntheticVoiceCmd("我的网络",1);
			}
			if(touch.xval>240 && touch.yval>160) // 右下
			{
					flag1=7;pic=0;Syn6658_SyntheticVoiceCmd("计算机",1);
			}
		}
}

void four()                      	//播报  						  
{
	if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,228);				//先显示图片
		LCD_ShowIntNum(240,145,Alarm2,1,BLACK,WHITE,16);
		pic++;
	}
		if(0 == Touch_Scan(&touch,1))								//检测触控
		{
			if(Alarm2>1)
			{
				if(touch.xval>35&&touch.xval<120&&touch.yval>105&&touch.yval<190)//减
				{
					Alarm2--;Alarm=Alarm2;
					Syn6658_SyntheticVoiceCmd("减",1);		
				}
			}
			if(Alarm2<9)
			{
				if(touch.xval>360&&touch.xval<450&&touch.yval>105&&touch.yval<190)//加
				{
					Alarm2++;Alarm=Alarm2;
					Syn6658_SyntheticVoiceCmd("加",1);		
				}
			}
			LCD_ShowIntNum(240,145,Alarm2,1,BLACK,WHITE,16);
			if(touch.xval>0&&touch.xval<100&&touch.yval>0&&touch.yval<90)  
					{flag1=3;pic=0;Syn6658_SyntheticVoiceCmd("返回",1);}
	}
}		
	
void five()														//闹钟
{
	if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,304);				//先显示图片
		LCD_ShowIntNum(170,150,hour,2,BLACK,WHITE,16);
		LCD_ShowIntNum(275,150,min,2,BLACK,WHITE,16);
		LCD_ShowIntNum(380,150,sec,2,BLACK,WHITE,16);
		pic++;
	}
	if(0 == Touch_Scan(&touch,1))								//检测触控
	{
		//加
		if(hour<23&&hour>0)
		{
			if(touch.xval>130&&touch.xval<220&&touch.yval>40&&touch.yval<120)//加 第一个
			{hour++;LCD_ShowIntNum(170,150,hour,2,BLACK,WHITE,16);Syn6658_SyntheticVoiceCmd("加",1);}
		}
		if(min<59&&min>0)
		{
			if(touch.xval>225&&touch.xval<325&&touch.yval>40&&touch.yval<120)//加    第二个
			{min++;LCD_ShowIntNum(275,150,min,2,BLACK,WHITE,16);Syn6658_SyntheticVoiceCmd("加",1);}}
		if(sec<59&&sec>0)
		{
			if(touch.xval>330&&touch.xval<430&&touch.yval>40&&touch.yval<120)//加  第三个
			{sec++;LCD_ShowIntNum(380,150,sec,2,BLACK,WHITE,16);Syn6658_SyntheticVoiceCmd("加",1);}}
		
			
		//减
		if(hour<24&&hour>1)
		{
			if(touch.xval>130&&touch.xval<220&&touch.yval>203&&touch.yval<280)//减  一
			{hour--;LCD_ShowIntNum(170,150,hour,2,BLACK,WHITE,16);Syn6658_SyntheticVoiceCmd("减",1);}
		}
		if(min<60&&min>1)
		{
			if(touch.xval>225&&touch.xval<325&&touch.yval>203&&touch.yval<280)//减  二
			{min--;LCD_ShowIntNum(275,150,min,2,BLACK,WHITE,16);Syn6658_SyntheticVoiceCmd("减",1);}}
		if(sec<60&&sec>1)
		{
			if(touch.xval>330&&touch.xval<430&&touch.yval>203&&touch.yval<280)//减  三
			{sec--;LCD_ShowIntNum(380,150,sec,2,BLACK,WHITE,16);Syn6658_SyntheticVoiceCmd("减",1);}}
		
		if(touch.xval>0&&touch.xval<100&&touch.yval>0&&touch.yval<90)			//返回
		{flag1=3;pic=0;Syn6658_SyntheticVoiceCmd("返回",1);}
	
			if(touch.xval>40&&touch.xval<110&&touch.yval>130&&touch.yval<196)//小闹钟  三
		{
			if(little==1)
			{
			Syn6658_SyntheticVoiceCmd("打开闹钟",1);little=0;
			}
			else{Syn6658_SyntheticVoiceCmd("关闭闹钟",1);little=1;}
		}
	}
	
}


void six()							//							
{
	if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,380);				//先显示图片
		LCD_ShowString(10,270,"wifiaccount:  ww",BLACK, WHITE,16);
		LCD_ShowString(10,300,"wifipossword:  123456789",BLACK, WHITE,16);
		pic++;
	}
	if(0 == Touch_Scan(&touch,1))								//检测触控并返回首页
	{
		if(touch.xval>240 && touch.yval>160) // 右下
				{flag1=3;pic=0;Syn6658_SyntheticVoiceCmd("返回",1);}
		if(touch.xval<240 && touch.yval<160)  //左上
				{flag1=1;pic=0;Syn6658_SyntheticVoiceCmd("返回首页",1);}
	}
}			


void seven()			//计算机	
{
	if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,456);				//先显示图片
		pic++;
	}
	counter();
	if(pic==0)
	{flag1=3;}
}


void ALL_Allot(void)										//总线程
{
	switch(flag1)
	{
		case 1:one();break;
		case 2:two();break;
		case 3:three();break;
		case 4:four();break;
		case 5:five();break;
		case 6:six();break;
		case 7:seven();break;
	}
	return;
}


