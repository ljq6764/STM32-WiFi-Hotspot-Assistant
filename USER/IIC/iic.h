#ifndef _IIC_H_
#define _IIC_H_

#include "io_bit.h"              

#define ACK	 0
#define	NACK 1  

#define IIC_SCL       PCout(5)     //SCL时钟引脚
#define IIC_SDA    	  PCout(4)     //输出SDA	 
#define IIC_SDAIN     PCin(4)      //输入SDA 

void IIC_Start(void);
void IIC_Stop(void);
unsigned char IIC_Wait_ACK(void);
void IIC_WriteByte(unsigned char data);
unsigned char IIC_ReadByte(unsigned char ack);

#endif
