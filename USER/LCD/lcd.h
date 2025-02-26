#ifndef __LCD_H
#define __LCD_H

#include "io_bit.h"

#define LCD_Backlight 1		//0:ȫ����1:pwm����

////////////////////////////////////////////////////////////////////
//-----------------LCD�˿ڶ���---------------- 
#define	LCD_LED PAout(8) 				//LCD����    		PA8
 
#define	LCD_CS_SET  GPIOC->BSRR=1<<9    //Ƭѡ�˿�  		PC9
#define	LCD_RS_SET	GPIOC->BSRR=1<<8    //����/���� 		PC8	   
#define	LCD_WR_SET	GPIOC->BSRR=1<<7    //д����			PC7
#define	LCD_RD_SET	GPIOC->BSRR=1<<6    //������			PC6
								    
#define	LCD_CS_CLR  GPIOC->BRR=1<<9     //Ƭѡ�˿�  		PC9
#define	LCD_RS_CLR	GPIOC->BRR=1<<8     //����/����			PC8	   
#define	LCD_WR_CLR	GPIOC->BRR=1<<7     //д����			PC7
#define	LCD_RD_CLR	GPIOC->BRR=1<<6     //������			PC6   

//PB0~15,��Ϊ������
#define DATAOUT(x) GPIOB->ODR = x; 	//�������
#define DATAIN     GPIOB->IDR;   	//��������	

 
//////////////////////////////////////////////////////////////////////
//ɨ�跽����
#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���

#define DFT_SCAN_DIR  L2R_U2D  //Ĭ�ϵ�ɨ�跽��
	 
//ɨ�跽����
#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���	 

#define DFT_SCAN_DIR  L2R_U2D  //Ĭ�ϵ�ɨ�跽��

//������ɫ
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
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
#define ORANGE				0xfc60  //�Ⱥ�ɫ
//GUI��ɫ

#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ 
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
	    															  
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
