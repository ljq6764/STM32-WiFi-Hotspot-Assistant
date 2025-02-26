#include "lcd.h"
#include "systick.h"
#include "font.h"
#include "w25qxx.h"
#include "timer.h"
/*********************************************************************************************************
* 函 数 名 : LCD_ShowFlashString
* 功能说明 : 显示字符串
* 形    参 : x、y：起止坐标，str：显示的字符串，pin_color：画笔颜色，back_color：背景颜色
* 返 回 值 : 无
* 备    注 : 字库里面的英文是等宽字体，不好看
*********************************************************************************************************/ 
void LCD_ShowFlashString(unsigned short x, unsigned short y, unsigned char *str, unsigned short pin_color, unsigned short back_color)
{
	while(*str != 0){
		if(*str < 127){		//ASCII可见字符最大为126
			LCD_ShowChar(x, y, *str, pin_color, back_color, 16);			//显示字符
			x += 8;			//列地址偏移
			str++;
			if(x > 240-16){	//如果走到了边沿
				x = 0;		//回到首列
				y += 16;	//行地址偏移
			}
		}else{
			LCD_ShowFlashGB2312(x, y, (char *)str, pin_color, back_color);	//显示中文
			x += 16;			//列地址偏移
			str += 2;			//一个中文两个字节
			if(x > 320-16){		//如果走到了边沿		
				x = 0;			//回到首列
				y += 16;		//行地址偏移
			}
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowFlashGB2312
* 功能说明 : 从flash中读取中文字模数据并显示
* 形    参 : x、y：起止坐标，ch：需要显示的中文，pen_color：画笔颜色，back_color：背景颜色
* 返 回 值 : 无
* 备    注 : 需要事先把字模数据写入到25Q64里，并且字模数组需要从第0个扇区开始存放，否则需要加上偏移量
*********************************************************************************************************/ 
void LCD_ShowFlashGB2312(unsigned char x, unsigned char y, char* ch, unsigned short pen_color, unsigned short back_color)
{
	unsigned char j = 0, k = 0;
	unsigned char temp[32] = {0}, c = 0;
	W25Qxx_ReadDatas(temp, ((*ch-0xA1)*94+(*(ch+1)-0xA1))*32, 32);
	LCD_Address_Set(x, y, x+16-1, y+16-1);				//挖好坑
	for(k=0; k<32; k++)
	{
		c = temp[k];
		for(j=0; j<8; j++)								//逐个位的判断，并显示对应的颜色
		{
			if(c&0x01)	LCD_Write_Data(pen_color);			//如果该位为1就画前景色
			else 		LCD_Write_Data(back_color);		//如果该位为0就画背景色
			c >>= 1;
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowFlashChar
* 功能说明 : 从flash中读取字符字模数据并显示
* 形    参 : x,y：起止坐标，ch：要显示的字符，pin_color：画笔颜色，back_color：背景颜色
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_ShowFlashChar(unsigned short x, unsigned short y, unsigned char ch, unsigned short pin_color, unsigned short back_color)
{
	unsigned short i = 0, j = 0;
	unsigned char temp[32] = {0}, c = 0;
	W25Qxx_ReadDatas(temp, ((0XA3-0XA1)*94+(ch-33))*32, 32);
	LCD_Address_Set(x, y, x+16-1, y+16-1);				//挖好坑
	for(i=0; i<32; i++)									//一个16*8的字符由16个字节的数据组成
	{
		c = temp[i];
		for(j=0; j<8; j++)								//逐个位的判断，并显示对应的颜色
		{
			if(c&0x01)		LCD_Write_Data(pin_color);		//如果该位为1就画前景色
			else 			LCD_Write_Data(back_color);	//如果该位为0就画背景色
			c >>= 1;
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowFlashPic
* 功能说明 : 从flash中读取图片取模数据并显示
* 形    参 : x0、y0：起止坐标，x1、y1：图片的宽度和高度，sector：图片取模数据存放的扇区编号
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_ShowFlashPic(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short sector)
{
	unsigned char temp[40] = {0};
	unsigned int i = 0, j = 0;
	unsigned int addr = sector * 4096;			//计算扇区地址
	LCD_Address_Set(x0, y0, x0+x1-1, y0+y1-1);	//挖好坑
	for(i=0; i<x1*y1/20; i++)
	{
		W25Qxx_ReadDatas(temp, addr, 40);
		for(j=0; j<40; j+=2)
			LCD_Write_Data(temp[j+1]<<8 | temp[j]);
		addr += 40;
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowGB2312
* 功能说明 : 显示存储在rom中的中文
* 形    参 : x0、y0：起止坐标，要显示的中文，pen_color：画笔颜色，back_color：背景颜色
* 返 回 值 : 无
* 备    注 : 具备索引功能
*********************************************************************************************************/ 
void LCD_ShowGB2312(unsigned char x, unsigned char y, char* ch, unsigned short pen_color, unsigned short back_color)
{
	unsigned char i = 0,j = 0,k = 0;
	unsigned char temp = 0;
	while(*str != 0){
	for(i=0; i<sizeof(indexes)/sizeof(indexes[0]); i++)
	{
		if(*ch == indexes[i][0] && *(ch+1) == indexes[i][1])	//分别比较该中文的两个字节是否相等
			break;												//如果完全相等那么就得到了该中文在字模数组中的位置
	}
	if(i>=sizeof(indexes)/sizeof(indexes[0]))	return;
	LCD_Address_Set(x, y, x+32-1, y+32-1);				//挖好坑
	for(k=0; k<128; k++)
	{
		temp = GB2312[i][k];
		for(j=0; j<8; j++)								//逐个位的判断，并显示对应的颜色
		{
			if(temp&0x01)	LCD_Write_Data(pen_color);	//如果该位为1就画前景色
			else 			LCD_Write_Data(back_color);	//如果该位为0就画背景色
			temp >>= 1;
		}
	}
			x += 16;			//列地址偏移
			str += 2;			//一个中文两个字节
			if(x > 320-16){		//如果走到了边沿		
				x = 0;		//回到首列
				y += 16;		//行地址偏移		
	}
}
/*********************************************************************************************************
* 函 数 名 : mypow
* 功能说明 : 求m的n次方
* 形    参 : m：底数，n：指数
* 返 回 值 : 
* 备    注 : 
*********************************************************************************************************/ 
unsigned int mypow(unsigned char m, unsigned char n)
{
	unsigned int result = 1;	 
	while(n--) result *= m;
	return result;
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowIntNum
* 功能说明 : 显示数字
* 形    参 : x、y：起止坐标，num：要显示的数字，len：显示的位数，pin_color：画笔颜色，back_color：背景颜色，sizey：字体大小
* 返 回 值 : 无
* 备    注 : 无
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
* 函 数 名 : LCD_ShowFloatNum
* 功能说明 : 显示带两位小数的浮点数
* 形    参 : x、y：起止坐标，num：显示的浮点数，len：显示的总位数，pin_color：画笔颜色，back_color：背景颜色，sizey：字体大小
* 返 回 值 : 
* 备    注 : 
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
* 函 数 名 : LCD_Fill
* 功能说明 : 在指定区域填充颜色
* 形    参 : xsta/ysta：起始坐标；xend,yend：终止坐标；color：要填充的颜色
* 返 回 值 : 无
* 备    注 : 填充矩形，清屏，画点、画水平线、画垂直线
*********************************************************************************************************/ 
void LCD_Fill(unsigned short xsta, unsigned short ysta, unsigned short xend, unsigned short yend, unsigned short color)
{          
	unsigned int i;
	
	LCD_Address_Set(xsta, ysta, xsta+xend-1, ysta+yend-1);	//设置显示范围
	
	LCD_RS_SET;		//写数据  
	for(i=0; i<xend*yend; i++){
		LCD_CS_CLR;		//选中器件,写一个数据需要选中器件
		DATAOUT(color);	//写入数据
		LCD_CS_SET;		//取消选中，写完一个数据需要释放器件
	}	
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowPic
* 功能说明 : 显示16bit图片
* 形    参 : x0/y0：起始坐标；x1,y1：终止坐标；gimage：图片数据指针
* 返 回 值 : 无
* 备    注 : c语言数组，水平扫描，无勾选，16位真彩
*********************************************************************************************************/ 

void LCD_ShowPic(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, const unsigned char *gimage)
{
	unsigned int i;
	
	LCD_Address_Set(x0, y0, x0+x1-1, y0+y1-1);	//挖好坑
	for(i=0; i<x1*y1; i++){
		LCD_Write_Data(*(gimage+1)<<8 | *gimage);
		gimage += 2;
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_Show_ASCII
* 功能说明 : 在指定位置显示一个ASCII字符
* 形    参 : x/y：起止坐标；ch：需要显示的ASCII字符；pen_color：画笔颜色；back_color：背景颜色
* 返 回 值 : 无
* 备    注 : 扫描方式：阴码，逐行扫描，逆向（低位在前）16*8；描点法绘制，速度偏慢！
*********************************************************************************************************/ 
void LCD_Show_ASCII(unsigned short x, unsigned short y, unsigned char ch, unsigned short pen_color, unsigned short back_color)
{  	
	unsigned short i, j, x0=x, temp;
	
	for(i=0; i<16; i++){
		temp = ascii_16_8[ch-32][i];
		for(j=0; j<8; j++){
			if(temp & (1<<j)){	//遇到1，画pen_color
				LCD_Draw_Point(x, y, pen_color);
			}
			else{				//遇到0，画back_color
				LCD_Draw_Point(x, y, back_color);
			}
			x++;				//换列
			if(x-x0 == 8){
				y++;			//换行
				x = x0;			//重回首列
			}
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowChar
* 功能说明 : 在指定位置显示一个ASCII字符
* 形    参 : x/y：起止坐标；ch：需要显示的ASCII字符；pen_color：画笔颜色；back_color：背景颜色
* 返 回 值 : 无
* 备    注 : 扫描方式：阴码，逐行扫描，逆向（低位在前）16*8；填充绘制，速度较快！
*********************************************************************************************************/ 
void LCD_ShowChar(unsigned int x, unsigned short y, unsigned char num, unsigned short pin_color, unsigned short back_color, unsigned char size)
{
	unsigned short i, j;
	unsigned char ch = num - 32;							//计算出该字符的字模数据位置
	unsigned char temp, sziey;

	LCD_Address_Set(x, y, x+size/2-1, y+16*(size/16)-1);	//挖好坑
	sziey = size/2*16*size/16/8;							//得到一个字符的字模字节数

	for(i=0; i<sziey; i++){									//一个16*8的字符由16个字节的数据组成
		//temp = (sziey==16?ascii_16_8[ch][i]:ascii_32_16[ch][i]);	//逐个取出字模数据
		temp = ascii_16_8[ch][i];
		for(j=0; j<8; j++){											//逐个位的判断，并显示对应的颜色
			if(temp&0x01){
				LCD_Write_Data(pin_color);				//如果该位为1就画前景色
			}else{
				LCD_Write_Data(back_color);				//如果该位为0就画背景色
			}
			temp >>= 1;
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_ShowString
* 功能说明 : 在指定位置显示一个字符串
* 形    参 : x/y：起止坐标；str：显示的字符串；pen_color：画笔颜色；back_color：背景颜色
* 返 回 值 : 无
* 备    注 : 对于英文字符来说，字宽是字高的一半
*********************************************************************************************************/ 
void LCD_ShowString(unsigned short x, unsigned short y, unsigned char *str, unsigned short pin_color, unsigned short back_color, unsigned char size)
{
	while(*str != 0){
		if(*str < 127){			//ASCII可见字符最大为126
			LCD_Show_ASCII(x, y, *str, pin_color, back_color);	//显示字符
			x += size/2;		//列地址偏移size/2
			str++;
			if(x > 320-size/2){	//如果走到了边沿
				x = 0;			//回到首列
				y += size;		//行地址偏移size
			}
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : LCD_Color_Fill
* 功能说明 : 颜色块填充
* 形    参 : x0/y0：起止坐标；x1/y1：终止坐标；pic：颜色数据指针
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_Color_Fill(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, const unsigned short *pic)
{
	unsigned int i;
	unsigned int count = (x1-x0+1) * (y1-y0+1);
	
	LCD_Address_Set(x0, y0, x1, y1);	//挖好坑
	LCD_RS_SET;				//写数据  
	for(i=0; i<count; i++){
		LCD_CS_CLR;			//选中器件,写一个数据需要选中器件,如果能解决这个文件即可开启DMA TO GPIO的方式驱动
		DATAOUT(pic[i]);	//写入数据
		LCD_CS_SET;			//取消选中，写完一个数据需要释放器件
	}	
}
/*********************************************************************************************************
* 函 数 名 : TP_Draw_Big_Point
* 功能说明 : 绘制一个大的点，用于手写场景
* 形    参 : x/y：绘制坐标中心
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void TP_Draw_Big_Point(unsigned short x,unsigned short y,unsigned short color)
{	    
	LCD_Draw_Point(x, y, color);	//中心点 
	LCD_Draw_Point(x+1 ,y, color);
	LCD_Draw_Point(x, y+1, color);
	LCD_Draw_Point(x+1, y+1, color);	 	  	
}
/*********************************************************************************************************
* 函 数 名 : LCD_Display_Dir
* 功能说明 : 设置LCD显示方向
* 形    参 : mode：0竖屏，1横屏
* 返 回 值 : 
* 备    注 : 
*********************************************************************************************************/ 
void LCD_Display_Dir(unsigned char mode)
{
	unsigned short regval 	= 0;
	
	if(mode == 1){
		regval |= (1<<7)|(0<<6)|(1<<5)|(1<<3); //横屏时,从下到上,从左到右 扫描
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
* 函 数 名 : LCD_Write_CMD
* 功能说明 : 写寄存器
* 形    参 : data：需要写入寄存器的值
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_Write_CMD(unsigned short data)
{ 
	LCD_CS_CLR; 	//选中器件
	LCD_RS_CLR;		//写命令  
// 	LCD_WR_CLR; 	//使能写(不读数据可以不写，保持低电平即可)
	DATAOUT(data); 	//写入数据
//	LCD_WR_SET; 	//使能读(不读数据可以不写，保持低电平即可)
 	LCD_CS_SET;   	//取消选中
}
/*********************************************************************************************************
* 函 数 名 : LCD_Write_Data
* 功能说明 : 写数据
* 形    参 : data：需要写入的数据
* 返 回 值 : 无
* 备    注 : 可以替代LCD_Write_DataX宏,拿时间换空间
*********************************************************************************************************/ 
void LCD_Write_Data(unsigned short data)
{
	LCD_CS_CLR;		//选中器件
	LCD_RS_SET;		//写数据  
//	LCD_WR_CLR;		//使能写(不读数据可以不写，保持低电平即可)
	DATAOUT(data);	//写入数据
//	LCD_WR_SET;		//使能读(不读数据可以不写，保持低电平即可)
	LCD_CS_SET;		//取消选中
}
/*********************************************************************************************************
* 函 数 名 : LCD_Draw_Point
* 功能说明 : 描点绘制点
* 形    参 : x/y：绘制坐标点；color：颜色
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_Draw_Point(unsigned short x, unsigned short y, unsigned short color)
{	   
	LCD_Write_CMD(0x2a);	//设置X坐标
    LCD_Write_Data(x>>8);	//起点X坐标高字节  
    LCD_Write_Data(x);     	//起点X坐标低字节
    LCD_Write_Data(x>>8); 	//终点X坐标高字节
    LCD_Write_Data(x);     	//终点X坐标低字节
	
	LCD_Write_CMD(0x2b); 	//设置Y坐标
    LCD_Write_Data(y>>8);
    LCD_Write_Data(y);
    LCD_Write_Data(y>>8);
    LCD_Write_Data(y);
	
    LCD_Write_CMD(0x2c);	//写入颜色数据
    LCD_Write_Data(color);
}
/*********************************************************************************************************
* 函 数 名 : LCD_ReadPoint
* 功能说明 : 读点颜色
* 形    参 : x/y：读取的坐标点
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
unsigned short LCD_ReadPoint(unsigned short x,unsigned short y)
{
  unsigned short i, data;
	
  LCD_Write_CMD(0x2a);		//设置X坐标
  LCD_Write_Data(x>>8);
  LCD_Write_Data(x&0XFF); 	//得到起始列号低字节
  LCD_Write_Data(x>>8);		//得到结束列号高字节
  LCD_Write_Data(x&0xFF);   //得到结束列号低字节

  LCD_Write_CMD(0x2b); 		//设置Y坐标
  LCD_Write_Data(y>>8);
  LCD_Write_Data(y&0XFF);
  LCD_Write_Data(y>>8);
  LCD_Write_Data(y&0xFF);
  
  LCD_Write_CMD(0X2E);		//读出像素点指令
  data = DATAIN;  			//dummy Read
  for(i=0; i<5; i++);		//短延时
  data = DATAIN; 			//实际坐标颜色    
  return data;  			//返回颜色值
}
/*********************************************************************************************************
* 函 数 名 : LCD_Address_Set
* 功能说明 : 设置起始和结束地址
* 形    参 : x1/y1：起止地址；x2/y2：结束地址
* 返 回 值 : 无
* 备    注 : 快速绘制
*********************************************************************************************************/ 
void LCD_Address_Set(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2)
{
	LCD_Write_CMD(0x2a);	//设置X坐标
    LCD_Write_Data(x1>> 8);	//起点X1坐标高字节  
    LCD_Write_Data(x1);     //起点X1坐标低字节
    LCD_Write_Data(x2>> 8); //终点X2坐标高字节
    LCD_Write_Data(x2);     //终点X2坐标低字节
	
	LCD_Write_CMD(0x2b); 	//设置Y坐标
    LCD_Write_Data(y1>> 8);
    LCD_Write_Data(y1);
    LCD_Write_Data(y2>> 8);
    LCD_Write_Data(y2);
	
    LCD_Write_CMD(0x2c);	//写入颜色数据
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
	TIM1->CCR1 = 0;						//初始关闭背光
#else
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	GPIO_ResetBits(GPIOA, GPIO_Pin_8);	//初始关闭背光
#endif	
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);	//默认使能写
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
	LCD_Write_Data(0x05); 	//设置16位BPP
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

	LCD_Write_CMD(0X3A); 	//设定16BPP
	LCD_Write_Data(0X55);	
	LCD_Write_CMD(0x11); 	// 退出睡眠状态
    Delay_Ms(120);
    LCD_Write_CMD(0x29); 	// 开启显示	
	
	LCD_Display_Dir(1);		//横屏显示
	
	LCD_Fill(0, 0, 480, 320, WHITE);    //清屏

#if LCD_Backlight
	TIM1->CCR1 = 1000;		
#else
	LCD_LED = 1;
#endif
}

/*********************************************************************************************************
* 功能说明 : 绘制一个矩形
* 形    参 : x0/y0：矩形的宽和高的起始坐标    x1/y1：矩形的宽和高的结束坐标   color:矩形边框颜色
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_Draw_Rectangle(u16 x0, u16 y0, u16 x1, u16 y1, u16 color)
{
	int i;	
	for(i=x0;i<x1;i++)  //绘制平行线
	{
		LCD_Draw_Point(i, y0,color);
		LCD_Draw_Point(i, y1,color);
	}
	for(i=y0;i<=y1;i++)  //绘制竖线
	{
		LCD_Draw_Point(x0,i,color);
		LCD_Draw_Point(x1,i,color);
	}
}
/*********************************************************************************************************
* 功能说明 : 绘制一个矩形，并且填充矩形背景颜色
* 形    参 : x0/y0：矩形的宽和高的起始坐标    x1/y1：矩形的宽和高的结束坐标   frame_color:矩形边框颜色   backg_color:矩形背景颜色
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_Draw_FillRectangle(u16 x0, u16 y0, u16 x1, u16 y1, u16 frame_color,u16 backg_color)
{
	int i,j;
	LCD_Draw_Rectangle(x0, y0, x1,y1,frame_color);  //绘制矩形
	for(j=y0+1;j<y1;j++)  //控制换行填充
	{
		for(i=x0+1;i<x1;i++)  //填充1根横线
		{
			LCD_Draw_Point(i,j,backg_color);   //画点		
		}
	}	
}


/*********************************************************************************************************
* 功能说明 : 字符+汉字混合显示
* 形    参 : x、y：起止坐标，ch:要显示的内容，pen_color：画笔颜色，back_color：背景颜色  size:显示字体大小
* 返 回 值 : 无
* 备    注 : 具备索引功能
*********************************************************************************************************/ 
void LCD_ShowChineseChar(u16 x, u16 y, char *ch, u16 pen_color, u16 back_color,u8 size)
{
	unsigned char i = 0,j = 0,k = 0;
	unsigned char temp = 0;
	
	while(*ch)   //遇到\0说明数据显示完毕
	{
		if(*ch > 127)   //汉字
		{
			for(i=0; i<sizeof(indexes)/sizeof(indexes[0]); i++)
			{
				if(*ch == indexes[i][0] && *(ch+1) == indexes[i][1])	//分别比较该中文的两个字节是否相等
					break;												//如果完全相等那么就得到了该中文在字模数组中的位置
			}
			if(i>=sizeof(indexes)/sizeof(indexes[0]))	return;
			LCD_Address_Set(x, y, x+32-1, y+32-1);				//挖好坑
			for(k=0; k<128; k++)
			{
				temp = GB2312[i][k];
				for(j=0; j<8; j++)								//逐个位的判断，并显示对应的颜色
				{
					if(temp&0x01)	LCD_Write_Data(pen_color);	//如果该位为1就画前景色
					else 			LCD_Write_Data(back_color);	//如果该位为0就画背景色
					temp >>= 1;
				}
			}
			x+=32;
			ch+=2; 
		}else{   //字符
			LCD_ShowChar(x, y,*ch,pen_color, back_color,16);  //显示一个字符			
			x+=8;  //为下一数据显示进行移位
		    ch+=1; 
		}
	}
	
}


/*********************************************************************************************************
* 功能说明 : 绘制一个矩形，填充矩形背景颜色，并且居中显示数据
* 形    参 : x0/y0：矩形的宽和高的起始坐标    x1/y1：矩形的宽和高的结束坐标   
									font_color:矩形边框+字体颜色   backg_color:矩形+字体背景颜色    ch:矩形居中显示的数据
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void LCD_Show_RectangleData(u16 x0, u16 y0, u16 x1, u16 y1,char* ch, u16 font_color,u16 backg_color)
{
	u16 wide=strlen(ch)*8;  //计算显示数据的宽度像素值
	LCD_Draw_FillRectangle(x0, y0, x1,y1, font_color,backg_color);   //绘制一个填充好颜色的矩形
	LCD_ShowChineseChar(((x1-x0)-wide)/2+x0, ((y1-y0)-16)/2+y0,ch,font_color,backg_color,16);  //字符+汉字混合显示	
}


