#include "at24c02.h"
#include "systick.h"
#include "iic.h"
/*********************************************************************************************************
* 函 数 名 : At24C02_Init
* 功能说明 : At24C02端口初始化
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 标准开漏输出，非主流你开心就好
*********************************************************************************************************/ 
void At24C02_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能PB外设时钟
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;		//初始化引脚
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;			//开漏输出模式
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;			//2MHZ
	GPIO_Init(GPIOC, &GPIO_InitStruct);						//根据GPIO_InitStruct初始化
	
	GPIO_SetBits(GPIOC, GPIO_Pin_4 | GPIO_Pin_5);			//PC10输出高电平，灯灭
}
/*********************************************************************************************************
* 函 数 名 : At24CXX_WriteOneByte
* 功能说明 : 向At24C02写入一个字节
* 形    参 : writeAddr：写入地址；dataToWrite：需要写入的数据
* 返 回 值 : 无
* 备    注 : writeAddr：写在那个地址，dataToWrite：写什么内容
*********************************************************************************************************/ 
void At24C02_WriteOneByte(unsigned char writeAddr,unsigned char dataToWrite)
{
	IIC_Start();						//起止信号
	IIC_WriteByte(0xa0);				//写入器件地址+写，0xa0
	if(IIC_Wait_ACK() == ACK){			//等待对方应答
		IIC_WriteByte(writeAddr);		//写入数据地址，WriteAddr
		if(IIC_Wait_ACK() == ACK){		//等待对方应答
			IIC_WriteByte(dataToWrite); //写入数据，DataToWrite
			if(IIC_Wait_ACK() == ACK){	//等待对方应答
				IIC_Stop();				//结束信号
			}else goto error;
		}else goto error;
	}else goto error;

error:
	IIC_Stop();	
}
/*********************************************************************************************************
* 函 数 名 : At24C02_ReadOneByte
* 功能说明 : 从At24C02读取一个字节
* 形    参 : readAddr：从哪个地址读
* 返 回 值 : 读取到的字节
* 备    注 : readAddr：从哪里读取
*********************************************************************************************************/ 
unsigned char At24C02_ReadOneByte(unsigned char readAddr)
{
	unsigned char temp;
	
	IIC_Start();							//起止信号
	IIC_WriteByte(0xa0);					//写入器件地址+写，0xa0
	if(IIC_Wait_ACK() == ACK){				//等待对方应答
		IIC_WriteByte(readAddr);			//写入数据地址，ReadAddr
		if(IIC_Wait_ACK() == ACK){			//等待对方应答
			IIC_Start();					//起止信号
			IIC_WriteByte(0xa1);			//写入器件地址+读，0xa1
			if(IIC_Wait_ACK() == ACK){		//等待对方应答
				temp = IIC_ReadByte(NACK);	//读取数据，对方返回数据，主机响应非应答
				IIC_Stop();					//结束信号
				return temp;
			}else goto error;
		}else goto error;
	}else goto error;
	
error:
	IIC_Stop();
	return temp;
}
/*********************************************************************************************************
* 函 数 名 : At24C02_WritePageByte
* 功能说明 : At24C02页写
* 形    参 : dataToWrite：写入的数据的指针；pageAddr：从那页开始写（0~31）；num：写入的字节数
* 返 回 值 : 无
* 备    注 : 写什么，从哪页开始写，写多少个
*********************************************************************************************************/ 
void At24C02_WritePageByte(unsigned char *dataToWrite, unsigned char pageAddr, unsigned short num)
{
	unsigned short cnt, i;
					
	for(i=0; i<(num>>3); i++){	//要写多少页才能写完,最少一页
		IIC_Start();				//起止信号
		IIC_WriteByte(0xa0);		//写器件地址
		if(IIC_Wait_ACK() == ACK){					//等待对方应答
			IIC_WriteByte(pageAddr);				//要写入数据的地址
			if(IIC_Wait_ACK() == ACK){				//等待对方应答	
				for(cnt=0; cnt<((num>>3)?8:(num&0x7)); cnt++){
					IIC_WriteByte(*dataToWrite++);	//要写入的数据
					if(IIC_Wait_ACK() == NACK){		//等待对方应答
						goto error;
					}
				}
				IIC_Stop();			//结束信号
				num -= 8;			//每次写入8个字节
				pageAddr += 8;		//地址整页偏移
				Delay_Ms(10);		//等待写周期
			}else goto error;
		}else goto error;
	}
error:
	IIC_Stop();
}
/*********************************************************************************************************
* 函 数 名 : At24C02_ReadPageByte
* 功能说明 : At24C02页读
* 形    参 : pBuffer：数据缓冲区；readAddr：读取的地址；num：读取的字节数
* 返 回 值 : 无
* 备    注 : 存到哪里去，从哪里开始读，读多少个
*********************************************************************************************************/ 
void At24C02_ReadPageByte(unsigned char *pBuffer, unsigned char readAddr, unsigned short num)
{
	unsigned short cnt;
	
	IIC_Start();							//起止信号
	IIC_WriteByte(0xa0);					//写器件地址
	if(IIC_Wait_ACK() == ACK){				//等待对方应答
		IIC_WriteByte(readAddr);			//要读取的数据的地址
		if(IIC_Wait_ACK() == ACK){			//等待对方应答
			IIC_Start();					//起止信号
			IIC_WriteByte(0xa1);			//读器件地址
			if(IIC_Wait_ACK() == ACK){				//等待对方应答
				for(cnt=0; cnt<num-1; cnt++){
					*pBuffer = IIC_ReadByte(ACK);	//读取一个字节，并给从机应答
					pBuffer++;	
				}
				*pBuffer = IIC_ReadByte(NACK);		//读取一个字节，不给应答
				IIC_Stop();							//结束信号
			}else goto error;
		}else goto error;
	}else goto error;
error:
	IIC_Stop();	
}
