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

#define EEPROM_ADJUST_ADDR_BASE 0	  		//保存在外部EEPROM里面的地址区间基址
#define FLASH_ADJUST_ADDR_BASE  2046*4096 	//保存在外部FLASH里面的地址区间基址
#define ADJUST_FLAG 0X1  					//校准标志，如果需要再次校准，只需要更改这个值就可以了

void Touch_Init(void);
void Touch_Adjust(void);
unsigned char Touch_Scan(TOUCH_TypeDef *touch, unsigned char mode);
void Touch_SaveAdjdata(void);
bool Touch_IsAdjusting(void);
void get_xpt2046_adjust_xyval(TOUCH_TypeDef *touch_val);  //获取校准后的x/y的坐标值

#endif
