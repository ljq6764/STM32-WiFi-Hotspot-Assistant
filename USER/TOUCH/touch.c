#include "touch.h"
#include "lcd.h"
#include "systick.h"
#include <math.h>
#include "w25qxx.h"
#include "at24c02.h"

float Ky, Kx;	//У��б�ʲ���
int   By, Bx;	//У��ƫ�Ʋ���

/*********************************************************************************************************
* �� �� �� : Spi_SendByte
* ����˵�� : ģ��spiʱ����һ���ֽ�
* ��    �� : data����Ҫ���͵��ֽ�
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
static void Spi_SendByte(unsigned char data)
{
	unsigned char i;
	
	T_SCK = 1;					//����Ϊ��
	for(i=0; i<8; i++){
		T_SCK = 0;				//����һ���½��أ�SMT32д
		T_MOSI = !!(data&0x80);	//������λ��״̬�ŵ�����
		data <<= 1;				//�������λ
		Delay_Us(1);
		T_SCK = 1;				//XPT2046���������ֲ�������������DCLK�������������
		Delay_Us(1);
	}
}
/*********************************************************************************************************
* �� �� �� : Spi_ReceByte
* ����˵�� : ģ��spi����һ���ֽڵ�����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
static unsigned char Spi_ReceByte(void)
{
	unsigned char data, i;
	
	T_SCK = 1;		//����Ϊ��	
	for(i=0; i<8; i++){
		T_SCK = 0;		//SXPT2046д�������ֲ�������������DCLK���½����Ƴ�
		Delay_Us(1);
		T_SCK = 1;		//STM32��
		Delay_Us(1);
		data <<= 1;
		data |= !!T_MISO;//��ȡ�����ߵ�״̬	
	}
	return data; 
}
/*********************************************************************************************************
* �� �� �� : Touch_ReadAD
* ����˵�� : ��ȡx��ADֵ
* ��    �� : ��
* �� �� ֵ : ��ȡ��������
* ��    ע : ��
*********************************************************************************************************/ 
unsigned short Touch_ReadAD(unsigned char cmd)
{
	unsigned char VH, VL;
	
	T_CS = 0;				//ѡ����������ʼͨ��
	Spi_SendByte(cmd);		//���Ͳ�������
	Delay_Us(1);
	VH = Spi_ReceByte();	//��ȡ��λ�ֽ�
	VL = Spi_ReceByte();	//��ȡ��λ�ֽ�
	T_CS = 1;				//ѡ����������ʼͨ��
	return (((VH << 8) | VL) >> 4);	//�ϲ��ߵ�λ�ֽڣ�����ȥ������ĸ�λ����Чλ
}
/*********************************************************************************************************
* �� �� �� : Touch_ReadAD_XY
* ����˵�� : ��ȡx��y���ADֵ������ƽ��
* ��    �� : ad�����ݽṹ��
* �� �� ֵ : ��
* ��    ע : ��������ȥ����ͷ��������ƽ��
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
* �� �� �� : Touch_Scan
* ����˵�� : ������ɨ��
* ��    �� : touch�����ݽṹ�壬mode��0���������㣬1��������
* �� �� ֵ : 0���д��㰴�£�1���޴��㰴��
* ��    ע : ��
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
* �� �� �� : Touch_Adjust
* ����˵�� : ������У׼
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Touch_Adjust(void)
{
	unsigned short lcd_pos[4][2] = {20, 20, 460, 20, 20, 300, 460, 300};
	TOUCH_TypeDef touch_pos[4];	//������� x,y��4��ADֵ
	unsigned char i, j;
	double len1=0.00f, len2=0.00f;
	
	LCD_ShowString(30, 130, "touch adjust start", RED, WHITE, 16);
	while(1){
		
        for(i=0; i<4; i++){			//������ȡ4��LCD�����Ӧ�Ĵ���������
			for(j=0; j<30; j++){	//��һ��ʮ�ּ�
				LCD_Draw_Point(lcd_pos[i][0]-15+j, lcd_pos[i][1], RED);
				LCD_Draw_Point(lcd_pos[i][0], lcd_pos[i][1]-15+j, RED);
			}
			printf("Wait for the check\r\n");
            while(T_PEN);		//�ȴ����´����� 
			Delay_Ms(50);		//��ʱ50ms�������ȶ�
			printf("Press the touch screen\r\n");
            Touch_ReadAD_XY(&touch_pos[i]);	//��ô�����������x,y����ֵ
            while(!T_PEN); 					//�ȴ��ɿ���
			Delay_Ms(200);
			
			for(j=0;j<30;j++)//���ʮ�ּ�ͼ��
			{
				LCD_Draw_Point(lcd_pos[i][0]-15+j, lcd_pos[i][1], WHITE);
				LCD_Draw_Point(lcd_pos[i][0], lcd_pos[i][1]-15+j, WHITE);
			}
        }
		
        //У������-�������Ĵ������Ƿ���ȷ  �������ȷ����У׼
        //ˮƽ������֮��ľ���Ƚ�		
        len1 = (float)sqrt((touch_pos[1].xval-touch_pos[0].xval) * (touch_pos[1].xval-touch_pos[0].xval) \
                     + (touch_pos[1].yval-touch_pos[0].yval) * (touch_pos[1].yval-touch_pos[0].yval));
        len2 = (float) sqrt((touch_pos[3].xval-touch_pos[2].xval) * (touch_pos[3].xval-touch_pos[2].xval) \
                     + (touch_pos[3].yval-touch_pos[2].yval) * (touch_pos[3].yval-touch_pos[2].yval));		
        if(((len1/len2)<0.95) || ((len1/len2)>1.05)){
            continue; //����ĵ㲻����Ҫ��
        }
		
        //��ֱ������֮��ľ���Ƚ�
        len1 = (float)sqrt((touch_pos[2].xval-touch_pos[0].xval) * (touch_pos[2].xval-touch_pos[0].xval) \
                     + (touch_pos[2].yval-touch_pos[0].yval) * (touch_pos[2].yval-touch_pos[0].yval));
        len2 = (float)sqrt((touch_pos[3].xval-touch_pos[1].xval) * (touch_pos[3].xval-touch_pos[1].xval) \
                     + (touch_pos[3].yval-touch_pos[1].yval) * (touch_pos[3].yval-touch_pos[1].yval));		
        if(((len1/len2)<0.95) || ((len1/len2)>1.05)){
            continue;//����ĵ㲻����Ҫ��
        }
		
        //�Խ���������֮��ľ���Ƚ�
        len1 = (float)sqrt((touch_pos[3].xval-touch_pos[0].xval) * (touch_pos[3].xval-touch_pos[0].xval) \
                     + (touch_pos[3].yval-touch_pos[0].yval) * (touch_pos[3].yval-touch_pos[0].yval));
        len2 = (float)sqrt((touch_pos[2].xval-touch_pos[1].xval) * (touch_pos[2].xval-touch_pos[1].xval) \
                     + (touch_pos[2].yval-touch_pos[1].yval) * (touch_pos[2].yval-touch_pos[1].yval));		
        if(((len1/len2)<0.95) || ((len1/len2)>1.05)){
            continue;//����ĵ㲻����Ҫ��
        }	
				
        //����У׼����   Kx (Ky)--б�� ��Bx(By) --ƫ����
        //����xӳ�� Xlcd = Kx * touch_x + Bx
        Kx = (float)(lcd_pos[1][0]-lcd_pos[0][0]) / (touch_pos[1].xval-touch_pos[0].xval);
        Bx = lcd_pos[0][0] - Kx*touch_pos[0].xval;
        //����yӳ�� Ylcd = Ky*touch_y + By
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
* �� �� �� : Touch_isAdjusting
* ����˵�� : ���洢�����Ƿ񱣴���У׼��Ϣ����������ȡ����
* ��    �� : ��
* �� �� ֵ : true����У׼��false��δУ׼
* ��    ע : ��
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
* �� �� �� : Touch_SaveAdjdata
* ����˵�� : ����У׼����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
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
* �� �� �� : Touch_Init
* ����˵�� : �������ӿڳ�ʼ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
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
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;			//��������
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
* Function��   get_xpt2046_adjust_xyval  
* Description: ���У׼��Ĵ���������ֵ������У׼���б�ʼ������ӦLCD�ϵ�����
* Return : touch_val--�ṹ�壬�����Ŵ������ĺ�������������ֵ
*********************************************************************/
void get_xpt2046_adjust_xyval(TOUCH_TypeDef *touch_val)
{
	TOUCH_TypeDef val1;
	if(T_PEN==0)	//���´�����
  {
			
			Touch_ReadAD_XY(&val1);//��ô�������AD��ֵ touch_val.xal touch_val.yal
				
			//ӳ������ --�Ѵ������õ���ADֵ ת���õ� LCDҺ��������
			touch_val->xval = Kx*val1.xval + Bx; //�ȵ�LCD X�������ֵ
			touch_val->yval = Ky*val1.yval + By; 
			//printf("����: xval=%d  yval=%d \r\n",touch_val->xval,touch_val->yval);
  }

}

