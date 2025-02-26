#ifndef __KEY_H
#define __KEY_H

#include "io_bit.h" 

#define KEY1 PAin(0)
#define KEY2 PAin(1)
#define KEY3 PCin(12)

typedef enum {
    EN_KEY_ID_KEY1 = 0,
    EN_KEY_ID_KEY2,
    EN_KEY_ID_KEY3,
    EN_KEY_ID_MAX, 
}KeyId_t;

void Key_Init(void);
void button_init(void);
bool Button_GetIsPush(KeyId_t id);
void TaskKeyMsg_Handle(void);

#endif
