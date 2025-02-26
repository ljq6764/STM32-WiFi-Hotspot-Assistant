#include "led.h"

/*********************************************************************************************************
* �� �� �� : Led_Init
* ����˵�� : LED�˿ڳ�ʼ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : PA11��LED1����PA12��LED2����PD2��LED3��
*********************************************************************************************************/ 
void Led_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    GPIO_SetBits(GPIOD, GPIO_Pin_2);
	GPIO_SetBits(GPIOA, GPIO_Pin_11 | GPIO_Pin_12);
}

void TaskledMsg_Handle(void)
{
	
}
