#include "ButtonEvent.h"
#include "key.h"

unsigned int systick;

//��Ҫ�ṩms��������
#define GET_TICK() systick

#ifndef GET_TICK
#error "Please define the GET_TICK() function to get the system time "
#endif

/**
  * @brief  ��ȡ���ϴ�ʱ���ʱ���(��uint32���ʶ��)
  * @param  prevTick: �ϵ�ʱ���
  * @retval ʱ���
  */
unsigned int GetTickElaps(unsigned int prevTick)
{
    unsigned int actTime = GET_TICK();

    if(actTime >= prevTick) {
        prevTick = actTime - prevTick;
    }
    else {
        prevTick = UINT32_MAX - prevTick + 1;
        prevTick += actTime;
    }

    return prevTick;
}
/**
  * @brief  �����¼���
  * @param  function: �ص�����ָ��
  * @retval ��
  */
void EventAttach(__pdev *pDev, FuncCallback_t function)
{
	pDev->eventCallback = function;
}

/**
  * @brief  ����¼�������ɨ������10ms
  * @param  nowState: ��ǰ����״̬
  * @retval ��
  */
void EventMonitor(__pdev *pDev, bool isPress)
{
    if(pDev->eventCallback == NULL){
        return;
    }

    if (isPress && pDev->nowState == STATE_NO_PRESS) {	
        pDev->nowState = STATE_PRESS;					
        pDev->lastPressTime = GET_TICK();			
    }
	
	if(pDev->isClick) {	
		if(GetTickElaps(pDev->lastPressTime) >= pDev->clickInterval) {
			pDev->isClick = false;	
			pDev->eventCallback(pDev, EVENT_SHORT_CLICKED);	
		}
	}

    if(pDev->nowState == STATE_NO_PRESS) {				
        return;
    }

    if (isPress && GetTickElaps(pDev->lastPressTime) >= pDev->longPressTimeCfg) {
        pDev->nowState = STATE_LONG_PRESS;				
        if(!pDev->isLongPressed) {						
            pDev->eventCallback(pDev, EVENT_LONG_PRESSED);		
            pDev->lastLongPressTime = GET_TICK();				
            pDev->isLongPressed = true;							
        } else if(GetTickElaps(pDev->lastLongPressTime) >= pDev->longPressRepeatTimeCfg) {
            pDev->lastLongPressTime = GET_TICK();					
        }
    } else if (!isPress) {					
        pDev->nowState = STATE_NO_PRESS;	
        if(GetTickElaps(pDev->lastClickTime) <= pDev->doubleClickTimeCfg) {
            pDev->eventCallback(pDev, EVENT_DOUBLE_CLICKED);	
			pDev->isClick = false;								
        }else if(GetTickElaps(pDev->lastPressTime) <= pDev->longPressTimeCfg) {
			pDev->isClick = true;	
		}

        pDev->isLongPressed = false;		//ȡ���������
        pDev->lastClickTime = GET_TICK();	//���������ʱ��
    }
}



