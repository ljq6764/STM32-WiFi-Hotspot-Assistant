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
* 函 数 名 : Usart3_SendString
* 功能说明 : USART3发送字符串函数
* 形    参 : str：需要发送的字符串
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Usart3_SendString(unsigned char *data)		//发送数据，末尾才有‘\0’
{
	while(*data != '\0')
	{
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TC));
		USART_SendData(USART3, *data++);
	}
}
/*********************************************************************************************************
* 函 数 名 : Usart3_SendPackage
* 功能说明 : USART3数据包发送函数
* 形    参 : data：需要发送的数据，len：发送的数据的长度
* 返 回 值 : 无
* 备    注 : 数据包中间可能会包含‘\0’, 所以需要依赖长度进行发送
*********************************************************************************************************/ 
void Usart3_SendPackage(unsigned char *data,unsigned short len)	//发送数据包，中间包含‘\0’,所以需要依赖长度进行发送
{
	while(len--)
	{
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TC));
		USART_SendData(USART3, *data++);
	}
}
/*********************************************************************************************************
* 函 数 名 : USART3_IRQHandler
* 功能说明 : USART3中断服务函数
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 接收中断+空闲中断, 每接收到一个字节，就会执行一次
*********************************************************************************************************/ 
void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE))						//如果是接收中断
	{
		usart3.RxBuff[usart3.RecLen] = USART_ReceiveData(USART3);		//把收到的数据存起来
		if (++usart3.RecLen > USART3_RECEIVE_DATA_LENGTH_MAX){
			usart3.RecLen = 0;
		}
	}
	else if (USART_GetITStatus(USART3, USART_IT_IDLE))	//如果是空闲中断
	{
		usart3.RecFlag = true;							//最高位置1
		if(usart3.RxBuff[usart3.RecLen-1] == SYN_MODULE_FREE){	//收到模块返回空闲码时
			if(report_plan.is_report_plan == true){		//当前有播报计划
				report_plan.is_syn_free = true;			//标记模块空闲
			}
		}
		
		USART_ReceiveData(USART3);						//读取DR，相当于清空SR的RXNE和IDLE位
	}
}
/*********************************************************************************************************
* 函 数 名 : Usart3_Init
* 功能说明 : 初始化USART3
* 形    参 : bound：波特率
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Usart3_Init(unsigned int bound)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,  ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);

	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);	//部分重映射，PC10/PC11
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate 		= bound;								//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//不使用硬件控制流
	USART_InitStructure.USART_Mode 			= USART_Mode_Rx | USART_Mode_Tx;		//收发模式
	USART_InitStructure.USART_Parity 		= USART_Parity_No;						//不使用奇偶校验
	USART_InitStructure.USART_StopBits 		= USART_StopBits_1;						//一位停止位
	USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;					//八位的数据长度
	USART_Init(USART3, &USART_InitStructure);

	NVIC_InitStruct.NVIC_IRQChannel    = USART3_IRQn;		//中断源
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//使能中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//响应优先级
	NVIC_Init(&NVIC_InitStruct);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);			//使能接收中断
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);			//使能空闲中断

	USART_Cmd(USART3, ENABLE);								//使能USART3
}

