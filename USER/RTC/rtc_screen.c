#include "rtc_screen.h"
#include "lcd.h"
#include "control.h"
#include "rtc.h"
#include "syn6658.h"
#include "esp12.h"
#include "QMsg.h"
#include "touch.h"
#include "PACK.h"
char  t;
void rtc_screen()
{
		if(t!=calendar.sec)
		{
			t=calendar.sec;
//			printf("%04d/%02d/%02d %02d:%02d:%02d\r\n", calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour ,calendar.min,calendar. sec);
			LCD_ShowIntNum(172,40,calendar.w_year,4,WHITE, ORANGE,16);									  
			LCD_ShowIntNum(205,40,calendar.w_month,2,WHITE, ORANGE,16);									  
			LCD_ShowIntNum(219,40,calendar.w_date,2,WHITE, ORANGE,16);	 
			switch(calendar.week)
			{
				case 0:
					LCD_ShowString(170,60,"Sunday   ",WHITE, ORANGE,16);
					break;
				case 1:
					LCD_ShowString(170,60,"Monday   ",WHITE, ORANGE,16);
					break;
				case 2:
					LCD_ShowString(170,60,"Tuesday  ",WHITE, ORANGE,16);
					break;
				case 3:
					LCD_ShowString(170,60,"Wednesday",WHITE, ORANGE,16);
					break;
				case 4:
					LCD_ShowString(170,60,"Thursday ",WHITE, ORANGE,16);
					break;
				case 5:
					LCD_ShowString(170,60,"Friday   ",WHITE, ORANGE,16);
					break;
				case 6:
					LCD_ShowString(170,60,"Saturday ",WHITE, ORANGE,16);
					break;  
			}
			LCD_ShowIntNum(180,80,calendar.hour,2,WHITE, ORANGE,16);									  
			LCD_ShowIntNum(199,80,calendar.min,2,WHITE, ORANGE,16);									  
			LCD_ShowIntNum(218,80,calendar.sec,2,WHITE, ORANGE,16);
		}

		
}





