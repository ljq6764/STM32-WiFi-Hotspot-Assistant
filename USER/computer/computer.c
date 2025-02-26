#include "computer.h"
#include "lcd.h"
#include "touch.h"
#include "syn6658.h"
#include "systick.h"
#include "PACK.h"
TOUCH_TypeDef xy;
int munber[4]={0};
int i=0;
unsigned int touch_x;
unsigned int touch_y;
//计算器界面显示


void counter(void)
{
	char data=0;
//	char data1[64] = {0};
	
	TOUCH_TypeDef t_xy;
	 if(0==Touch_Scan(&t_xy, 0)){
		  touch_x = t_xy.xval;
			touch_y = t_xy.yval;
		if(touch_x>0&&touch_x<115&&touch_y>88&&touch_y<144)
		{
			munber[i]=munber[i]*10+7;
			sprintf(&data,"%d",munber[i]);
			Syn6658_SyntheticVoiceCmd("7",1);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
		}
		if(touch_x>0&&touch_x<115&&touch_y>146&&touch_y<202)
		{
			munber[i]=munber[i]*10+4;
			sprintf(&data,"%d",munber[i]);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			Syn6658_SyntheticVoiceCmd("4",1);
		}
		if(touch_x>0&&touch_x<115&&touch_y>204&&touch_y<260)
		{
			munber[i]=munber[i]*10+1;
			sprintf(&data,"%d",munber[i]);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			Syn6658_SyntheticVoiceCmd("1",1);
		}
		if(touch_x>0&&touch_x<115&&touch_y>262&&touch_y<320)
		{
			munber[i]=munber[i]*10+0;
			sprintf(&data,"%d",munber[i]);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			Syn6658_SyntheticVoiceCmd("0",1);
		}
		
		if(touch_x>120&&touch_x<235&&touch_y>88&&touch_y<144)
		{
			munber[i]=munber[i]*10+8;
			sprintf(&data,"%d",munber[i]);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			Syn6658_SyntheticVoiceCmd("8",1);
		}
		if(touch_x>120&&touch_x<235&&touch_y>146&&touch_y<202)
		{
			munber[i]=munber[i]*10+5;
			sprintf(&data,"%d",munber[i]);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			Syn6658_SyntheticVoiceCmd("5",1);
		}
		if(touch_x>120&&touch_x<235&&touch_y>204&&touch_y<260)
		{
			munber[i]=munber[i]*10+2;
			sprintf(&data,"%d",munber[i]);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			Syn6658_SyntheticVoiceCmd("2",1);
		}
		
		
		if(touch_x>240&&touch_x<355&&touch_y>88&&touch_y<144)
		{
			munber[i]=munber[i]*10+9;
			sprintf(&data,"%d",munber[i]);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			Syn6658_SyntheticVoiceCmd("9",1);
		}
		if(touch_x>240&&touch_x<355&&touch_y>146&&touch_y<202)
		{
			munber[i]=munber[i]*10+6;
			sprintf(&data,"%d",munber[i]);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			Syn6658_SyntheticVoiceCmd("6",1);
		}
		if(touch_x>240&&touch_x<355&&touch_y>204&&touch_y<260)
		{
			munber[i]=munber[i]*10+3;
			sprintf(&data,"%d",munber[i]);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			Syn6658_SyntheticVoiceCmd("3",1);
		}
		
		if(touch_x>240&&touch_x<355&&touch_y>262&&touch_y<320)
		{
			Syn6658_SyntheticVoiceCmd("加",1);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			munber[1]=1;
			i=2;
		}
		
		if(touch_x>360&&touch_x<480&&touch_y>88&&touch_y<144)
		{
			Syn6658_SyntheticVoiceCmd("除以",1);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			munber[1]=4;
			i=2;
		}
		if(touch_x>360&&touch_x<480&&touch_y>146&&touch_y<202)
		{
			Syn6658_SyntheticVoiceCmd("乘以",1);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			munber[1]=3;
			i=2;
		}
		if(touch_x>360&&touch_x<480&&touch_y>204&&touch_y<260)
		{
			Syn6658_SyntheticVoiceCmd("减",1);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			munber[1]=2;
			i=2;
		}
		
		if(touch_x>360&&touch_x<480&&touch_y>262&&touch_y<320)
		{
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			
	    Syn6658_SyntheticVoiceCmd("等于",1);
			switch(munber[1])
			{
				case 1: munber[3]=munber[0]+munber[2];break;
				case 2: munber[3]=munber[0]-munber[2];break;
				case 3: munber[3]=munber[0]*munber[2];break;
				case 4: munber[3]=munber[0]/munber[2];break;
			}
			sprintf(&data,"%d",munber[3]);
			Syn6658_SyntheticVoiceCmd(&data,1);
			LCD_Show_RectangleData(15, 0, 351, 65,&data, BLACK,WHITE);
			i=0;
		}
		
		if(touch_x>400&&touch_x<480&&touch_y>0&&touch_y<80)
		{
		
			Syn6658_SyntheticVoiceCmd("清除",1);
			i=0;
			memset(munber,0,sizeof(munber));//将数组清零
			LCD_Show_RectangleData(15, 0, 351, 65,"0", BLACK,WHITE);
		}
		if(touch_x>120&&touch_x<235&&touch_y>262&&touch_y<320)
		{
			Syn6658_SyntheticVoiceCmd("返回",1);pic=0;
		}
 }
}
