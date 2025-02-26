#ifndef __LCD_H
#define __LCD_H

#include "io_bit.h"

#define LCD_Backlight 1		//0:全亮，1:pwm调制

////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义---------------- 
#define	LCD_LED PAout(8) 				//LCD背光    		PA8
 
#define	LCD_CS_SET  GPIOC->BSRR=1<<9    //片选端口  		PC9
#define	LCD_RS_SET	GPIOC->BSRR=1<<8    //数据/命令 		PC8	   
#define	LCD_WR_SET	GPIOC->BSRR=1<<7    //写数据			PC7
#define	LCD_RD_SET	GPIOC->BSRR=1<<6    //读数据			PC6
								    
#define	LCD_CS_CLR  GPIOC->BRR=1<<9     //片选端口  		PC9
#define	LCD_RS_CLR	GPIOC->BRR=1<<8     //数据/命令			PC8	   
#define	LCD_WR_CLR	GPIOC->BRR=1<<7     //写数据			PC7
#define	LCD_RD_CLR	GPIOC->BRR=1<<6     //读数据			PC6   

//PB0~15,作为数据线
#define DATAOUT(x) GPIOB->ODR = x; 	//数据输出
#define DATAIN     GPIOB->IDR;   	//数据输入	

 
//////////////////////////////////////////////////////////////////////
//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左

#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向
	 
//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左	 

#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
#define ORANGE				0xfc60  //橙红色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色 
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)
	    															  
void Lcd_Init(void);			
void LCD_Write_CMD(unsigned short data);
void LCD_Write_Data(unsigned short data);
void LCD_Draw_Point(unsigned short x, unsigned short y, unsigned short color);
unsigned short LCD_ReadPoint(unsigned short x,unsigned short y);
void LCD_Address_Set(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2);
void TP_Draw_Big_Point(unsigned short x,unsigned short y,unsigned short color);
void LCD_Fill(unsigned short xsta, unsigned short ysta, unsigned short xend, unsigned short yend, unsigned short color);
void LCD_Color_Fill(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, const unsigned short *pic);
void LCD_ShowChar(unsigned int x, unsigned short y, unsigned char num, unsigned short pin_color, unsigned short back_color, unsigned char size);
void LCD_Show_ASCII(unsigned short x, unsigned short y, unsigned char ch, unsigned short pen_color, unsigned short back_color);
void LCD_ShowString(unsigned short x, unsigned short y, unsigned char *str, unsigned short pin_color, unsigned short back_color, unsigned char size);
void LCD_ShowPic(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, const unsigned char *gimage);
void LCD_Display_Dir(unsigned char mode);
void LCD_ShowFlashString(unsigned short x, unsigned short y, unsigned char *str, unsigned short pin_color, unsigned short back_color);
void LCD_ShowFlashGB2312(unsigned char x, unsigned char y, char* ch, unsigned short pen_color, unsigned short back_color);
void LCD_ShowFlashChar(unsigned short x, unsigned short y, unsigned char ch, unsigned short pin_color, unsigned short back_color);
void LCD_ShowFlashPic(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short sector);
void LCD_ShowGB2312(unsigned char x, unsigned char y, char* ch, unsigned short pen_color, unsigned short back_color);
void LCD_ShowIntNum(unsigned int x, unsigned char y, unsigned short num, unsigned char len, unsigned short pin_color, unsigned short back_color, unsigned char sizey);
void LCD_ShowFloatNum(unsigned char x, unsigned char y, float num, unsigned char len, unsigned short pin_color, unsigned short back_color, unsigned char sizey);
void LCD_Show_RectangleData(u16 x0, u16 y0, u16 x1, u16 y1,char* ch, u16 font_color,u16 backg_color);
#endif
