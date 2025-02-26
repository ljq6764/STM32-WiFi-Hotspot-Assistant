#ifndef __LED_H
#define __LED_H

#include "io_bit.h"

#define LED1 PAout(11)
#define LED2 PAout(12)
#define LED3 PDout(2)

void Led_Init(void);
void TaskledMsg_Handle(void);

#endif
