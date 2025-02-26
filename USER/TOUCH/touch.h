#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "io_bit.h"

#define T_PEN   PCin(1)  	
#define T_MISO 	PCin(2)   	
#define T_MOSI 	PCout(3)  	
#define T_SCK 	PCout(0)  	
#define T_CS  	PCout(13)  


typedef struct
{
	unsigned short xval;
	unsigned short yval;
}TOUCH_TypeDef;

#define EEPROM_ADJUST_ADDR_BASE 0	  		//�������ⲿEEPROM����ĵ�ַ�����ַ
#define FLASH_ADJUST_ADDR_BASE  2046*4096 	//�������ⲿFLASH����ĵ�ַ�����ַ
#define ADJUST_FLAG 0X1  					//У׼��־�������Ҫ�ٴ�У׼��ֻ��Ҫ�������ֵ�Ϳ�����

void Touch_Init(void);
void Touch_Adjust(void);
unsigned char Touch_Scan(TOUCH_TypeDef *touch, unsigned char mode);
void Touch_SaveAdjdata(void);
bool Touch_IsAdjusting(void);
void get_xpt2046_adjust_xyval(TOUCH_TypeDef *touch_val);  //��ȡУ׼���x/y������ֵ

#endif
