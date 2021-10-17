/***********************************************************************
*   文件功能：LCD的IC底层驱动，更换屏幕的话，更改此文件内容即可
***********************************************************************/

#include"LCD_DRV.h"
#include "stm32f1xx_ll_dma.h"

//ILI9341初始化程序
/*void TFTLCD_Init()
{
    LCD_WR_REG(0xCF);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xC1);
	LCD_WR_DATA(0X30);
	LCD_WR_REG(0xED);
	LCD_WR_DATA(0x64);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0X12);
	LCD_WR_DATA(0X81);
	LCD_WR_REG(0xE8);
	LCD_WR_DATA(0x85);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x7A);
    LCD_WR_REG(0xCB);
    LCD_WR_DATA(0x39);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x34);
	LCD_WR_DATA(0x02);
	LCD_WR_REG(0xF7);
	LCD_WR_DATA(0x20);
	LCD_WR_REG(0xEA);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xC0);    //Power control
	LCD_WR_DATA(0x1B);   //VRH[5:0]
	LCD_WR_REG(0xC1);    //Power control
	LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0]
	LCD_WR_REG(0xC5);    //VCM control
	LCD_WR_DATA(0x30); 	 //3F
	LCD_WR_DATA(0x30); 	 //3C
	LCD_WR_REG(0xC7);    //VCM control2
	LCD_WR_DATA(0XB7);
	LCD_WR_REG(0x36);    // Memory Access Control
	LCD_WR_DATA(0x48);
	LCD_WR_REG(0x3A);
	LCD_WR_DATA(0x55);
	LCD_WR_REG(0xB1);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x1A);
	LCD_WR_REG(0xB6);    // Display Function Control
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0xA2);
    LCD_WR_REG(0xF2);    // 3Gamma Function Disable
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0x26);    //Gamma curve selected
	LCD_WR_DATA(0x01);
	LCD_WR_REG(0xE0);    //Set Gamma
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x2A);
	LCD_WR_DATA(0x28);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x0E);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x54);
	LCD_WR_DATA(0XA9);
	LCD_WR_DATA(0x43);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0XE1);    //Set Gamma
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x06);
	LCD_WR_DATA(0x2B);
	LCD_WR_DATA(0x56);
	LCD_WR_DATA(0x3C);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x3F);
	LCD_WR_DATA(0x3F);
	LCD_WR_DATA(0x0F);
	LCD_WR_REG(0x2B);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3f);
	LCD_WR_REG(0x2A);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xef);
	LCD_WR_REG(0x11); //Exit Sleep
	HAL_Delay(120);
	LCD_WR_REG(0x29); //display on

}*/

void LCDPara_Init()
{
    LCDPara.setxcmd=0X2a;
    LCDPara.setycmd=0X2b;
    LCDPara.wramcmd=0X2C;
    LCDPara.height=240;
    LCDPara.width=320;
    LCD_WR_REG(0x36);
    LCD_WR_DATA(0xC8);//0x08会颠倒
}

//写命令
void LCD_WR_REG(unsigned short reg)
{
    LCD_REG_ADDR=reg;
}
//写数据
void LCD_WR_DATA(unsigned short dat)
{
    LCD_RAM_ADDR=dat;
}

//读一个数据
unsigned short LCD_RD_data()
{
    return LCD_RAM_ADDR;
}

//显示
void LCDDisplay_ON()
{
    LCD_REG_ADDR=0x29;
}
//不显示
void LCDDisplay_OFF()
{
    LCD_REG_ADDR=0x28;
}

//设置坐标
void LCD_SetCursor(int xCoord,int yCoord)
{
    LCD_WR_REG(LCDPara.setxcmd);
    LCD_WR_DATA(xCoord>>8);LCD_WR_DATA(xCoord&0XFF);
	LCD_WR_REG(LCDPara.setycmd);
	LCD_WR_DATA(yCoord>>8);LCD_WR_DATA(yCoord&0XFF);
}

//读取颜色信息
unsigned int LCD_ReadPoint(unsigned short x,unsigned short y)
{
    unsigned r,g,b;
    if(x>LCDPara.width||y>LCDPara.height)
    {
        return 0;
    }
    LCD_SetCursor(x, y);
    LCD_WR_REG(0X2E);
    r=LCD_RD_data();    r=LCD_RD_data();    b=LCD_RD_data();
    g=r&0xff;
    g<<=8;
    return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));
}

//画点
void LCD_Fast_DrawPoint(unsigned short x,unsigned short y,unsigned short color)
{
    LCD_WR_REG(LCDPara.setxcmd);
	LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF);
	LCD_WR_REG(LCDPara.setycmd);
	LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF);

    LCD_REG_ADDR=LCDPara.wramcmd;
	LCD_RAM_ADDR=color;
}

//画面
void LCD_Fill(int xs,int ys,int xe,int ye,unsigned short color)
{
    int xlen;
    int j;
    xlen=xe-xs+1;
    for(j=ys;j<=ye;j++)
    {
        LCD_SetCursor(xs, j);
        LCD_WR_REG(LCDPara.wramcmd);
        for(int i=0;i<xlen;i++)
        {
            LCD_RAM_ADDR=color;
        }
    }
}

