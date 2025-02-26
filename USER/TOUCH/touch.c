#include "touch.h"
#include "lcd.h"
#include "systick.h"
#include <math.h>
#include "w25qxx.h"
#include "at24c02.h"

float Ky, Kx;	//校验斜率参数
int   By, Bx;	//校验偏移参数

/*********************************************************************************************************
* 函 数 名 : Spi_SendByte
* 功能说明 : 模拟spi时序发送一个字节
* 形    参 : data：需要发送的字节
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
static void Spi_SendByte(unsigned char data)
{
	unsigned char i;
	
	T_SCK = 1;					//空闲为高
	for(i=0; i<8; i++){
		T_SCK = 0;				//产生一个下降沿，SMT32写
		T_MOSI = !!(data&0x80);	//把数据位的状态放到线上
		data <<= 1;				//更新最高位
		Delay_Us(1);
		T_SCK = 1;				//XPT2046读，数据手册描述：数据在DCLK上升沿锁存进来
		Delay_Us(1);
	}
}
/*********************************************************************************************************
* 函 数 名 : Spi_ReceByte
* 功能说明 : 模拟spi接收一个字节的数据
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
static unsigned char Spi_ReceByte(void)
{
	unsigned char data, i;
	
	T_SCK = 1;		//空闲为高	
	for(i=0; i<8; i++){
		T_SCK = 0;		//SXPT2046写，数据手册描述：数据在DCLK的下降沿移出
		Delay_Us(1);
		T_SCK = 1;		//STM32读
		Delay_Us(1);
		data <<= 1;
		data |= !!T_MISO;//读取数据线的状态	
	}
	return data; 
}
/*********************************************************************************************************
* 函 数 名 : Touch_ReadAD
* 功能说明 : 读取x轴AD值
* 形    参 : 无
* 返 回 值 : 读取到的数据
* 备    注 : 无
*********************************************************************************************************/ 
unsigned short Touch_ReadAD(unsigned char cmd)
{
	unsigned char VH, VL;
	
	T_CS = 0;				//选中器件，开始通信
	Spi_SendByte(cmd);		//发送测量命令
	Delay_Us(1);
	VH = Spi_ReceByte();	//读取高位字节
	VL = Spi_ReceByte();	//读取低位字节
	T_CS = 1;				//选中器件，开始通信
	return (((VH << 8) | VL) >> 4);	//合并高低位字节，并且去掉最低四个位的无效位
}
/*********************************************************************************************************
* 函 数 名 : Touch_ReadAD_XY
* 功能说明 : 读取x和y轴的AD值，并求平均
* 形    参 : ad：数据结构体
* 返 回 值 : 无
* 备    注 : 最好排序后去掉两头数据再求平均
*********************************************************************************************************/ 
void Touch_ReadAD_XY(TOUCH_TypeDef *ad)
{
	unsigned int adx = 0, ady = 0;
	unsigned char i;
	
	for(i=0; i<10; i++){
		adx += Touch_ReadAD(0x90);
		ady += Touch_ReadAD(0xd0);
	}
	ad->xval = adx / 10;
	ad->yval = ady / 10;
}
/*********************************************************************************************************
* 函 数 名 : Touch_Scan
* 功能说明 : 触摸屏扫描
* 形    参 : touch：数据结构体，mode：0不允许连点，1允许连点
* 返 回 值 : 0：有触点按下，1：无触点按下
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Touch_Scan(TOUCH_TypeDef *touch, unsigned char mode)
{
	TOUCH_TypeDef ad;
	static unsigned char flag = 0;
	
	if(mode){
		flag = 0;
	}
	
	if(!T_PEN && !flag){
		flag = 1;
		Touch_ReadAD_XY(&ad);
		touch->xval = ad.xval*Kx+Bx;
		touch->yval = ad.yval*Ky+By;
		return T_PEN;
	}else if(T_PEN){
		flag = 0;
	}
	return 1;
}
/*********************************************************************************************************
* 函 数 名 : Touch_Adjust
* 功能说明 : 触摸屏校准
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Touch_Adjust(void)
{
	unsigned short lcd_pos[4][2] = {20, 20, 460, 20, 20, 300, 460, 300};
	TOUCH_TypeDef touch_pos[4];	//用来存放 x,y的4个AD值
	unsigned char i, j;
	double len1=0.00f, len2=0.00f;
	
	LCD_ShowString(30, 130, "touch adjust start", RED, WHITE, 16);
	while(1){
		
        for(i=0; i<4; i++){			//连续读取4个LCD坐标对应的触摸屏坐标
			for(j=0; j<30; j++){	//画一个十字架
				LCD_Draw_Point(lcd_pos[i][0]-15+j, lcd_pos[i][1], RED);
				LCD_Draw_Point(lcd_pos[i][0], lcd_pos[i][1]-15+j, RED);
			}
			printf("Wait for the check\r\n");
            while(T_PEN);		//等待按下触摸屏 
			Delay_Ms(50);		//延时50ms待数据稳定
			printf("Press the touch screen\r\n");
            Touch_ReadAD_XY(&touch_pos[i]);	//获得触摸屏测量的x,y轴数值
            while(!T_PEN); 					//等待松开手
			Delay_Ms(200);
			
			for(j=0;j<30;j++)//清掉十字架图标
			{
				LCD_Draw_Point(lcd_pos[i][0]-15+j, lcd_pos[i][1], WHITE);
				LCD_Draw_Point(lcd_pos[i][0], lcd_pos[i][1]-15+j, WHITE);
			}
        }
		
        //校验坐标-计算点击的触摸点是否正确  如果不正确重新校准
        //水平两个点之间的距离比较		
        len1 = (float)sqrt((touch_pos[1].xval-touch_pos[0].xval) * (touch_pos[1].xval-touch_pos[0].xval) \
                     + (touch_pos[1].yval-touch_pos[0].yval) * (touch_pos[1].yval-touch_pos[0].yval));
        len2 = (float) sqrt((touch_pos[3].xval-touch_pos[2].xval) * (touch_pos[3].xval-touch_pos[2].xval) \
                     + (touch_pos[3].yval-touch_pos[2].yval) * (touch_pos[3].yval-touch_pos[2].yval));		
        if(((len1/len2)<0.95) || ((len1/len2)>1.05)){
            continue; //点击的点不符合要求
        }
		
        //垂直两个点之间的距离比较
        len1 = (float)sqrt((touch_pos[2].xval-touch_pos[0].xval) * (touch_pos[2].xval-touch_pos[0].xval) \
                     + (touch_pos[2].yval-touch_pos[0].yval) * (touch_pos[2].yval-touch_pos[0].yval));
        len2 = (float)sqrt((touch_pos[3].xval-touch_pos[1].xval) * (touch_pos[3].xval-touch_pos[1].xval) \
                     + (touch_pos[3].yval-touch_pos[1].yval) * (touch_pos[3].yval-touch_pos[1].yval));		
        if(((len1/len2)<0.95) || ((len1/len2)>1.05)){
            continue;//点击的点不符合要求
        }
		
        //对角线两个点之间的距离比较
        len1 = (float)sqrt((touch_pos[3].xval-touch_pos[0].xval) * (touch_pos[3].xval-touch_pos[0].xval) \
                     + (touch_pos[3].yval-touch_pos[0].yval) * (touch_pos[3].yval-touch_pos[0].yval));
        len2 = (float)sqrt((touch_pos[2].xval-touch_pos[1].xval) * (touch_pos[2].xval-touch_pos[1].xval) \
                     + (touch_pos[2].yval-touch_pos[1].yval) * (touch_pos[2].yval-touch_pos[1].yval));		
        if(((len1/len2)<0.95) || ((len1/len2)>1.05)){
            continue;//点击的点不符合要求
        }	
				
        //计算校准参数   Kx (Ky)--斜率 ；Bx(By) --偏移量
        //计算x映射 Xlcd = Kx * touch_x + Bx
        Kx = (float)(lcd_pos[1][0]-lcd_pos[0][0]) / (touch_pos[1].xval-touch_pos[0].xval);
        Bx = lcd_pos[0][0] - Kx*touch_pos[0].xval;
        //计算y映射 Ylcd = Ky*touch_y + By
        Ky = (float)( lcd_pos[2][1]-lcd_pos[0][1]) / (touch_pos[2].yval-touch_pos[0].yval);
        By = lcd_pos[0][1] - Ky*touch_pos[0].yval;     
        LCD_Fill(0, 0, 480, 320, WHITE);
		LCD_ShowString(30, 130, "touch adjust OK", RED, WHITE, 16);
		printf("calibration parameter: Ky=%f;Kx=%f;By=%d;Bx=%d;\r\n", Ky, Kx, By, Bx);
        Delay_Ms(1000);
        Delay_Ms(1000);
        LCD_Fill(0, 0, 480, 320, WHITE);
        break;				
	}	
}
/*********************************************************************************************************
* 函 数 名 : Touch_isAdjusting
* 功能说明 : 检查存储器里是否保存有校准信息，如果有则读取出来
* 形    参 : 无
* 返 回 值 : true：已校准；false：未校准
* 备    注 : 无
*********************************************************************************************************/ 
bool Touch_IsAdjusting(void)
{
	unsigned char temp;
	unsigned char buff[20] = {0};
#if 0 		  
	At24C02_ReadPageByte(buff, EEPROM_ADJUST_ADDR_BASE, 17); 
	memcpy(&temp, buff+16, 1);

	if(temp == ADJUST_FLAG){
		memcpy(&Kx, buff, 4);
		memcpy(&Bx, buff+4, 4);
		memcpy(&Ky, buff+8, 4);
		memcpy(&By, buff+12, 4);
		printf("read: Ky=%f;Kx=%f;By=%d;Bx=%d;\r\n", Ky, Kx, By, Bx);
		return true;
	}
#else		  
	W25Qxx_ReadDatas(buff, FLASH_ADJUST_ADDR_BASE, 17);	
	memcpy(&temp, buff+16, 1);
	
	if(temp == ADJUST_FLAG){ 		
		memcpy(&Kx, buff, 4);
		memcpy(&Bx, buff+4, 4);
		memcpy(&Ky, buff+8, 4);
		memcpy(&By, buff+12, 4);	
		printf("read: Ky=%f;Kx=%f;By=%d;Bx=%d;\r\n", Ky, Kx, By, Bx);
		
		return true;		 
	}
#endif 
	
	return false;
}
/*********************************************************************************************************
* 函 数 名 : Touch_SaveAdjdata
* 功能说明 : 保存校准数据
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 									    
void Touch_SaveAdjdata(void)
{  
	unsigned char temp = ADJUST_FLAG;	
	unsigned char buff[20] = {0};
#if 0
	memcpy(buff, &Kx, 4);
	memcpy(buff+4, &Bx, 4);
	memcpy(buff+8, &Ky, 4);
	memcpy(buff+12, &By, 4);
	memcpy(buff+16, &temp, 1);
	At24C02_WritePageByte(buff, EEPROM_ADJUST_ADDR_BASE, 17); 
	printf("write: Ky=%f;Kx=%f;By=%d;Bx=%d;\r\n", Ky, Kx, By, Bx);
#else
	W25Qxx_EraseSector(FLASH_ADJUST_ADDR_BASE/4096);
	memcpy(buff, &Kx, 4);
	memcpy(buff+4, &Bx, 4);
	memcpy(buff+8, &Ky, 4);
	memcpy(buff+12, &By, 4);
	memcpy(buff+16, &temp, 1);
	W25Qxx_WritePage(buff, FLASH_ADJUST_ADDR_BASE, 17);
	printf("write: Ky=%f;Kx=%f;By=%d;Bx=%d;\r\n", Ky, Kx, By, Bx);
#endif
}
/*********************************************************************************************************
* 函 数 名 : Touch_Init
* 功能说明 : 触摸屏接口初始化
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Touch_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);	
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_13);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;			//上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_2);

	if(Touch_IsAdjusting()){
		return;
	}else{ 			
		Touch_Adjust();  
		Touch_SaveAdjdata();	 
	}				
}

/********************************************************************
* Function：   get_xpt2046_adjust_xyval  
* Description: 获得校准后的触摸屏坐标值，根据校准后的斜率计算出对应LCD上的坐标
* Return : touch_val--结构体，里面存放触摸屏的横坐标和纵坐标的值
*********************************************************************/
void get_xpt2046_adjust_xyval(TOUCH_TypeDef *touch_val)
{
	TOUCH_TypeDef val1;
	if(T_PEN==0)	//按下触摸屏
  {
			
			Touch_ReadAD_XY(&val1);//获得触摸屏的AD的值 touch_val.xal touch_val.yal
				
			//映射坐标 --把触摸屏得到的AD值 转换得到 LCD液晶屏坐标
			touch_val->xval = Kx*val1.xval + Bx; //等到LCD X轴的坐标值
			touch_val->yval = Ky*val1.yval + By; 
			//printf("坐标: xval=%d  yval=%d \r\n",touch_val->xval,touch_val->yval);
  }

}

