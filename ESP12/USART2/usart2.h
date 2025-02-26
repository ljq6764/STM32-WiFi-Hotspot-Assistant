#ifndef __USART2_H
#define __USART2_H

#include "io_bit.h"

#define USART2_RX_BUFF_MAX 128
#define USART2_NETWORK_BUFF_MAX 4096

typedef struct
{
	unsigned char *cacheRxBuff;
	unsigned char RxBuff[USART2_RX_BUFF_MAX];
	unsigned char RecFlag;
	unsigned short RecLen;	
	unsigned short cacheLen;
}USART2_InitStructure;

extern USART2_InitStructure usart2;
extern unsigned char cRxBuff[USART2_NETWORK_BUFF_MAX];

void Usart2_Init(unsigned int bound);
void Usart2_SendString(char *data);
void Usart2_SendPackage(unsigned char *data, unsigned short len);

#endif
