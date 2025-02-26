#ifndef __USART3_H
#define __USART3_H

#include "io_bit.h"

#define USART3_RECEIVE_DATA_LENGTH_MAX 10

typedef struct
{
	unsigned char RxBuff[USART3_RECEIVE_DATA_LENGTH_MAX];
	unsigned char RecFlag;
	unsigned short RecLen;	
}USART3_InitStructure;

extern USART3_InitStructure usart3;

void Usart3_Init(u32 bound);
void Usart3_SendString(u8 *data);
void Usart3_SendPackage(u8 *data,u16 len);
void Usart_StatusClear(USART_TypeDef* USARTx);

#endif
