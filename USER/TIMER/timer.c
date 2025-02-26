#include "timer.h"

/*********************************************************************************************************
* �� �� �� : Timer1_PwmInit
* ����˵�� : TIM1 CCR1��ʼ��
* ��    �� : arr����װ�أ�psc��Ԥ��Ƶ
* �� �� ֵ : ��
* ��    ע : Fpwm��HZ�� = Fclk / ����arr+1��*��psc+1����
			 Duty_ratio��%�� = CRR1 / ARR
*********************************************************************************************************/ 
void Timer1_PwmInit(unsigned short arr, unsigned short psc)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};
	TIM_OCInitTypeDef  TIM_OCInitStructure = {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);  
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1, ENABLE);
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	TIM_TimeBaseStructure.TIM_Period = arr - 1; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  
	TIM_CtrlPWMOutputs(TIM1, ENABLE);							
	TIM_Cmd(TIM1, ENABLE); 
}

/*********************************************************************************************************
* �� �� �� : Timer6_Init
* ����˵�� : TIM6��ʼ��
* ��    �� : arr����װ�أ�psc��Ԥ��Ƶ
* �� �� ֵ : ��
* ��    ע : ���ʱ�� = (arr+1)*(psc+1) / clk
*********************************************************************************************************/ 
void Timer6_Init(unsigned short arr, unsigned short psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct 	= {0};
	NVIC_InitTypeDef 		NVIC_InitStruct			= {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);		//ʹ��TIM6����ʱ��
	
	TIM_TimeBaseInitStruct.TIM_Period 			 = arr - 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler 		 = psc - 1 ;
	TIM_TimeBaseInitStruct.TIM_CounterMode 		 = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_ClockDivision 	 = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
	
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel    = TIM6_IRQn;			//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//��Ӧ���ȼ�2
	NVIC_Init(&NVIC_InitStruct);	
	
	TIM_Cmd(TIM6, DISABLE);
}

/*********************************************************************************************************
* �� �� �� : Timer7_Init
* ����˵�� : TIM7��ʼ��
* ��    �� : arr����װ�أ�psc��Ԥ��Ƶ
* �� �� ֵ : ��
* ��    ע : ���ʱ�� = (arr+1)*(psc+1) / clk
*********************************************************************************************************/ 
void Timer7_Init(unsigned short arr, unsigned short psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct 	= {0};
	NVIC_InitTypeDef 		NVIC_InitStruct			= {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);		//ʹ��TIM6����ʱ��
	
	TIM_TimeBaseInitStruct.TIM_Period 			 = arr - 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler 		 = psc - 1 ;
	TIM_TimeBaseInitStruct.TIM_CounterMode 		 = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_ClockDivision 	 = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);
	
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel    = TIM7_IRQn;			//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ���ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 2;	//��Ӧ���ȼ�2
	NVIC_Init(&NVIC_InitStruct);	
	
	TIM_Cmd(TIM7, ENABLE);
}
void TIM6_IRQHandler(void)
{
	if(TIM6->SR & 0x01){
		
		TIM6->SR &= 0xfe;
	}
}
void TIM7_IRQHandler(void)
{
	if(TIM7->SR & 0x01){
		
		TIM7->SR &= 0xfe;
	}
}

