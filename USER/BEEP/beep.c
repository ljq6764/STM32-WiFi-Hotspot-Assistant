#include "beep.h"
/*********************************************************************************************************
* 函 数 名 : Beep_Init
* 功能说明 : 初始化BEEP端口
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 蜂鸣器PA15（BEEP）
*********************************************************************************************************/ 
void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能PC外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//开启SWD，失能JTAG
	
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_15;				//初始化引脚
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_Out_PP;			//输出模式
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_2MHz;			//2MHZ
	GPIO_Init(GPIOA, &GPIO_InitStruct);						//根据GPIO_InitStruct初始化
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);						//输出低电平，蜂鸣器不响
}


void beep(char beepid)
{
	if(beepid==1)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_15);
	}
	if(beepid==2)
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_15);
	}
}
