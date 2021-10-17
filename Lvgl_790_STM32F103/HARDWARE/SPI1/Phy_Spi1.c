/***********************************************************************
*   文件功能：字库芯片驱动
***********************************************************************/
#include "Phy_Spi1.h"



void Phy_Spi1Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef Init;

    /* Peripheral clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : PA4 -------->SPI1_NSS*/
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);            
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    Init.Mode = SPI_MODE_MASTER;
    Init.Direction = SPI_DIRECTION_2LINES;
    Init.DataSize = SPI_DATASIZE_8BIT;
    Init.CLKPolarity = SPI_POLARITY_HIGH;//high
    Init.CLKPhase = SPI_PHASE_2EDGE;//2
    Init.NSS = SPI_NSS_SOFT;
    Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    Init.FirstBit = SPI_FIRSTBIT_MSB;
    Init.TIMode = SPI_TIMODE_DISABLE;
    Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    Init.CRCPolynomial = 10;
  
    /* Disble the selected SPI peripheral */
    CLEAR_BIT(SPI1->CR1, SPI_CR1_SPE);
    /*----------------------- SPIx CR1 & CR2 Configuration ---------------------*/
    /* Configure : SPI Mode, Communication Mode, Data size, Clock polarity and phase, NSS management,
    Communication speed, First bit and CRC calculation state */
    WRITE_REG(SPI1->CR1, (Init.Mode | Init.Direction | Init.DataSize |
                                  Init.CLKPolarity | Init.CLKPhase | (Init.NSS & SPI_CR1_SSM) |
                                  Init.BaudRatePrescaler | Init.FirstBit  | Init.CRCCalculation) );

    /* Configure : NSS management */
    WRITE_REG(SPI1->CR2, (((Init.NSS >> 16U) & SPI_CR2_SSOE) | Init.TIMode));

    /*---------------------------- SPIx CRCPOLY Configuration ------------------*/
    /* Configure : CRC Polynomial */
    WRITE_REG(SPI1->CRCPR, Init.CRCPolynomial);

    /* Activate the SPI mode (Make sure that I2SMOD bit in I2SCFGR register is reset) */
    CLEAR_BIT(SPI1->I2SCFGR, SPI_I2SCFGR_I2SMOD);

    /* Enble the selected SPI peripheral */
    SET_BIT(SPI1->CR1, SPI_CR1_SPE);
}


void Phy_Spi1ReadBuffer(unsigned char * buffer, unsigned long addr, unsigned short byteNum)
{
    unsigned short i;
    if(buffer==NULL)
        return;
    
    NSS_L;
#if 1
    while(READ_BIT(SPI1->SR, SPI_FLAG_TXE)==0);   //等待，直到发送为空
    *(__IO uint8_t *)&SPI1->DR = READ_CMD;
    while(READ_BIT(SPI1->SR, SPI_FLAG_TXE)==0);   //等待，直到发送为空
    *(__IO uint8_t *)&SPI1->DR = ((addr>>16)&0xff);
    while(READ_BIT(SPI1->SR, SPI_FLAG_TXE)==0);   //等待，直到发送为空
    *(__IO uint8_t *)&SPI1->DR = ((addr>>8)&0xff);
    while(READ_BIT(SPI1->SR, SPI_FLAG_TXE)==0);   //等待，直到发送为空
    *(__IO uint8_t *)&SPI1->DR = (addr&0xff);
    
    while(READ_BIT(SPI1->SR, SPI_FLAG_RXNE)==0);  //(*(uint8_t *)buffer) = SPI1->DR;
    (*(uint8_t *)buffer) = SPI1->DR;
    while(READ_BIT(SPI1->SR, SPI_FLAG_RXNE))
    {
    	(*(uint8_t *)buffer) = SPI1->DR;
    }
    for(i=0; i<byteNum; i++)
    {
        while(READ_BIT(SPI1->SR, SPI_FLAG_TXE)==0);   //等待，直到发送为空
        *(__IO uint8_t *)&SPI1->DR = 0;
        while(READ_BIT(SPI1->SR, SPI_FLAG_RXNE)==0);  //等待，直到发送为空
        (*(uint8_t *)buffer) = SPI1->DR;
        buffer++;
    }
#else
    


#endif
    NSS_H;
}

