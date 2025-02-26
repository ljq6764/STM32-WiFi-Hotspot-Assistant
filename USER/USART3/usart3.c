#include "usart3.h"
#include "usart1.h"
#include "syn6658.h"

USART3_InitStructure usart3;

void Usart_StatusClear(USART_TypeDef* USARTx)
{
	usart3.RecFlag = false;
	usart3.RecLen = 0;
}

/*********************************************************************************************************
* �� �� �� : Usart3_SendString
* ����˵�� : USART3�����ַ�������
* ��    �� : str����Ҫ���͵��ַ���
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Usart3_SendString(unsigned char *data)		//�������ݣ�ĩβ���С�\0��
{
	while(*data != '\0')
	{
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TC));
		USART_SendData(USART3, *data++);
	}
}
/*********************************************************************************************************
* �� �� �� : Usart3_SendPackage
* ����˵�� : USART3���ݰ����ͺ���
* ��    �� : data����Ҫ���͵����ݣ�len�����͵����ݵĳ���
* �� �� ֵ : ��
* ��    ע : ���ݰ��м���ܻ������\0��, ������Ҫ�������Ƚ��з���
*********************************************************************************************************/ 
void Usart3_SendPackage(unsigned char *data,unsigned short len)	//�������ݰ����м������\0��,������Ҫ�������Ƚ��з���
{
	while(len--)
	{
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TC));
		USART_SendData(USART3, *data++);
	}
}
/*********************************************************************************************************
* �� �� �� : USART3_IRQHandler
* ����˵�� : USART3�жϷ�����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : �����ж�+�����ж�, ÿ���յ�һ���ֽڣ��ͻ�ִ��һ��
*********************************************************************************************************/ 
void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE))						//����ǽ����ж�
	{
		usart3.RxBuff[usart3.RecLen] = USART_ReceiveData(USART3);		//���յ������ݴ�����
		if (++usart3.RecLen > USART3_RECEIVE_DATA_LENGTH_MAX){
			usart3.RecLen = 0;
		}
	}
	else if (USART_GetITStatus(USART3, USART_IT_IDLE))	//����ǿ����ж�
	{
		usart3.RecFlag = true;							//���λ��1
		if(usart3.RxBuff[usart3.RecLen-1] == SYN_MODULE_FREE){	//�յ�ģ�鷵�ؿ�����ʱ
			if(report_plan.is_report_plan == true){		//��ǰ�в����ƻ�
				report_plan.is_syn_free = true;			//���ģ�����
			}
		}
		
		USART_ReceiveData(USART3);						//��ȡDR���൱�����SR��RXNE��IDLEλ
	}
}
/*********************************************************************************************************
* �� �� �� : Usart3_Init
* ����˵�� : ��ʼ��USART3
* ��    �� : bound��������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Usart3_Init(unsigned int bound)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,  ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);

	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);	//������ӳ�䣬PC10/PC11
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate 		= bound;								//������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��ʹ��Ӳ��������
	USART_InitStructure.USART_Mode 			= USART_Mode_Rx | USART_Mode_Tx;		//�շ�ģʽ
	USART_InitStructure.USART_Parity 		= USART_Parity_No;						//��ʹ����żУ��
	USART_InitStructure.USART_StopBits 		= USART_StopBits_1;						//һλֹͣλ
	USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;					//��λ�����ݳ���
	USART_Init(USART3, &USART_InitStructure);

	NVIC_InitStruct.NVIC_IRQChannel    = USART3_IRQn;		//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);			//ʹ�ܽ����ж�
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);			//ʹ�ܿ����ж�

	USART_Cmd(USART3, ENABLE);								//ʹ��USART3
}

