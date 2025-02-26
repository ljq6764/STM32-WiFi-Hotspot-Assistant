#include "key.h"
#include "led.h"
#include "systick.h"
#include "beep.h"
#include "ButtonEvent.h"
#include "syn6658.h"

static __pdev pdev[EN_KEY_ID_MAX]; 

static bool ButtonEnable = true;	//��ť�����Կ��أ�Ĭ�ϴ�

/**
  * @brief  ��ť�¼��ص�����
  * @param  pDev����ť����ָ��
  * @param  event���¼�����
  * @retval ��ť��״̬
  */
static void Button_PushHandler(void *pDev, int event)
{
	switch(((__pdev *)(pDev))->buttonId){
		case EN_KEY_ID_KEY1:
			switch(event){
				case EVENT_SHORT_CLICKED:
					LED1 = !LED1;
					break;
				case EVENT_LONG_PRESSED:
					BEEP = !BEEP;
					break;
				case EVENT_DOUBLE_CLICKED:
					
					break;
			}
			break;
		case EN_KEY_ID_KEY2:
			switch(event){
				case EVENT_SHORT_CLICKED:
					LED2 = !LED2;
					break;
				case EVENT_LONG_PRESSED:
					Syn6658_SetReportPlan(0);
					break;
				case EVENT_DOUBLE_CLICKED:
					
					break;
			}
			break;
		case EN_KEY_ID_KEY3:
			switch(event){
				case EVENT_SHORT_CLICKED:
					LED3 = !LED3;
					break;
				case EVENT_LONG_PRESSED:
					Syn6658_SetReportPlan(1);
					break;
				case EVENT_DOUBLE_CLICKED:
					
					break;
			}
			break;
		default:  printf("There is no button with this id!\r\n");	break;
	}
}
/**
  * @brief  ��ʼ����ť����ButtonEvent���ʵ����
  * @param  ��
  * @retval ��
  */
void Button_Init()
{
	unsigned char i;
	
	memset(pdev, 0, sizeof(pdev));
	
	for(i=0; i<EN_KEY_ID_MAX; i++) {
		pdev[i].longPressTimeCfg = 1000;		
		pdev[i].longPressRepeatTimeCfg = 500;	
		pdev[i].doubleClickTimeCfg = 300;		
		pdev[i].clickInterval = 400;		
		
		pdev[i].buttonId = i;
		pdev[i].lastLongPressTime = 0;
		pdev[i].lastClickTime = 0;
		pdev[i].lastPressTime = 0;
		pdev[i].isLongPressed = false;
		pdev[i].nowState = STATE_NO_PRESS;

		EventAttach(&pdev[i], Button_PushHandler);
	}
}
/**
  * @brief  ��ⰴť�Ƿ���
  * @param  id�������İ�ť���
  * @retval ��ť��״̬
  */
bool Button_GetIsPush(KeyId_t id)
{
	bool state = false;
	
	switch((int)id){
		case EN_KEY_ID_KEY1:
			state = !!KEY1;
			break;
		case EN_KEY_ID_KEY2:
			state = !KEY2;
			break;
		case EN_KEY_ID_KEY3:
			state = !KEY3;
			break;
		default: printf("There is no button with this id!\r\n");	break;
	}
	
    return state;
}
/**
  * @brief  ���ð�ť�Ŀ�����
  * @param  en��
  * @retval ��
  */
void Button_SetEnable(bool en)
{
    ButtonEnable = en;
}
/**
  * @brief  ���°�ť״̬��ɨ����Ϊ10ms
  * @param  ��
  * @retval ��
  */
void TaskKeyMsg_Handle()
{
	unsigned char i;
	
	if(!ButtonEnable) {
        return;
    }
	
	for(i=0; i<EN_KEY_ID_MAX; i++) {
		EventMonitor(&pdev[i], Button_GetIsPush((KeyId_t)i));
	}
}
/**
  * @brief  �������˿ڳ�ʼ�������Ŷ��壺PA0��KEY1����PA1��KEY2����PC12��KEY3��
  * @param  ��
  * @retval ��
  */
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;	//��ʼ������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//��������ģʽ
	GPIO_Init(GPIOA, &GPIO_InitStruct);						//����GPIO_InitStruct��ʼ��
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//��������ģʽ
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	Button_Init();
}
