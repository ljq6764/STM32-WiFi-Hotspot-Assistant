#include "usart2.h"
#include "usart1.h"
#include "timer.h"
#include "control.h"
#include "esp12.h"

USART2_InitStructure usart2;
/*********************************************************************************************************
* �� �� �� : Usart2_SendString
* ����˵�� : USART2�����ַ�������
* ��    �� : data����Ҫ���͵��ַ���
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Usart2_SendString(char *data)		//�����ַ�����ĩβ���С�\0��
{
	while(*data != '\0')
	{
		while(!USART_GetFlagStatus(WIFI_USART, USART_FLAG_TC));
		USART_SendData(WIFI_USART, *data++);
	}
}
/*********************************************************************************************************
* �� �� �� : Usart2_SendPackage
* ����˵�� : USART2���ݰ����ͺ���
* ��    �� : data����Ҫ���͵����ݣ�len�����͵����ݵĳ���
* �� �� ֵ : ��
* ��    ע : ���ݰ��м���ܻ������\0��, ������Ҫ�������Ƚ��з���
*********************************************************************************************************/ 
void Usart2_SendPackage(unsigned char *data, unsigned short len)	//�������ݰ����м������\0��,������Ҫ�������Ƚ��з���
{
	while(len--)
	{
		while(!USART_GetFlagStatus(WIFI_USART, USART_FLAG_TC));
		USART_SendData(WIFI_USART, *data++);
	}
}
/*********************************************************************************************************
* �� �� �� : USART2_IRQHandler
* ����˵�� : USART2�жϷ�����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : �����ж�+�����ж�, ÿ���յ�һ���ֽڣ��ͻ�ִ��һ��
*********************************************************************************************************/ 
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(WIFI_USART, USART_IT_RXNE))							//����ǽ����ж�
	{
		if(network_flag.r.networkIsRequest == true){							//���������������
			if(usart2.cacheRxBuff == NULL){
				printf("null\r\n");
				return;
			}
			usart2.cacheRxBuff[usart2.cacheLen] = USART_ReceiveData(WIFI_USART);
			if(++usart2.cacheLen >= USART2_NETWORK_BUFF_MAX){
				usart2.cacheLen = 0;
			}
		}else{																//ƽʱ״̬	
			usart2.RxBuff[usart2.RecLen] = USART_ReceiveData(WIFI_USART);	//���յ������ݴ�����
			if (++usart2.RecLen >= USART2_RX_BUFF_MAX){
				usart2.RecLen = 0;
			}
		}
	}else if(USART_GetITStatus(WIFI_USART, USART_IT_IDLE)){
		if(network_flag.r.networkIsRequest == true){		//������������
			if(usart2.cacheRxBuff == NULL){
				printf("null-\r\n");
				return;
			}
			usart2.cacheRxBuff[usart2.cacheLen] = '\0';
			usart2.RecFlag = true;							//��ǽ������
			USART_ReceiveData(WIFI_USART);
		}else{
			usart2.RxBuff[usart2.RecLen] = '\0';			//����ĩβ��ӽ�����
			usart2.RecFlag = true;							//��ǽ������
			USART_ReceiveData(WIFI_USART);
			if(Esp12_GetNetWorkState() == N_DISCONNECT){	//δ����ʱ���ڻ���
				printf("%s", usart2.RxBuff);
				usart2.RecLen = 0;
			}
		}
	}
}
/*********************************************************************************************************
* �� �� �� : Usart2_Init
* ����˵�� : ��ʼ��USART2
* ��    �� : bound��������
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Usart2_Init(unsigned int bound)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,  ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate 		= bound;								//������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��ʹ��Ӳ��������
	USART_InitStructure.USART_Mode 			= USART_Mode_Rx | USART_Mode_Tx;		//�շ�ģʽ
	USART_InitStructure.USART_Parity 		= USART_Parity_No;						//��ʹ����żУ��
	USART_InitStructure.USART_StopBits 		= USART_StopBits_1;						//һλֹͣλ
	USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;					//��λ�����ݳ���
	USART_Init(WIFI_USART, &USART_InitStructure);

	NVIC_InitStruct.NVIC_IRQChannel    = USART2_IRQn;		//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStruct);

	USART_ITConfig(WIFI_USART, USART_IT_RXNE, ENABLE);		//ʹ�ܽ����ж�
	USART_ITConfig(WIFI_USART, USART_IT_IDLE, ENABLE);		//ʹ�ܿ����ж�

	USART_Cmd(WIFI_USART, ENABLE);							//ʹ��USART1
}

