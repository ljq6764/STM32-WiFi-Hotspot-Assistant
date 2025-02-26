#ifndef __BEEP_H
#define __BEEP_H

#include "io_bit.h"

#define BEEP PAout(15)

void Beep_Init(void);
extern void beep(char beepid);

#endif
