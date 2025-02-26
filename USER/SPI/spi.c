#include "spi.h"
/*********************************************************************************************************
* �� �� �� : Spi1_WriteReadByte
* ����˵�� : Ӳ��SPI1��дһ���ֽ�
* ��  �� : data��д�������
* �� �� ֵ : ��ȡ��������
* ��  ע : ��
*********************************************************************************************************/ 
unsigned char Spi1_WriteReadByte(unsigned char data)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)  == RESET);		//�ȴ����ͻ�����Ϊ��
	SPI_I2S_SendData(SPI1, data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);		//�ȴ����ͻ�������Ϊ��
	return SPI_I2S_ReceiveData(SPI1);
}
/*********************************************************************************************************
* �� �� �� : Spi2_WriteReadByte
* ����˵�� : Ӳ��SPI2��дһ���ֽ�
* ��  �� : data��д�������
* �� �� ֵ : ��ȡ��������
* ��  ע : ��
*********************************************************************************************************/ 
unsigned char Spi2_WriteReadByte(unsigned char data)
{
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)  == RESET);		//�ȴ����ͻ�����Ϊ��
	SPI_I2S_SendData(SPI2, data);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);		//�ȴ����ͻ�������Ϊ��
	return SPI_I2S_ReceiveData(SPI2);
}
/*********************************************************************************************************
* �� �� �� : Spi3_WriteReadByte
* ����˵�� : Ӳ��SPI3��дһ���ֽ�
* ��  �� : data��д�������
* �� �� ֵ : ��ȡ��������
* ��  ע : ��
*********************************************************************************************************/ 
unsigned char Spi3_WriteReadByte(unsigned char data)
{				
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE)  == RESET);		//�ȴ����ͻ�����Ϊ��
	SPI_I2S_SendData(SPI3, data);
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);	//�ȴ����ͻ�������Ϊ��
	return SPI_I2S_ReceiveData(SPI3);	  
}
/*********************************************************************************************************
* �� �� �� : Spi1_Init
* ����˵�� : ��ʼ��Ӳ��SPI1
* ��  �� : ��
* �� �� ֵ : ��
* ��  ע : ��
*********************************************************************************************************/ 
void Spi1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	SPI_InitTypeDef SPI_InitStruct		= {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,  ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;	//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//��ʼ��

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	//�ķ�Ƶ�����48M��
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;						//�ڶ���ʱ�ӱ��ز���
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;						//ʱ�ӿ���Ϊ��
	SPI_InitStruct.SPI_CRCPolynomial = 0x7;							//CRC����ʽ
	SPI_InitStruct.SPI_DataSize  = SPI_DataSize_8b;					//��λ����֡��ʽ
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//˫��˫��ȫ˫��
	SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;				//��λ�ȷ�
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;						//��ģʽ
	SPI_InitStruct.SPI_NSS  = SPI_NSS_Soft;							//�������
	SPI_Init(SPI1, &SPI_InitStruct);
	
	SPI_Cmd(SPI1, ENABLE);											//ʹ������
}

/*********************************************************************************************************
* �� �� �� : Spi2_Init
* ����˵�� : ��ʼ��Ӳ��SPI2
* ��  �� : ��
* �� �� ֵ : ��
* ��  ע : ��
*********************************************************************************************************/ 
void Spi2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	SPI_InitTypeDef SPI_InitStruct		= {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//������ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,  ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;	//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//��ʼ��

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//����Ƶ
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;						//�ڶ���ʱ�ӱ��ز���
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;						//ʱ�ӿ���Ϊ��
	SPI_InitStruct.SPI_CRCPolynomial = 0x7;							//CRC����ʽ
	SPI_InitStruct.SPI_DataSize  = SPI_DataSize_8b;					//��λ����֡��ʽ
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//˫��˫��ȫ˫��
	SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;				//��λ�ȷ�
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;						//��ģʽ
	SPI_InitStruct.SPI_NSS  = SPI_NSS_Soft;							//�������
	SPI_Init(SPI2, &SPI_InitStruct);
	SPI_Cmd(SPI2, ENABLE);											//ʹ������
}
/*********************************************************************************************************
* �� �� �� : Spi3_Init
* ����˵�� : ��ʼ��Ӳ��SPI3
* ��  �� : ��
* �� �� ֵ : ��
* ��  ע : ��
*********************************************************************************************************/ 
void Spi3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	SPI_InitTypeDef SPI_InitStruct		= {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//������ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,  ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//��ʼ��

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	//����Ƶ
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;						//�ڶ���ʱ�ӱ��ز���
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;						//ʱ�ӿ���Ϊ��
	SPI_InitStruct.SPI_CRCPolynomial = 0x7;							//CRC����ʽ
	SPI_InitStruct.SPI_DataSize  = SPI_DataSize_8b;					//��λ����֡��ʽ
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//˫��˫��ȫ˫��
	SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;				//��λ�ȷ�
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;						//��ģʽ
	SPI_InitStruct.SPI_NSS  = SPI_NSS_Soft;							//�������
	SPI_Init(SPI3, &SPI_InitStruct);
	
	SPI_Cmd(SPI3, ENABLE);											//ʹ������
}