void LCD_DMA_FILL(void     *mbuf,unsigned long bufsize)
{
    LL_DMA_InitTypeDef DMA_InitStruct;

    __HAL_RCC_DMA2_CLK_ENABLE();                        //使能DMA2时钟

    LL_DMA_DeInit(DMA2, LL_DMA_CHANNEL_2);              //将DMA2的通道2寄存器重设为缺省值

    LL_DMA_DisableChannel(DMA2, LL_DMA_CHANNEL_2);      //关闭DMA2 通道2
    while((LL_DMA_IsActiveFlag_TC2(DMA2))||(LL_DMA_IsActiveFlag_TE2(DMA2))||(LL_DMA_IsActiveFlag_GI2(DMA2))||(LL_DMA_IsActiveFlag_HT2(DMA2)));

    DMA_InitStruct.PeriphOrM2MSrcAddress  = (unsigned long)mbuf;           //DMA外设基地址
    DMA_InitStruct.MemoryOrM2MDstAddress  = LCD_MEMORY_ADDR;               //DMA内存基地址
    DMA_InitStruct.Direction              = LL_DMA_DIRECTION_MEMORY_TO_MEMORY;//存储器到存储器
    DMA_InitStruct.Mode                   = LL_DMA_MODE_NORMAL;             //工作在正常缓存模式
    DMA_InitStruct.PeriphOrM2MSrcIncMode  = LL_DMA_PERIPH_INCREMENT;        //数据源地址自增
    DMA_InitStruct.MemoryOrM2MDstIncMode  = LL_DMA_MEMORY_NOINCREMENT;      //数据目标地址不变
    DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;     //数据源宽度为16位
    DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;     //数据目标宽度为16位
    DMA_InitStruct.NbData                 = bufsize;                        //DMA通道的DMA缓存的大小
    DMA_InitStruct.Priority               = LL_DMA_PRIORITY_MEDIUM;         //DMA通道 x拥有高优先级
    LL_DMA_Init(DMA2, LL_DMA_CHANNEL_2, &DMA_InitStruct);
    LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_2);                           //开启DMA2 通道2
    while(!LL_DMA_IsActiveFlag_TC2(DMA2));//传输完成退出while 假设不会出现中断
    LL_DMA_ClearFlag_GI2(DMA2);
}


//HX8368初始化程序
void TFTLCD_Init()
{
	int i;
	LCD_WR_REG(0xB9);   // Set EXTC
	LCD_WR_DATA(0xFF);
	LCD_WR_DATA(0x83);
	LCD_WR_DATA(0x68);
	HAL_Delay(20);

	LCD_WR_REG(0xBB);   // Set OTP
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x80);   //OTP load disable
	HAL_Delay(20);

	LCD_WR_REG(0xC2);  // For Himax internel use
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x30);
	HAL_Delay(20);

	LCD_WR_REG(0xC0);  // For Himax internel use
	LCD_WR_DATA(0x1B);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0xEC);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	HAL_Delay(20);

	LCD_WR_REG(0xE3);   // For Himax internel use
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x4F);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x4F);
	HAL_Delay(20);

	LCD_WR_REG(0xBD);  // For Himax internel use
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x20);
	LCD_WR_DATA(0x52);
	HAL_Delay(20);

	LCD_WR_REG(0xBF);  // For Himax internel use
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x04);
	LCD_WR_DATA(0x2C);
	HAL_Delay(20);

	LCD_WR_REG(0xB1); // Set Power 调节电压 降低功耗
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x02);//0x02
	LCD_WR_DATA(0x02);//0x1E
	LCD_WR_DATA(0x00);//0x00
	LCD_WR_DATA(0x22);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x8D);
	HAL_Delay(20);

	LCD_WR_REG(0xB6);    // Set VCOM
	LCD_WR_DATA(0xc0);   // VMF 95
	LCD_WR_DATA(0x55);   // VMH 64
	LCD_WR_DATA(0x42);   // VML 42
	HAL_Delay(20);

	LCD_WR_REG(0xE0);   // Set Gamma
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x24);
	LCD_WR_DATA(0x23);
	LCD_WR_DATA(0x21);
	LCD_WR_DATA(0x21);
	LCD_WR_DATA(0x3F);
	LCD_WR_DATA(0x16);
	LCD_WR_DATA(0x5D);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x06);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x10);
	LCD_WR_DATA(0x17);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x1E);
	LCD_WR_DATA(0x1E);
	LCD_WR_DATA(0x1C);
	LCD_WR_DATA(0x1B);
	LCD_WR_DATA(0x2C);
	LCD_WR_DATA(0x22);
	LCD_WR_DATA(0x69);
	LCD_WR_DATA(0x07);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x16);
	LCD_WR_DATA(0x19);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0xFF);
	HAL_Delay(20);

	LCD_WR_REG(0xB0);   // Set OSC
	LCD_WR_DATA(0x0D);
	LCD_WR_DATA(0x01);
	HAL_Delay(20);

	LCD_WR_REG(0x36);   // Set BGR
	LCD_WR_DATA(0x08);

	LCD_WR_REG(0x3a);
	LCD_WR_DATA(0x55);

	LCD_WR_REG(0x2D);   // Look up table for 65K color
	for(i=0;i<32;i++)
	{
	if(i<16)
	LCD_WR_DATA(2*i);  // RED
	else
	LCD_WR_DATA(2*i+1);
	}
	for(i=0;i<64;i++)
	LCD_WR_DATA(1*i);   // Green
	for(i=0;i<32;i++)
	{
	if(i<16)
	LCD_WR_DATA(2*i);  // Blue
	else
	LCD_WR_DATA(2*i+1);
	}

	LCD_WR_REG(0x21);

	LCD_WR_REG(0x11);    //Sleep Out
	HAL_Delay(20);

	LCD_WR_REG(0x29);    //Display On
	HAL_Delay(20);


    LCD_WR_REG(0x51);
    LCD_WR_DATA(0xff);
    LCD_WR_REG(0x52);
	i = LCD_RD_data();
	i = LCD_RD_data();
	if(0xff == i)
	{
		while(1);

	}
}





