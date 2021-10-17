#ifndef _Phy_SPI1_
#define _Phy_SPI1_

#include "stm32f1xx_hal.h"

#ifndef NULL
#define NULL 0
#endif


#define READ_CMD        0x03

#define NSS_H     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define NSS_L     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)


void Phy_Spi1Init(void);
void Phy_Spi1ReadBuffer(unsigned char * buffer, unsigned long addr, unsigned short byteNum);

#endif

