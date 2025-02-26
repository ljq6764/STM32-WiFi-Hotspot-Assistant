#include "usart2.h"
#include "usart1.h"
#include "timer.h"
#include "control.h"
#include "esp12.h"

USART2_InitStructure usart2;
/*********************************************************************************************************
* 函 数 名 : Usart2_SendString
* 功能说明 : USART2发送字符串函数
* 形    参 : data：需要发送的字符串
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Usart2_SendString(char *data)		//发送字符串，末尾才有‘\0’
{
	while(*data != '\0')
	{
		while(!USART_GetFlagStatus(WIFI_USART, USART_FLAG_TC));
		USART_SendData(WIFI_USART, *data++);
	}
}
/*********************************************************************************************************
* 函 数 名 : Usart2_SendPackage
* 功能说明 : USART2数据包发送函数
* 形    参 : data：需要发送的数据，len：发送的数据的长度
* 返 回 值 : 无
* 备    注 : 数据包中间可能会包含‘\0’, 所以需要依赖长度进行发送
*********************************************************************************************************/ 
void Usart2_SendPackage(unsigned char *data, unsigned short len)	//发送数据包，中间包含‘\0’,所以需要依赖长度进行发送
{
	while(len--)
	{
		while(!USART_GetFlagStatus(WIFI_USART, USART_FLAG_TC));
		USART_SendData(WIFI_USART, *data++);
	}
}
/*********************************************************************************************************
* 函 数 名 : USART2_IRQHandler
* 功能说明 : USART2中断服务函数
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 接收中断+空闲中断, 每接收到一个字节，就会执行一次
*********************************************************************************************************/ 
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(WIFI_USART, USART_IT_RXNE))							//如果是接收中断
	{
		if(network_flag.r.networkIsRequest == true){							//如果正在请求网络
			if(usart2.cacheRxBuff == NULL){
				printf("null\r\n");
				return;
			}
			usart2.cacheRxBuff[usart2.cacheLen] = USART_ReceiveData(WIFI_USART);
			if(++usart2.cacheLen >= USART2_NETWORK_BUFF_MAX){
				usart2.cacheLen = 0;
			}
		}else{																//平时状态	
			usart2.RxBuff[usart2.RecLen] = USART_ReceiveData(WIFI_USART);	//把收到的数据存起来
			if (++usart2.RecLen >= USART2_RX_BUFF_MAX){
				usart2.RecLen = 0;
			}
		}
	}else if(USART_GetITStatus(WIFI_USART, USART_IT_IDLE)){
		if(network_flag.r.networkIsRequest == true){		//正在请求网络
			if(usart2.cacheRxBuff == NULL){
				printf("null-\r\n");
				return;
			}
			usart2.cacheRxBuff[usart2.cacheLen] = '\0';
			usart2.RecFlag = true;							//标记接收完成
			USART_ReceiveData(WIFI_USART);
		}else{
			usart2.RxBuff[usart2.RecLen] = '\0';			//数据末尾添加结束符
			usart2.RecFlag = true;							//标记接收完成
			USART_ReceiveData(WIFI_USART);
			if(Esp12_GetNetWorkState() == N_DISCONNECT){	//未连接时串口回显
				printf("%s", usart2.RxBuff);
				usart2.RecLen = 0;
			}
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : Usart2_Init
* 功能说明 : 初始化USART2
* 形    参 : bound：波特率
* 返 回 值 : 无
* 备    注 : 无
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

	USART_InitStructure.USART_BaudRate 		= bound;								//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//不使用硬件控制流
	USART_InitStructure.USART_Mode 			= USART_Mode_Rx | USART_Mode_Tx;		//收发模式
	USART_InitStructure.USART_Parity 		= USART_Parity_No;						//不使用奇偶校验
	USART_InitStructure.USART_StopBits 		= USART_StopBits_1;						//一位停止位
	USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;					//八位的数据长度
	USART_Init(WIFI_USART, &USART_InitStructure);

	NVIC_InitStruct.NVIC_IRQChannel    = USART2_IRQn;		//中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//响应优先级
	NVIC_Init(&NVIC_InitStruct);

	USART_ITConfig(WIFI_USART, USART_IT_RXNE, ENABLE);		//使能接收中断
	USART_ITConfig(WIFI_USART, USART_IT_IDLE, ENABLE);		//使能空闲中断

	USART_Cmd(WIFI_USART, ENABLE);							//使能USART1
}

