#include "beep.h"
/*********************************************************************************************************
* �� �� �� : Beep_Init
* ����˵�� : ��ʼ��BEEP�˿�
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ������PA15��BEEP��
*********************************************************************************************************/ 
void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��PC����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//����SWD��ʧ��JTAG
	
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_15;				//��ʼ������
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_Out_PP;			//���ģʽ
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_2MHz;			//2MHZ
	GPIO_Init(GPIOA, &GPIO_InitStruct);						//����GPIO_InitStruct��ʼ��
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);						//����͵�ƽ������������
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
