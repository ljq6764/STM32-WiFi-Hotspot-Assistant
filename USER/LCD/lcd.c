#include "lcd.h"
#include "systick.h"
#include "font.h"
#include "w25qxx.h"
#include "timer.h"
/*********************************************************************************************************
* �� �� �� : LCD_ShowFlashString
* ����˵�� : ��ʾ�ַ���
* ��    �� : x��y����ֹ���꣬str����ʾ���ַ�����pin_color��������ɫ��back_color��������ɫ
* �� �� ֵ : ��
* ��    ע : �ֿ������Ӣ���ǵȿ����壬���ÿ�
*********************************************************************************************************/ 
void LCD_ShowFlashString(unsigned short x, unsigned short y, unsigned char *str, unsigned short pin_color, unsigned short back_color)
{
	while(*str != 0){
		if(*str < 127){		//ASCII�ɼ��ַ����Ϊ126
			LCD_ShowChar(x, y, *str, pin_color, back_color, 16);			//��ʾ�ַ�
			x += 8;			//�е�ַƫ��
			str++;
			if(x > 240-16){	//����ߵ��˱���
				x = 0;		//�ص�����
				y += 16;	//�е�ַƫ��
			}
		}else{
			LCD_ShowFlashGB2312(x, y, (char *)str, pin_color, back_color);	//��ʾ����
			x += 16;			//�е�ַƫ��
			str += 2;			//һ�����������ֽ�
			if(x > 320-16){		//����ߵ��˱���		
				x = 0;			//�ص�����
				y += 16;		//�е�ַƫ��
			}
		}
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowFlashGB2312
* ����˵�� : ��flash�ж�ȡ������ģ���ݲ���ʾ
* ��    �� : x��y����ֹ���꣬ch����Ҫ��ʾ�����ģ�pen_color��������ɫ��back_color��������ɫ
* �� �� ֵ : ��
* ��    ע : ��Ҫ���Ȱ���ģ����д�뵽25Q64�������ģ������Ҫ�ӵ�0��������ʼ��ţ�������Ҫ����ƫ����
*********************************************************************************************************/ 
void LCD_ShowFlashGB2312(unsigned char x, unsigned char y, char* ch, unsigned short pen_color, unsigned short back_color)
{
	unsigned char j = 0, k = 0;
	unsigned char temp[32] = {0}, c = 0;
	W25Qxx_ReadDatas(temp, ((*ch-0xA1)*94+(*(ch+1)-0xA1))*32, 32);
	LCD_Address_Set(x, y, x+16-1, y+16-1);				//�ںÿ�
	for(k=0; k<32; k++)
	{
		c = temp[k];
		for(j=0; j<8; j++)								//���λ���жϣ�����ʾ��Ӧ����ɫ
		{
			if(c&0x01)	LCD_Write_Data(pen_color);			//�����λΪ1�ͻ�ǰ��ɫ
			else 		LCD_Write_Data(back_color);		//�����λΪ0�ͻ�����ɫ
			c >>= 1;
		}
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowFlashChar
* ����˵�� : ��flash�ж�ȡ�ַ���ģ���ݲ���ʾ
* ��    �� : x,y����ֹ���꣬ch��Ҫ��ʾ���ַ���pin_color��������ɫ��back_color��������ɫ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_ShowFlashChar(unsigned short x, unsigned short y, unsigned char ch, unsigned short pin_color, unsigned short back_color)
{
	unsigned short i = 0, j = 0;
	unsigned char temp[32] = {0}, c = 0;
	W25Qxx_ReadDatas(temp, ((0XA3-0XA1)*94+(ch-33))*32, 32);
	LCD_Address_Set(x, y, x+16-1, y+16-1);				//�ںÿ�
	for(i=0; i<32; i++)									//һ��16*8���ַ���16���ֽڵ��������
	{
		c = temp[i];
		for(j=0; j<8; j++)								//���λ���жϣ�����ʾ��Ӧ����ɫ
		{
			if(c&0x01)		LCD_Write_Data(pin_color);		//�����λΪ1�ͻ�ǰ��ɫ
			else 			LCD_Write_Data(back_color);	//�����λΪ0�ͻ�����ɫ
			c >>= 1;
		}
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowFlashPic
* ����˵�� : ��flash�ж�ȡͼƬȡģ���ݲ���ʾ
* ��    �� : x0��y0����ֹ���꣬x1��y1��ͼƬ�Ŀ�Ⱥ͸߶ȣ�sector��ͼƬȡģ���ݴ�ŵ��������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_ShowFlashPic(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short sector)
{
	unsigned char temp[40] = {0};
	unsigned int i = 0, j = 0;
	unsigned int addr = sector * 4096;			//����������ַ
	LCD_Address_Set(x0, y0, x0+x1-1, y0+y1-1);	//�ںÿ�
	for(i=0; i<x1*y1/20; i++)
	{
		W25Qxx_ReadDatas(temp, addr, 40);
		for(j=0; j<40; j+=2)
			LCD_Write_Data(temp[j+1]<<8 | temp[j]);
		addr += 40;
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowGB2312
* ����˵�� : ��ʾ�洢��rom�е�����
* ��    �� : x0��y0����ֹ���꣬Ҫ��ʾ�����ģ�pen_color��������ɫ��back_color��������ɫ
* �� �� ֵ : ��
* ��    ע : �߱���������
*********************************************************************************************************/ 
void LCD_ShowGB2312(unsigned char x, unsigned char y, char* ch, unsigned short pen_color, unsigned short back_color)
{
	unsigned char i = 0,j = 0,k = 0;
	unsigned char temp = 0;
	while(*str != 0){
	for(i=0; i<sizeof(indexes)/sizeof(indexes[0]); i++)
	{
		if(*ch == indexes[i][0] && *(ch+1) == indexes[i][1])	//�ֱ�Ƚϸ����ĵ������ֽ��Ƿ����
			break;												//�����ȫ�����ô�͵õ��˸���������ģ�����е�λ��
	}
	if(i>=sizeof(indexes)/sizeof(indexes[0]))	return;
	LCD_Address_Set(x, y, x+32-1, y+32-1);				//�ںÿ�
	for(k=0; k<128; k++)
	{
		temp = GB2312[i][k];
		for(j=0; j<8; j++)								//���λ���жϣ�����ʾ��Ӧ����ɫ
		{
			if(temp&0x01)	LCD_Write_Data(pen_color);	//�����λΪ1�ͻ�ǰ��ɫ
			else 			LCD_Write_Data(back_color);	//�����λΪ0�ͻ�����ɫ
			temp >>= 1;
		}
	}
			x += 16;			//�е�ַƫ��
			str += 2;			//һ�����������ֽ�
			if(x > 320-16){		//����ߵ��˱���		
				x = 0;		//�ص�����
				y += 16;		//�е�ַƫ��		
	}
}
/*********************************************************************************************************
* �� �� �� : mypow
* ����˵�� : ��m��n�η�
* ��    �� : m��������n��ָ��
* �� �� ֵ : 
* ��    ע : 
*********************************************************************************************************/ 
unsigned int mypow(unsigned char m, unsigned char n)
{
	unsigned int result = 1;	 
	while(n--) result *= m;
	return result;
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowIntNum
* ����˵�� : ��ʾ����
* ��    �� : x��y����ֹ���꣬num��Ҫ��ʾ�����֣�len����ʾ��λ����pin_color��������ɫ��back_color��������ɫ��sizey�������С
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_ShowIntNum(unsigned int x, unsigned char y, unsigned short num, unsigned char len, unsigned short pin_color, unsigned short back_color, unsigned char sizey)
{         	
	unsigned char t = 0, temp = 0;
	unsigned char enshow = 0;
	unsigned char sizex = sizey/2;
	for(t=0; t<len; t++)
	{
		temp = (num/mypow(10,len-t-1))%10;
		if(enshow==0 && t<(len-1))
		{
			if(temp == 0)
			{
				LCD_ShowChar(x+t*sizex, y, ' ', pin_color, back_color, sizey);
				continue;
			}else enshow = 1;  
		}
	 	LCD_ShowChar(x+t*sizex, y, temp+48, pin_color, back_color, sizey);
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowFloatNum
* ����˵�� : ��ʾ����λС���ĸ�����
* ��    �� : x��y����ֹ���꣬num����ʾ�ĸ�������len����ʾ����λ����pin_color��������ɫ��back_color��������ɫ��sizey�������С
* �� �� ֵ : 
* ��    ע : 
*********************************************************************************************************/ 
void LCD_ShowFloatNum(unsigned char x, unsigned char y, float num, unsigned char len, unsigned short pin_color, unsigned short back_color, unsigned char sizey)
{         	
	unsigned char t = 0, temp = 0, sizex = 0;
	unsigned short num1 = 0;
	sizex = sizey/2;
	num1 = num*100;
	for(t=0; t<len; t++)
	{
		temp = (num1/mypow(10,len-t-1))%10;
		if(t == (len-2))
		{
			LCD_ShowChar(x+(len-2)*sizex, y, '.', pin_color, back_color, sizey);
			t++;
			len += 1;
		}
	 	LCD_ShowChar(x+t*sizex, y, temp+48, pin_color, back_color, sizey);
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_Fill
* ����˵�� : ��ָ�����������ɫ
* ��    �� : xsta/ysta����ʼ���ꣻxend,yend����ֹ���ꣻcolor��Ҫ������ɫ
* �� �� ֵ : ��
* ��    ע : �����Σ����������㡢��ˮƽ�ߡ�����ֱ��
*********************************************************************************************************/ 
void LCD_Fill(unsigned short xsta, unsigned short ysta, unsigned short xend, unsigned short yend, unsigned short color)
{          
	unsigned int i;
	
	LCD_Address_Set(xsta, ysta, xsta+xend-1, ysta+yend-1);	//������ʾ��Χ
	
	LCD_RS_SET;		//д����  
	for(i=0; i<xend*yend; i++){
		LCD_CS_CLR;		//ѡ������,дһ��������Ҫѡ������
		DATAOUT(color);	//д������
		LCD_CS_SET;		//ȡ��ѡ�У�д��һ��������Ҫ�ͷ�����
	}	
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowPic
* ����˵�� : ��ʾ16bitͼƬ
* ��    �� : x0/y0����ʼ���ꣻx1,y1����ֹ���ꣻgimage��ͼƬ����ָ��
* �� �� ֵ : ��
* ��    ע : c�������飬ˮƽɨ�裬�޹�ѡ��16λ���
*********************************************************************************************************/ 

void LCD_ShowPic(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, const unsigned char *gimage)
{
	unsigned int i;
	
	LCD_Address_Set(x0, y0, x0+x1-1, y0+y1-1);	//�ںÿ�
	for(i=0; i<x1*y1; i++){
		LCD_Write_Data(*(gimage+1)<<8 | *gimage);
		gimage += 2;
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_Show_ASCII
* ����˵�� : ��ָ��λ����ʾһ��ASCII�ַ�
* ��    �� : x/y����ֹ���ꣻch����Ҫ��ʾ��ASCII�ַ���pen_color��������ɫ��back_color��������ɫ
* �� �� ֵ : ��
* ��    ע : ɨ�跽ʽ�����룬����ɨ�裬���򣨵�λ��ǰ��16*8����㷨���ƣ��ٶ�ƫ����
*********************************************************************************************************/ 
void LCD_Show_ASCII(unsigned short x, unsigned short y, unsigned char ch, unsigned short pen_color, unsigned short back_color)
{  	
	unsigned short i, j, x0=x, temp;
	
	for(i=0; i<16; i++){
		temp = ascii_16_8[ch-32][i];
		for(j=0; j<8; j++){
			if(temp & (1<<j)){	//����1����pen_color
				LCD_Draw_Point(x, y, pen_color);
			}
			else{				//����0����back_color
				LCD_Draw_Point(x, y, back_color);
			}
			x++;				//����
			if(x-x0 == 8){
				y++;			//����
				x = x0;			//�ػ�����
			}
		}
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowChar
* ����˵�� : ��ָ��λ����ʾһ��ASCII�ַ�
* ��    �� : x/y����ֹ���ꣻch����Ҫ��ʾ��ASCII�ַ���pen_color��������ɫ��back_color��������ɫ
* �� �� ֵ : ��
* ��    ע : ɨ�跽ʽ�����룬����ɨ�裬���򣨵�λ��ǰ��16*8�������ƣ��ٶȽϿ죡
*********************************************************************************************************/ 
void LCD_ShowChar(unsigned int x, unsigned short y, unsigned char num, unsigned short pin_color, unsigned short back_color, unsigned char size)
{
	unsigned short i, j;
	unsigned char ch = num - 32;							//��������ַ�����ģ����λ��
	unsigned char temp, sziey;

	LCD_Address_Set(x, y, x+size/2-1, y+16*(size/16)-1);	//�ںÿ�
	sziey = size/2*16*size/16/8;							//�õ�һ���ַ�����ģ�ֽ���

	for(i=0; i<sziey; i++){									//һ��16*8���ַ���16���ֽڵ��������
		//temp = (sziey==16?ascii_16_8[ch][i]:ascii_32_16[ch][i]);	//���ȡ����ģ����
		temp = ascii_16_8[ch][i];
		for(j=0; j<8; j++){											//���λ���жϣ�����ʾ��Ӧ����ɫ
			if(temp&0x01){
				LCD_Write_Data(pin_color);				//�����λΪ1�ͻ�ǰ��ɫ
			}else{
				LCD_Write_Data(back_color);				//�����λΪ0�ͻ�����ɫ
			}
			temp >>= 1;
		}
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_ShowString
* ����˵�� : ��ָ��λ����ʾһ���ַ���
* ��    �� : x/y����ֹ���ꣻstr����ʾ���ַ�����pen_color��������ɫ��back_color��������ɫ
* �� �� ֵ : ��
* ��    ע : ����Ӣ���ַ���˵���ֿ����ָߵ�һ��
*********************************************************************************************************/ 
void LCD_ShowString(unsigned short x, unsigned short y, unsigned char *str, unsigned short pin_color, unsigned short back_color, unsigned char size)
{
	while(*str != 0){
		if(*str < 127){			//ASCII�ɼ��ַ����Ϊ126
			LCD_Show_ASCII(x, y, *str, pin_color, back_color);	//��ʾ�ַ�
			x += size/2;		//�е�ַƫ��size/2
			str++;
			if(x > 320-size/2){	//����ߵ��˱���
				x = 0;			//�ص�����
				y += size;		//�е�ַƫ��size
			}
		}
	}
}
/*********************************************************************************************************
* �� �� �� : LCD_Color_Fill
* ����˵�� : ��ɫ�����
* ��    �� : x0/y0����ֹ���ꣻx1/y1����ֹ���ꣻpic����ɫ����ָ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_Color_Fill(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, const unsigned short *pic)
{
	unsigned int i;
	unsigned int count = (x1-x0+1) * (y1-y0+1);
	
	LCD_Address_Set(x0, y0, x1, y1);	//�ںÿ�
	LCD_RS_SET;				//д����  
	for(i=0; i<count; i++){
		LCD_CS_CLR;			//ѡ������,дһ��������Ҫѡ������,����ܽ������ļ����ɿ���DMA TO GPIO�ķ�ʽ����
		DATAOUT(pic[i]);	//д������
		LCD_CS_SET;			//ȡ��ѡ�У�д��һ��������Ҫ�ͷ�����
	}	
}
/*********************************************************************************************************
* �� �� �� : TP_Draw_Big_Point
* ����˵�� : ����һ����ĵ㣬������д����
* ��    �� : x/y��������������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void TP_Draw_Big_Point(unsigned short x,unsigned short y,unsigned short color)
{	    
	LCD_Draw_Point(x, y, color);	//���ĵ� 
	LCD_Draw_Point(x+1 ,y, color);
	LCD_Draw_Point(x, y+1, color);
	LCD_Draw_Point(x+1, y+1, color);	 	  	
}
/*********************************************************************************************************
* �� �� �� : LCD_Display_Dir
* ����˵�� : ����LCD��ʾ����
* ��    �� : mode��0������1����
* �� �� ֵ : 
* ��    ע : 
*********************************************************************************************************/ 
void LCD_Display_Dir(unsigned char mode)
{
	unsigned short regval 	= 0;
	
	if(mode == 1){
		regval |= (1<<7)|(0<<6)|(1<<5)|(1<<3); //����ʱ,���µ���,������ ɨ��
	}else{
		regval |= (0<<7)|(0<<6)|(0<<5)|(1<<3); 
	}
	
	LCD_Write_CMD(0X36);
	LCD_Write_Data(regval);
	
	LCD_Write_CMD(0x2a); 
	LCD_Write_Data(0);
	LCD_Write_Data(0);
	LCD_Write_Data(479>>8);
	LCD_Write_Data(479&0XFF);
	
	LCD_Write_CMD(0x2b); 
	LCD_Write_Data(0);
	LCD_Write_Data(0);
	LCD_Write_Data(319>>8);
	LCD_Write_Data(319&0XFF); 	
	
}
/*********************************************************************************************************
* �� �� �� : LCD_Write_CMD
* ����˵�� : д�Ĵ���
* ��    �� : data����Ҫд��Ĵ�����ֵ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_Write_CMD(unsigned short data)
{ 
	LCD_CS_CLR; 	//ѡ������
	LCD_RS_CLR;		//д����  
// 	LCD_WR_CLR; 	//ʹ��д(�������ݿ��Բ�д�����ֵ͵�ƽ����)
	DATAOUT(data); 	//д������
//	LCD_WR_SET; 	//ʹ�ܶ�(�������ݿ��Բ�д�����ֵ͵�ƽ����)
 	LCD_CS_SET;   	//ȡ��ѡ��
}
/*********************************************************************************************************
* �� �� �� : LCD_Write_Data
* ����˵�� : д����
* ��    �� : data����Ҫд�������
* �� �� ֵ : ��
* ��    ע : �������LCD_Write_DataX��,��ʱ�任�ռ�
*********************************************************************************************************/ 
void LCD_Write_Data(unsigned short data)
{
	LCD_CS_CLR;		//ѡ������
	LCD_RS_SET;		//д����  
//	LCD_WR_CLR;		//ʹ��д(�������ݿ��Բ�д�����ֵ͵�ƽ����)
	DATAOUT(data);	//д������
//	LCD_WR_SET;		//ʹ�ܶ�(�������ݿ��Բ�д�����ֵ͵�ƽ����)
	LCD_CS_SET;		//ȡ��ѡ��
}
/*********************************************************************************************************
* �� �� �� : LCD_Draw_Point
* ����˵�� : �����Ƶ�
* ��    �� : x/y����������㣻color����ɫ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_Draw_Point(unsigned short x, unsigned short y, unsigned short color)
{	   
	LCD_Write_CMD(0x2a);	//����X����
    LCD_Write_Data(x>>8);	//���X������ֽ�  
    LCD_Write_Data(x);     	//���X������ֽ�
    LCD_Write_Data(x>>8); 	//�յ�X������ֽ�
    LCD_Write_Data(x);     	//�յ�X������ֽ�
	
	LCD_Write_CMD(0x2b); 	//����Y����
    LCD_Write_Data(y>>8);
    LCD_Write_Data(y);
    LCD_Write_Data(y>>8);
    LCD_Write_Data(y);
	
    LCD_Write_CMD(0x2c);	//д����ɫ����
    LCD_Write_Data(color);
}
/*********************************************************************************************************
* �� �� �� : LCD_ReadPoint
* ����˵�� : ������ɫ
* ��    �� : x/y����ȡ�������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
unsigned short LCD_ReadPoint(unsigned short x,unsigned short y)
{
  unsigned short i, data;
	
  LCD_Write_CMD(0x2a);		//����X����
  LCD_Write_Data(x>>8);
  LCD_Write_Data(x&0XFF); 	//�õ���ʼ�кŵ��ֽ�
  LCD_Write_Data(x>>8);		//�õ������кŸ��ֽ�
  LCD_Write_Data(x&0xFF);   //�õ������кŵ��ֽ�

  LCD_Write_CMD(0x2b); 		//����Y����
  LCD_Write_Data(y>>8);
  LCD_Write_Data(y&0XFF);
  LCD_Write_Data(y>>8);
  LCD_Write_Data(y&0xFF);
  
  LCD_Write_CMD(0X2E);		//�������ص�ָ��
  data = DATAIN;  			//dummy Read
  for(i=0; i<5; i++);		//����ʱ
  data = DATAIN; 			//ʵ��������ɫ    
  return data;  			//������ɫֵ
}
/*********************************************************************************************************
* �� �� �� : LCD_Address_Set
* ����˵�� : ������ʼ�ͽ�����ַ
* ��    �� : x1/y1����ֹ��ַ��x2/y2��������ַ
* �� �� ֵ : ��
* ��    ע : ���ٻ���
*********************************************************************************************************/ 
void LCD_Address_Set(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2)
{
	LCD_Write_CMD(0x2a);	//����X����
    LCD_Write_Data(x1>> 8);	//���X1������ֽ�  
    LCD_Write_Data(x1);     //���X1������ֽ�
    LCD_Write_Data(x2>> 8); //�յ�X2������ֽ�
    LCD_Write_Data(x2);     //�յ�X2������ֽ�
	
	LCD_Write_CMD(0x2b); 	//����Y����
    LCD_Write_Data(y1>> 8);
    LCD_Write_Data(y1);
    LCD_Write_Data(y2>> 8);
    LCD_Write_Data(y2);
	
    LCD_Write_CMD(0x2c);	//д����ɫ����
}
void Lcd_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
#if LCD_Backlight	 
	Timer1_PwmInit(1000, 72);
	TIM1->CCR1 = 0;						//��ʼ�رձ���
#else
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	GPIO_ResetBits(GPIOA, GPIO_Pin_8);	//��ʼ�رձ���
#endif	
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);	//Ĭ��ʹ��д
	GPIO_SetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
 
	GPIO_SetBits(GPIOB,GPIO_Pin_All);

	Delay_Ms(120); 
	LCD_Write_CMD(0XF2);
	LCD_Write_Data(0x18);
	LCD_Write_Data(0xA3);
	LCD_Write_Data(0x12);
	LCD_Write_Data(0x02);
	LCD_Write_Data(0XB2);
	LCD_Write_Data(0x12);
	LCD_Write_Data(0xFF);
	LCD_Write_Data(0x10);
	LCD_Write_Data(0x00);
	LCD_Write_CMD(0XF8);
	LCD_Write_Data(0x21);
	LCD_Write_Data(0x04);
	LCD_Write_CMD(0XF9);
	LCD_Write_Data(0x00);
	LCD_Write_Data(0x08);
	LCD_Write_CMD(0x36);
	LCD_Write_Data(0x08); 
	LCD_Write_CMD(0x3A);
	LCD_Write_Data(0x05); 	//����16λBPP
	LCD_Write_CMD(0xB4);
	LCD_Write_Data(0x01);	//0x00
	LCD_Write_CMD(0xB6);
	LCD_Write_Data(0x02);
	LCD_Write_Data(0x22);
	LCD_Write_CMD(0xC1);
	LCD_Write_Data(0x41);
	LCD_Write_CMD(0xC5);
	LCD_Write_Data(0x00);
	LCD_Write_Data(0x07);	//0X18
	LCD_Write_CMD(0xE0);
	LCD_Write_Data(0x0F);
	LCD_Write_Data(0x1F);
	LCD_Write_Data(0x1C);
	LCD_Write_Data(0x0C);
	LCD_Write_Data(0x0F);
	LCD_Write_Data(0x08);
	LCD_Write_Data(0x48);
	LCD_Write_Data(0x98);
	LCD_Write_Data(0x37);
	LCD_Write_Data(0x0A);
	LCD_Write_Data(0x13);
	LCD_Write_Data(0x04);
	LCD_Write_Data(0x11);
	LCD_Write_Data(0x0D);
	LCD_Write_Data(0x00);
	LCD_Write_CMD(0xE1);
	LCD_Write_Data(0x0F);
	LCD_Write_Data(0x32);
	LCD_Write_Data(0x2E);
	LCD_Write_Data(0x0B);
	LCD_Write_Data(0x0D);
	LCD_Write_Data(0x05);
	LCD_Write_Data(0x47);
	LCD_Write_Data(0x75);
	LCD_Write_Data(0x37);
	LCD_Write_Data(0x06);
	LCD_Write_Data(0x10);
	LCD_Write_Data(0x03);
	LCD_Write_Data(0x24);
	LCD_Write_Data(0x20);
	LCD_Write_Data(0x00);

	LCD_Write_CMD(0X3A); 	//�趨16BPP
	LCD_Write_Data(0X55);	
	LCD_Write_CMD(0x11); 	// �˳�˯��״̬
    Delay_Ms(120);
    LCD_Write_CMD(0x29); 	// ������ʾ	
	
	LCD_Display_Dir(1);		//������ʾ
	
	LCD_Fill(0, 0, 480, 320, WHITE);    //����

#if LCD_Backlight
	TIM1->CCR1 = 1000;		
#else
	LCD_LED = 1;
#endif
}

/*********************************************************************************************************
* ����˵�� : ����һ������
* ��    �� : x0/y0�����εĿ�͸ߵ���ʼ����    x1/y1�����εĿ�͸ߵĽ�������   color:���α߿���ɫ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_Draw_Rectangle(u16 x0, u16 y0, u16 x1, u16 y1, u16 color)
{
	int i;	
	for(i=x0;i<x1;i++)  //����ƽ����
	{
		LCD_Draw_Point(i, y0,color);
		LCD_Draw_Point(i, y1,color);
	}
	for(i=y0;i<=y1;i++)  //��������
	{
		LCD_Draw_Point(x0,i,color);
		LCD_Draw_Point(x1,i,color);
	}
}
/*********************************************************************************************************
* ����˵�� : ����һ�����Σ����������α�����ɫ
* ��    �� : x0/y0�����εĿ�͸ߵ���ʼ����    x1/y1�����εĿ�͸ߵĽ�������   frame_color:���α߿���ɫ   backg_color:���α�����ɫ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_Draw_FillRectangle(u16 x0, u16 y0, u16 x1, u16 y1, u16 frame_color,u16 backg_color)
{
	int i,j;
	LCD_Draw_Rectangle(x0, y0, x1,y1,frame_color);  //���ƾ���
	for(j=y0+1;j<y1;j++)  //���ƻ������
	{
		for(i=x0+1;i<x1;i++)  //���1������
		{
			LCD_Draw_Point(i,j,backg_color);   //����		
		}
	}	
}


/*********************************************************************************************************
* ����˵�� : �ַ�+���ֻ����ʾ
* ��    �� : x��y����ֹ���꣬ch:Ҫ��ʾ�����ݣ�pen_color��������ɫ��back_color��������ɫ  size:��ʾ�����С
* �� �� ֵ : ��
* ��    ע : �߱���������
*********************************************************************************************************/ 
void LCD_ShowChineseChar(u16 x, u16 y, char *ch, u16 pen_color, u16 back_color,u8 size)
{
	unsigned char i = 0,j = 0,k = 0;
	unsigned char temp = 0;
	
	while(*ch)   //����\0˵��������ʾ���
	{
		if(*ch > 127)   //����
		{
			for(i=0; i<sizeof(indexes)/sizeof(indexes[0]); i++)
			{
				if(*ch == indexes[i][0] && *(ch+1) == indexes[i][1])	//�ֱ�Ƚϸ����ĵ������ֽ��Ƿ����
					break;												//�����ȫ�����ô�͵õ��˸���������ģ�����е�λ��
			}
			if(i>=sizeof(indexes)/sizeof(indexes[0]))	return;
			LCD_Address_Set(x, y, x+32-1, y+32-1);				//�ںÿ�
			for(k=0; k<128; k++)
			{
				temp = GB2312[i][k];
				for(j=0; j<8; j++)								//���λ���жϣ�����ʾ��Ӧ����ɫ
				{
					if(temp&0x01)	LCD_Write_Data(pen_color);	//�����λΪ1�ͻ�ǰ��ɫ
					else 			LCD_Write_Data(back_color);	//�����λΪ0�ͻ�����ɫ
					temp >>= 1;
				}
			}
			x+=32;
			ch+=2; 
		}else{   //�ַ�
			LCD_ShowChar(x, y,*ch,pen_color, back_color,16);  //��ʾһ���ַ�			
			x+=8;  //Ϊ��һ������ʾ������λ
		    ch+=1; 
		}
	}
	
}


/*********************************************************************************************************
* ����˵�� : ����һ�����Σ������α�����ɫ�����Ҿ�����ʾ����
* ��    �� : x0/y0�����εĿ�͸ߵ���ʼ����    x1/y1�����εĿ�͸ߵĽ�������   
									font_color:���α߿�+������ɫ   backg_color:����+���屳����ɫ    ch:���ξ�����ʾ������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void LCD_Show_RectangleData(u16 x0, u16 y0, u16 x1, u16 y1,char* ch, u16 font_color,u16 backg_color)
{
	u16 wide=strlen(ch)*8;  //������ʾ���ݵĿ������ֵ
	LCD_Draw_FillRectangle(x0, y0, x1,y1, font_color,backg_color);   //����һ��������ɫ�ľ���
	LCD_ShowChineseChar(((x1-x0)-wide)/2+x0, ((y1-y0)-16)/2+y0,ch,font_color,backg_color,16);  //�ַ�+���ֻ����ʾ	
}


