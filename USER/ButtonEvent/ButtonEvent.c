#include "ButtonEvent.h"
#include "key.h"

unsigned int systick;

//需要提供ms级的心跳
#define GET_TICK() systick

#ifndef GET_TICK
#error "Please define the GET_TICK() function to get the system time "
#endif

/**
  * @brief  获取与上次时间的时间差(带uint32溢出识别)
  * @param  prevTick: 上的时间戳
  * @retval 时间差
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
  * @brief  按键事件绑定
  * @param  function: 回调函数指针
  * @retval 无
  */
void EventAttach(__pdev *pDev, FuncCallback_t function)
{
	pDev->eventCallback = function;
}

/**
  * @brief  监控事件，建议扫描周期10ms
  * @param  nowState: 当前按键状态
  * @retval 无
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

        pDev->isLongPressed = false;		//取消长按标记
        pDev->lastClickTime = GET_TICK();	//更新最后点击时间
    }
}



