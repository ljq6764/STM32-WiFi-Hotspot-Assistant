#include "usart1.h"

USART1_TypeDef usart1;

/*********************************************************************************************************
* �� �� �� : Usart1_SendString
* ����˵�� : USART1�����ַ�������
* ��    �� : str����Ҫ���͵��ַ���
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Usart1_SendString(unsigned char *str)
{
	while(*str != 0)		
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//�ȴ���һ���ֽڷ������
		USART_SendData(USART1, *str++);								//����һ���ֽ�
	}
}
/*********************************************************************************************************
* �� �� �� : Usart1_SendPackage
* ����˵�� : USART1���ݰ����ͺ���
* ��    �� : data����Ҫ���͵����ݣ�len�����͵����ݵĳ���
* �� �� ֵ : ��
* ��    ע : ���ݰ��м���ܻ������\0��, ������Ҫ�������Ƚ��з���
*********************************************************************************************************/ 
void Usart1_SendPackage(unsigned char *data, unsigned short len)	
{
	while(len--)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//�ȴ���һ���ֽڷ������
		USART_SendData(USART1, *data++);							//����һ���ֽ�
	}
}
/*********************************************************************************************************
* �� �� �� : USART1_IRQHandler
* ����˵�� : USART1�жϷ�����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : �����ж�+�����ж�, ÿ���յ�һ���ֽڣ��ͻ�ִ��һ��
*********************************************************************************************************/ 
void USART1_IRQHandler(void)									
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)				//����յ������ݣ��жϴ�����ʲô�жϣ�
	{
		usart1.RxBuff[usart1.RecLen] = USART_ReceiveData(USART1);	//��ȡ����
		if(++usart1.RecLen > USART1_RX_BUFF_MAX)					//������յ������ݳ�����������С����ô��ͷ��ʼ���
			usart1.RecLen = 0;
	}
	else if(USART_GetITStatus(USART1, USART_IT_IDLE) == SET)		//��������˿����жϣ���ô�ַ����������
	{
		usart1.RxBuff[usart1.RecLen] = 0;							//�ڵ�ǰ��ĩβ��ӽ�����			
		usart1.RecFlag = 1;											//��ʾλ��1��֪ͨ��������д���
		USART_ReceiveData(USART1);									//��ȡDR���SR����
	}
}
/*********************************************************************************************************
* �� �� �� : Usart1_Init
* ����˵�� : ��ʼ��USART1
* ��    �� : bound��������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Usart1_Init(unsigned int bound)
{
	GPIO_InitTypeDef 	GPIO_InitStruct		= {0};
	USART_InitTypeDef 	USART_InitStruct	= {0};
	NVIC_InitTypeDef 	NVIC_InitStruct		= {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,  ENABLE);
  
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	USART_InitStruct.USART_BaudRate 	= bound;									//������
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��ʹ��Ӳ��������
	USART_InitStruct.USART_Mode 		= USART_Mode_Tx | USART_Mode_Rx;			//�շ�ģʽ
	USART_InitStruct.USART_Parity 		= USART_Parity_No;							//��ʹ����żУ��
	USART_InitStruct.USART_StopBits 	= USART_StopBits_1;							//һλֹͣλ
	USART_InitStruct.USART_WordLength 	= USART_WordLength_8b;						//��λ�����ݳ���
	USART_Init(USART1, &USART_InitStruct);
	
	USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);			//ʹ�ܽ����ж�
	USART_ITConfig(USART1,USART_IT_IDLE, ENABLE);			//ʹ�ܿ����ж�

	NVIC_InitStruct.NVIC_IRQChannel    = USART1_IRQn;		//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//��Ӧ���ȼ�2
	NVIC_Init(&NVIC_InitStruct);	
	
	USART_Cmd(USART1, ENABLE);								//ʹ������
}

/*****�������´���,֧��printf����,������Ҫѡ��use MicroLIB*****/	  
#if 1
	#pragma import(__use_no_semihosting)             
	//��׼����Ҫ��֧�ֺ���                 
	struct __FILE 
	{ 
		int handle; 
	}; 

	FILE __stdout;       
	//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
	void _sys_exit(int x) 
	{ 
		x = x; 
	} 
	void _ttywrch(int ch)
	{
		ch = ch;
	}
	//�ض���fputc���� 
	int fputc(int ch, FILE *f)
	{ 	
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(USART1, ch);	
		return ch;
	}
/**********************printf support end**********************/	  
#endif
