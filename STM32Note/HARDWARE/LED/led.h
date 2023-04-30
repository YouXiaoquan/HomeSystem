#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED0 PCout(13)
#define led3 PBout(12)
#define led1 PBout(13)
#define led2 PBout(14)
#define led4 PBout(15)
#define lock PBout(8)
#define beep PBout(9)
void Command_GPIO_Init(void);//≥ı ºªØ

#endif
