#ifndef __25QXX_H
#define __25QXX_H

#include "io_bit.h"

#define	W25QXX_CS PAout(4) 

//W25QÖ¸Áî±í
#define W25QXX_WriteEn			    0x06 	//Ð´Ê¹ÄÜ 
#define W25QXX_WriteDis			    0x04 	//Ð´Ê§ÄÜ
#define W25QXX_ReadSR		        0x05 	//¶Á¼Ä´æÆ÷ 
#define W25QXX_WriteSR		        0x01 	//Ð´¼Ä´æÆ÷ 
#define W25QXX_ReadData				0x03 	//¶ÁÊý¾Ý 
#define W25QXX_PageProgram			0x02 	//Ò³Ð´ 
#define W25QXX_SectorErase			0x20 	//ÉÈÇø²Á³ý 
#define W25QXX_BlockErase			0xD8 	//Çø¿é²Á³ý 
#define W25QXX_ReadId 				0x90	//¶ÁID

void W25Qxx_Init(void);
void W25Qxx_WriteEnable(void); 
void W25Qxx_WriteDisable(void);
void W25Qxx_WaitBusy(void);  
unsigned char W25QXX_ReadStatusRegister(void);
void W25Qxx_WriteStatusRegister(unsigned char data);
unsigned short W25Qxx_ReadID(void);

void W25Qxx_ReadDatas(unsigned char* buffer, unsigned int read_address, unsigned short num_data);  
void W25Qxx_WritePage(unsigned char* buffer, unsigned int write_addr, unsigned short num_data); 

void W25Qxx_EraseSector(unsigned int sector_addr);  	 

void USART1_W25Qxx(unsigned short sector,unsigned short n);

#endif

