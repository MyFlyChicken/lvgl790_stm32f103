#ifndef __DELAY_H
#define __DELAY_H 			   

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

void delay_init(void);
void delay_us(unsigned long nus);
void delay_ms(unsigned short nms);
void delay_xms(unsigned short nms);

#endif





























