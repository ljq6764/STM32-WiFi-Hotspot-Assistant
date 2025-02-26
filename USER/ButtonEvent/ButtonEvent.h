#ifndef __ButtonEvent_H
#define __ButtonEvent_H

#include "io_bit.h"

typedef enum
{
#define EVENT_DEF(evt) evt
#include "EventType.inc"
#undef EVENT_DEF
	_EVENT_LAST
}Event_t;

typedef enum {
	STATE_NO_PRESS,
	STATE_PRESS,
	STATE_LONG_PRESS
}State_t;

typedef enum {LOW = 0, HIGH = !LOW} GPIO_State_Type;

typedef void (*FuncCallback_t)(void *pDev, int event);

typedef struct {
	bool isLongPressed;
	bool isClick;
	State_t nowState;
	
	unsigned char  buttonId;
	unsigned short clickInterval;
	unsigned short longPressTimeCfg;
	unsigned short longPressRepeatTimeCfg;
	unsigned short doubleClickTimeCfg;
	unsigned int   lastLongPressTime;
	unsigned int   lastPressTime;
	unsigned int   lastClickTime;
	
	FuncCallback_t eventCallback; 
}__pdev;

void EventAttach(__pdev *pDev, FuncCallback_t eventCallback);
void EventMonitor(__pdev *pDev, bool isPress);


#endif
