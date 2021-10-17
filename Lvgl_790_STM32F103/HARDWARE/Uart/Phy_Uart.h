#ifndef _Phy_UART_
#define _Phy_UART_

#include "stm32f1xx_hal.h"
#include "main.h"

#define USART1_USED 
//#define USART2_USED 
//#define USART3_USED

#define USART2_TX_Pin GPIO_PIN_2
#define USART2_TX_GPIO_Port GPIOA
#define USART2_RX_Pin GPIO_PIN_3
#define USART2_RX_GPIO_Port GPIOA
#define USART3_TX_Pin GPIO_PIN_10
#define USART3_TX_GPIO_Port GPIOB
#define USART3_RX_Pin GPIO_PIN_11
#define USART3_RX_GPIO_Port GPIOB
#define USART1_TX_Pin GPIO_PIN_9
#define USART1_TX_GPIO_Port GPIOA
#define USART1_RX_Pin GPIO_PIN_10
#define USART1_RX_GPIO_Port GPIOA


#define USART_BAUDRATE_NUM          6
typedef enum _USART_BAUD_
{
    USART_BAUD_4800 = 0,
    USART_BAUD_9600,
    USART_BAUD_19200,
    USART_BAUD_38400,
    USART_BAUD_57600,
    USART_BAUD_115200
}USART_BAUD_SET;

typedef enum _USART_FORMAT_
{
    USART_8N2 = 0,          //8数据位 2停止位
    USART_8E1,              //8数据位，1位偶校验，1停止位
    USART_8O1               //8数据位，1位奇校验，1停止位    
}USART_FORMAT_SET;

#define FRAME_MAX_LEN       128     //最大帧长

//接收缓冲区定义
typedef struct _USART_RX_BUF_
{
    uint8_t ucData[FRAME_MAX_LEN];
    uint8_t ucLen;                  //接收到的数据数量
    uint8_t ucBusyFlag;             //0本缓冲区无数据，1本缓冲区数据待处理
}USART_RX_BUF;
//发送缓冲区定义
typedef struct _USART_TX_BUF_
{
    uint8_t ucData[FRAME_MAX_LEN];
    uint8_t ucLen;                  //要发送的数据数量
}USART_TX_BUF;

#define USART_RX_BUF_NUM               2    //2个交替缓存，一个在正在处理时另一个可以实时接收
//收发控制结构体
typedef struct _USART_CTRL_
{
    UART_HandleTypeDef * huart;
    USART_BAUD_SET eUsartBaudSet;   //记录设置的波特率
    uint8_t ucRxFrameEndTimer;      //接收帧结束判断计时
    uint8_t ucTxFrameEndTimer;      //发送帧结束判断计时
    USART_RX_BUF * pxRxBuf;         //当前接收缓冲区
    USART_RX_BUF xRxBuf[USART_RX_BUF_NUM];
    USART_TX_BUF xTxBuf;
    uint32_t ulTxTime;              //本次发送需要的时间，单位ms
	osSemaphoreId plock;
}USART_CTRL;


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern TIM_HandleTypeDef htim2;

extern USART_CTRL xUsart1Ctrl;
extern USART_CTRL xUsart2Ctrl;
extern USART_CTRL xUsart3Ctrl;


void Phy_UsartInit(USART_TypeDef * usartBase, USART_BAUD_SET baudRateSet, USART_FORMAT_SET format);
void Phy_UsartTryNewBaudRate(USART_TypeDef * usartBase, USART_FORMAT_SET format);
HAL_StatusTypeDef Phy_UART_Start_Transmit_IT(USART_CTRL * pxUsartCtrl, const uint8_t *pData, uint8_t Size);
HAL_StatusTypeDef Phy_UART_ReStart_Transmit_IT(USART_CTRL * pxUsartCtrl);
HAL_StatusTypeDef Phy_UART_Start_Receive_IT(USART_CTRL * pxUsartCtrl);
void Phy_UART_IRQHandler(USART_CTRL * pxUsartCtrl);
void Phy_TIM2_Init(void);
void Phy_TIM2_IRQHandler(void);
void Phy_UART_Release(USART_RX_BUF * pRxBuffer, void * pUnlock);

#ifdef USART1_USED
#define PHY_USART1_START_TRANSMIT_IT(pcdata, cSize)     Phy_UART_Start_Transmit_IT(&xUsart1Ctrl, (const uint8_t *)(pcdata), (cSize))
#define PHY_USART1_RESTART_TRANSMIT_IT()                Phy_UART_ReStart_Transmit_IT(&xUsart1Ctrl)
#define PHY_USART1_START_RECEIVE_IT()                   Phy_UART_Start_Receive_IT(&xUsart1Ctrl)
#define PHY_USART1_IRQHANDLER()                         Phy_UART_IRQHandler(&xUsart1Ctrl)
#define PHY_USART1_LOCK()                               xSemaphoreTake(xUsart1Ctrl.plock, portMAX_DELAY)
#else
#define PHY_USART1_START_TRANSMIT_IT(pcdata, cSize)      
#define PHY_USART1_RESTART_TRANSMIT_IT()            
#define PHY_USART1_START_RECEIVE_IT()   
#define PHY_USART1_IRQHANDLER()        
#endif

#ifdef USART2_USED
#define PHY_USART2_START_TRANSMIT_IT(pcdata, cSize)     Phy_UART_Start_Transmit_IT(&xUsart2Ctrl, (const uint8_t *)(pcdata), (cSize))
#define PHY_USART2_RESTART_TRANSMIT_IT()                Phy_UART_ReStart_Transmit_IT(&xUsart2Ctrl)
#define PHY_USART2_START_RECEIVE_IT()                   Phy_UART_Start_Receive_IT(&xUsart2Ctrl)
#define PHY_USART2_IRQHANDLER()                         Phy_UART_IRQHandler(&xUsart2Ctrl)
#define PHY_USART2_LOCK()                               xSemaphoreTake(xUsart2Ctrl.plock, portMAX_DELAY)
#else
#define PHY_USART2_START_TRANSMIT_IT(pcdata, cSize)    
#define PHY_USART2_RESTART_TRANSMIT_IT()          
#define PHY_USART2_START_RECEIVE_IT()   
#define PHY_USART2_IRQHANDLER()        
#endif

#ifdef USART3_USED
#define PHY_USART3_START_TRANSMIT_IT(pcdata, cSize)     Phy_UART_Start_Transmit_IT(&xUsart3Ctrl, (const uint8_t *)(pcdata), (cSize))
#define PHY_USART3_RESTART_TRANSMIT_IT()                Phy_UART_ReStart_Transmit_IT(&xUsart3Ctrl)
#define PHY_USART3_START_RECEIVE_IT()                   Phy_UART_Start_Receive_IT(&xUsart3Ctrl)
#define PHY_USART3_IRQHANDLER()                         Phy_UART_IRQHandler(&xUsart3Ctrl)
#define PHY_USART3_LOCK()                               xSemaphoreTake(xUsart3Ctrl.plock, portMAX_DELAY)
#else
#define PHY_USART3_START_TRANSMIT_IT(pcdata, cSize) 
#define PHY_USART3_RESTART_TRANSMIT_IT()        
#define PHY_USART3_START_RECEIVE_IT()  
#define PHY_USART3_IRQHANDLER()       
#endif

#endif


