#ifndef _AT24C02_H_
#define _AT24C02_H_

#include "io_bit.h"  

void At24C02_Init(void);

void At24C02_WriteOneByte(unsigned char writeAddr, unsigned char dataToWrite);
unsigned char At24C02_ReadOneByte(unsigned char readAddr);

void At24C02_WritePageByte(unsigned char *dataToWrite, unsigned char pageAddr, unsigned short num);
void At24C02_ReadPageByte(unsigned char *pBuffer, unsigned char readAddr, unsigned short num);

#endif
