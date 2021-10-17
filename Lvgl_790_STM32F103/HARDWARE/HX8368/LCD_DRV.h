#ifndef _LCD_DRV_H
#define _LCD_DRV_H
#include "stm32f1xx_hal.h"


#define Operators 

typedef struct para{
    unsigned short width;
    unsigned short height;
    unsigned short wramcmd;
    unsigned short setxcmd;
    unsigned short setycmd;
    unsigned short dir;
}LCD_para;
LCD_para LCDPara;//LCD信息 像素及坐标轴



typedef struct LCD{
    volatile unsigned short REG;
    volatile unsigned short RAM;
}LCD_TYPEDEF;

#define BANK1   ((unsigned long )(0x60000000|0x0001FFFE))    //LCD 操作器

#define LCD_MEMORY_ADDR	(0x60020000 + 0)
#define LCD_REG_ADDR    *(volatile unsigned short *)(0x60000000)
#define LCD_RAM_ADDR    *(volatile unsigned short *)(0x60020000)

#define LcdDat  ((LCD_TYPEDEF*)BANK1)

//颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40
#define BRRED 			 0XFC07
#define GRAY  			 0X8430

#define BACK_Color  GREEN
#define POINT_Color RED

//坐标扫描方向
#define L2R_U2D  0
#define L2R_D2U  1
#define R2L_U2D  2
#define R2L_D2U  3
#define U2D_L2R  4
#define U2D_R2L  5
#define D2U_L2R  6
#define D2U_R2L  7

//默认扫描方向
#define Default_Scan L2R_U2D
#define high_num8   8
#define high_num16  16



//lcd IC初始化
void TFTLCD_Init();


extern void LCD_WR_DATA(unsigned short dat);
extern void LCD_WR_REG(unsigned short reg);
extern unsigned short LCD_RD_data();

extern void LCD_Fast_DrawPoint(unsigned short x,unsigned short y,unsigned short color);
extern void LCD_SetCursor(int xCoord,int yCoord);
extern unsigned int LCD_ReadPoint(unsigned short x,unsigned short y);
extern void LCD_Fill(int xs,int ys,int xe,int ye,unsigned short color);
void LCD_DMA_FILL(void     *mbuf,unsigned long bufsize);



extern void LCDPara_Init();







void LCDDisplay_ON();
void LCDDisplay_OFF();







#endif

