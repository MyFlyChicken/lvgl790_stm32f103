/***********************************************************************
*   文件功能：按键扫描，并发送STemwin消息
***********************************************************************/

#include "KeyScan.h"
#include "stm32f1xx.h"


#define KEY_SENDBIT         0xFF
#define KEY0_DEFAULT        (KEY_SENDBIT&(~Send_ToLastWin))
#define KEY1_DEFAULT        (KEY_SENDBIT&(~Send_AddLine)&(~Send_Insert)&(~Send_NextPage)&(~Send_Tab))
//#define KEY2_DEFAULT        (KEY_SENDBIT&(~Send_WMOK))
#define KEY3_DEFAULT        (KEY_SENDBIT&(~Send_Tab)&(~Send_NextPage)&(~Send_AddLine)&(~Send_Insert))
#define KEY4_DEFAULT        (KEY_SENDBIT&(~Send_AddLine)&(~Send_Insert)&(~Send_NextPage)&(~Send_Tab))
#define KEY5_DEFAULT        (KEY_SENDBIT&(~Send_Tab)&(~Send_NextPage)&(~Send_AddLine)&(~Send_Insert))
#define KEY6_DEFAULT
#define KEY7_DEFAULT
#define KEY8_DEFAULT

unsigned char ucEditMoveFlag;//1 左键按下 0右键按下
unsigned short usControlInfo = 0;
unsigned char ucLRKeyDefault = KEY_DIR_EFFECT_DEFAULT;//非0 不能发送GUI_KEY_RIGHT与GUI_KEY_LEFT
unsigned char ucHallFlag = 1;//为1 发送 GUI_KEY_ENTER 为0 发送WM_OK
KEY_STATE xKeyState;


static KEY_STATE eLastKey = KEY_NONE;
static KEY_STATE eCurrKey = KEY_NONE;
static unsigned char ucFilter;
static unsigned char ucDelay;


SEND_FLAG eSendFlag;


static KEY_STATE KeyScan_KeyMonitor(void)
{
    KEY_STATE eKey;

    eKey = KEY_NONE;
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)==0)
    {   
        eKey |= KEY_STOP_RES;
    }
    if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0)==0)
    {
        eKey |= KEY_FUN;
    }
    if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1)==0)
    {
        eKey |= KEY_RIGHT;
    }
    if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2)==0)
    {
        eKey |= KEY_DOWN;
    }
    if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3)==0)
    {
        eKey |= KEY_ENTER;
    }
    if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4)==0)
    {
        eKey |= KEY_UP;
    }
    if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5)==0)
    {
        eKey |= KEY_LEFT;
    }
    if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6)==0)
    {
        eKey |= KEY_PRG;  
    }
    if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7)==0)
	{
		eKey |= KEY_RUN;
	}
    if(eKey==eLastKey)
    {
        ucFilter++;
        if(ucFilter>=5)
        {
            eCurrKey = eKey;
        }
    }
    else
    {
        ucFilter = 0;
    }
    eLastKey = eKey;
    return eCurrKey;
}

void KeyScan_KeyMessage(void)
{
    static KEY_STATE xLastKeyState;
    unsigned short usTemp;
    xKeyState=KeyScan_KeyMonitor();
    if(KEY_NONE==xLastKeyState)
    {
        switch(xKeyState)
        {
            case KEY_PRG:

                break;
            case KEY_UP:

                break;
            case KEY_ENTER:

                break;
            case KEY_LEFT:

                break;
            case KEY_DOWN:

                break;
            case KEY_RIGHT:

                break;
            case KEY_RUN:

            break;                
            default:  
                
                break;
        }
        ucDelay = 0;
    }
}


