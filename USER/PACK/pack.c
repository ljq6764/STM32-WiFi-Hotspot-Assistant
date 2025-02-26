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

static int flag1=1;									//ҳλ���
unsigned short keyTime = 0;
TOUCH_TypeDef touch;
int pic=0;
int Alarm=1;
int Alarm2=1;
int hour=10;
int	min=10;
int	sec=10;
int little=1;
void one()												//ʱ����ҳ����
{
	if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,0);				//��һҳ����ʾͼƬ
		pic++;
	}
	{
		rtc_screen();
		if(0 == Touch_Scan(&touch,0))
		{
			if(touch.xval<240)
			{
				flag1=2;pic=0;Syn6658_SyntheticVoiceCmd("����",1);
			}
			if(touch.xval>240)
			{
				flag1=3;pic=0;Syn6658_SyntheticVoiceCmd("��������",1);
			}		
		}
	}
}

void two()													//��������
	{
		if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,76);				//����ʾͼƬ
		LCD_Fill(0,140,132,320,LIGHTBLUE);
		LCD_Fill(325,140,460,320,LIGHTBLUE);
		pic++;
	}
		if(0 == Touch_Scan(&touch,0))		//������Ļ�ж�
			{if(touch.xval<240 && touch.yval<160)
				{flag1=1;pic=0;Syn6658_SyntheticVoiceCmd("����",1);}
			if(touch.xval>240 && touch.yval<160)
			{Syn6658_SyntheticVoiceCmd("����������С�꣬���żǵô�ɡ��Ԥ������Ԥ��С��",1);
			}
		}
			
	}

void three()												//���ܽ���
{
	if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,152);				//����ʾͼƬ
		pic++;
	}
		if(0 == Touch_Scan(&touch,0))
		{
			if(touch.xval<240 && touch.yval<160) // ����
			{
					flag1=4;pic=0;Syn6658_SyntheticVoiceCmd("��������",1);
			}
			if(touch.xval>240 && touch.yval<160) // ����
			{
					flag1=5;pic=0;Syn6658_SyntheticVoiceCmd("��������",1);
			}
			if(touch.xval<240 && touch.yval>160) // ����
			{
					flag1=6;pic=0;Syn6658_SyntheticVoiceCmd("�ҵ�����",1);
			}
			if(touch.xval>240 && touch.yval>160) // ����
			{
					flag1=7;pic=0;Syn6658_SyntheticVoiceCmd("�����",1);
			}
		}
}

void four()                      	//����  						  
{
	if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,228);				//����ʾͼƬ
		LCD_ShowIntNum(240,145,Alarm2,1,BLACK,WHITE,16);
		pic++;
	}
		if(0 == Touch_Scan(&touch,1))								//��ⴥ��
		{
			if(Alarm2>1)
			{
				if(touch.xval>35&&touch.xval<120&&touch.yval>105&&touch.yval<190)//��
				{
					Alarm2--;Alarm=Alarm2;
					Syn6658_SyntheticVoiceCmd("��",1);		
				}
			}
			if(Alarm2<9)
			{
				if(touch.xval>360&&touch.xval<450&&touch.yval>105&&touch.yval<190)//��
				{
					Alarm2++;Alarm=Alarm2;
					Syn6658_SyntheticVoiceCmd("��",1);		
				}
			}
			LCD_ShowIntNum(240,145,Alarm2,1,BLACK,WHITE,16);
			if(touch.xval>0&&touch.xval<100&&touch.yval>0&&touch.yval<90)  
					{flag1=3;pic=0;Syn6658_SyntheticVoiceCmd("����",1);}
	}
}		
	
void five()														//����
{
	if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,304);				//����ʾͼƬ
		LCD_ShowIntNum(170,150,hour,2,BLACK,WHITE,16);
		LCD_ShowIntNum(275,150,min,2,BLACK,WHITE,16);
		LCD_ShowIntNum(380,150,sec,2,BLACK,WHITE,16);
		pic++;
	}
	if(0 == Touch_Scan(&touch,1))								//��ⴥ��
	{
		//��
		if(hour<23&&hour>0)
		{
			if(touch.xval>130&&touch.xval<220&&touch.yval>40&&touch.yval<120)//�� ��һ��
			{hour++;LCD_ShowIntNum(170,150,hour,2,BLACK,WHITE,16);Syn6658_SyntheticVoiceCmd("��",1);}
		}
		if(min<59&&min>0)
		{
			if(touch.xval>225&&touch.xval<325&&touch.yval>40&&touch.yval<120)//��    �ڶ���
			{min++;LCD_ShowIntNum(275,150,min,2,BLACK,WHITE,16);Syn6658_SyntheticVoiceCmd("��",1);}}
		if(sec<59&&sec>0)
		{
			if(touch.xval>330&&touch.xval<430&&touch.yval>40&&touch.yval<120)//��  ������
			{sec++;LCD_ShowIntNum(380,150,sec,2,BLACK,WHITE,16);Syn6658_SyntheticVoiceCmd("��",1);}}
		
			
		//��
		if(hour<24&&hour>1)
		{
			if(touch.xval>130&&touch.xval<220&&touch.yval>203&&touch.yval<280)//��  һ
			{hour--;LCD_ShowIntNum(170,150,hour,2,BLACK,WHITE,16);Syn6658_SyntheticVoiceCmd("��",1);}
		}
		if(min<60&&min>1)
		{
			if(touch.xval>225&&touch.xval<325&&touch.yval>203&&touch.yval<280)//��  ��
			{min--;LCD_ShowIntNum(275,150,min,2,BLACK,WHITE,16);Syn6658_SyntheticVoiceCmd("��",1);}}
		if(sec<60&&sec>1)
		{
			if(touch.xval>330&&touch.xval<430&&touch.yval>203&&touch.yval<280)//��  ��
			{sec--;LCD_ShowIntNum(380,150,sec,2,BLACK,WHITE,16);Syn6658_SyntheticVoiceCmd("��",1);}}
		
		if(touch.xval>0&&touch.xval<100&&touch.yval>0&&touch.yval<90)			//����
		{flag1=3;pic=0;Syn6658_SyntheticVoiceCmd("����",1);}
	
			if(touch.xval>40&&touch.xval<110&&touch.yval>130&&touch.yval<196)//С����  ��
		{
			if(little==1)
			{
			Syn6658_SyntheticVoiceCmd("������",1);little=0;
			}
			else{Syn6658_SyntheticVoiceCmd("�ر�����",1);little=1;}
		}
	}
	
}


void six()							//							
{
	if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,380);				//����ʾͼƬ
		LCD_ShowString(10,270,"wifiaccount:  ww",BLACK, WHITE,16);
		LCD_ShowString(10,300,"wifipossword:  123456789",BLACK, WHITE,16);
		pic++;
	}
	if(0 == Touch_Scan(&touch,1))								//��ⴥ�ز�������ҳ
	{
		if(touch.xval>240 && touch.yval>160) // ����
				{flag1=3;pic=0;Syn6658_SyntheticVoiceCmd("����",1);}
		if(touch.xval<240 && touch.yval<160)  //����
				{flag1=1;pic=0;Syn6658_SyntheticVoiceCmd("������ҳ",1);}
	}
}			


void seven()			//�����	
{
	if(pic==0)
	{
		LCD_ShowFlashPic(0,0,480,320,456);				//����ʾͼƬ
		pic++;
	}
	counter();
	if(pic==0)
	{flag1=3;}
}


void ALL_Allot(void)										//���߳�
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


