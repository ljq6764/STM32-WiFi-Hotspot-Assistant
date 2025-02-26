#include "key.h"
#include "led.h"
#include "systick.h"
#include "beep.h"
#include "ButtonEvent.h"
#include "syn6658.h"

static __pdev pdev[EN_KEY_ID_MAX]; 

static bool ButtonEnable = true;	//按钮可用性开关，默认打开

/**
  * @brief  按钮事件回调函数
  * @param  pDev：按钮对象指针
  * @param  event：事件类型
  * @retval 按钮的状态
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
  * @brief  初始化按钮，对ButtonEvent类的实例化
  * @param  无
  * @retval 无
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
  * @brief  检测按钮是否按下
  * @param  id：待检测的按钮编号
  * @retval 按钮的状态
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
  * @brief  设置按钮的可用性
  * @param  en：
  * @retval 无
  */
void Button_SetEnable(bool en)
{
    ButtonEnable = en;
}
/**
  * @brief  更新按钮状态，扫描间隔为10ms
  * @param  无
  * @retval 无
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
  * @brief  物理按键端口初始化，引脚定义：PA0（KEY1）、PA1（KEY2）、PC12（KEY3）
  * @param  无
  * @retval 无
  */
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;	//初始化引脚
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//下拉输入模式
	GPIO_Init(GPIOA, &GPIO_InitStruct);						//根据GPIO_InitStruct初始化
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//上拉输入模式
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	Button_Init();
}
