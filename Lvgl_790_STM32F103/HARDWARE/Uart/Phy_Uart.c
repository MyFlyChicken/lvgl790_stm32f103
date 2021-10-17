/*
    ���������������ж��շ����ݡ�
    �ж����ȼ�Ҫ����FreeRTOS�������������ȼ����Ա�֤���ݽ��յļ�ʱ�ԡ�
*/
#include <string.h>

#include "Phy_Uart.h"



const uint32_t culUsartBaudRate[USART_BAUDRATE_NUM]={
    4800,   9600,   19200,  38400,  57600,  115200
};

//����������֡�����ж�ʱ������3.5���ֽ�ʱ�䣬��λ0.2ms
const uint8_t cucFrameEndTime[USART_BAUDRATE_NUM]={
    42,     21,     11,     6,      4,      2
};

#ifdef USART1_USED
UART_HandleTypeDef huart1;
USART_CTRL xUsart1Ctrl;
#endif

#ifdef USART2_USED
UART_HandleTypeDef huart2;
USART_CTRL xUsart2Ctrl;
#endif

#ifdef USART3_USED
UART_HandleTypeDef huart3;
USART_CTRL xUsart3Ctrl;
#endif

TIM_HandleTypeDef htim2;


static void Phy_UsartCtrlInit(USART_CTRL * pxUsartCtrl, USART_BAUD_SET baudRateSet, UART_HandleTypeDef * huart);
static USART_BAUD_SET Phy_UsartNextBaud(USART_BAUD_SET currBaudRate);
static HAL_StatusTypeDef Phy_UsartGetIdleRxBuf(USART_CTRL * pxUsartCtrl);
static HAL_StatusTypeDef Phy_UART_Transmit_IT(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef Phy_UART_EndTransmit_IT(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef Phy_UART_Receive_IT(USART_CTRL * pxUsartCtrl);
static void Phy_UART_EndRxTransfer(UART_HandleTypeDef *huart);
static void Phy_UsartFrameEndDeal(USART_CTRL * pxUsartCtrl);

#if WWDG_EN
    static unsigned short usFeedDog;
#endif

/*******************************************************************************
* ��������     : ��ʼ������
* �β�     ��  usartBase������ָ������
            baudRateSet��ָ��������
            format��ָ�����ݸ�ʽ
* ����	   : BiGuangMing	
* ʱ��       : 09/10/2019
* ˵��	   : 
********************************************************************************/
void Phy_UsartInit(USART_TypeDef * usartBase, USART_BAUD_SET baudRateSet, USART_FORMAT_SET format)
{
    UART_HandleTypeDef * huart;
    USART_CTRL * pxUsartCtrl;
    if(USART1==usartBase)
    {
#ifdef USART1_USED    
        huart = &huart1;
        huart->Instance = USART1;
        pxUsartCtrl = &xUsart1Ctrl; 
#else
        return;
#endif  
    }
    else if(USART2==usartBase)
    {
#ifdef USART2_USED    
        huart = &huart2;
        huart->Instance = USART2;
        pxUsartCtrl = &xUsart2Ctrl;
#else
        return;
#endif
    }
    else if(USART3==usartBase)
    {
#ifdef USART3_USED    
        huart = &huart3;
        huart->Instance = USART3;
        pxUsartCtrl = &xUsart3Ctrl;
#else
        return;
#endif  
    }else{
        return;
    }    
    HAL_UART_DeInit(huart);
    
    huart->Init.BaudRate = culUsartBaudRate[baudRateSet];
        
    if(USART_8O1==format)
    {
        huart->Init.WordLength = UART_WORDLENGTH_9B;
        huart->Init.StopBits = UART_STOPBITS_1;
        huart->Init.Parity = UART_PARITY_ODD;
    }
    else if(USART_8E1==format)
    {
        huart->Init.WordLength = UART_WORDLENGTH_9B;
        huart->Init.StopBits = UART_STOPBITS_1;
        huart->Init.Parity = UART_PARITY_EVEN;
    }else{
        huart->Init.WordLength = UART_WORDLENGTH_8B;
        huart->Init.StopBits = UART_STOPBITS_2;
        huart->Init.Parity = UART_PARITY_NONE;
    }
    
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(huart);

    Phy_UsartCtrlInit(pxUsartCtrl, baudRateSet, huart);
}

/*******************************************************************************
* ��������     : ��ʼ�������շ����ƽṹ��
* �β�     ��  pxUsartCtrl���ṹ��ָ��
            baudRateSet��ָ��������
* ����	   : BiGuangMing	
* ʱ��       : 09/10/2019
* ˵��	   : 
********************************************************************************/
static void Phy_UsartCtrlInit(USART_CTRL * pxUsartCtrl, USART_BAUD_SET baudRateSet, UART_HandleTypeDef * huart)
{
    uint8_t i;
    pxUsartCtrl->eUsartBaudSet = baudRateSet;

	if(pxUsartCtrl->huart != huart  )
	{
		pxUsartCtrl->plock = xSemaphoreCreateBinary();
	}
	
    pxUsartCtrl->huart = huart;
	xSemaphoreGive(pxUsartCtrl->plock);
    for(i=0; i<USART_RX_BUF_NUM; i++)
    {
        pxUsartCtrl->xRxBuf[i].ucBusyFlag = 0;
    }
    pxUsartCtrl->ulTxTime = 0;
}

/*******************************************************************************
* ��������     : �����һ��������
* �β�     ��  currBaudRate����ǰ������
* ����	   : BiGuangMing	
* ʱ��       : 09/10/2019
* ˵��	   : 
********************************************************************************/
static USART_BAUD_SET Phy_UsartNextBaud(USART_BAUD_SET currBaudRate)
{
    uint8_t temp;
    temp = (uint8_t)currBaudRate + 1;
    if(temp>=USART_BAUDRATE_NUM)
    {
        temp = 0;
    }
    return (USART_BAUD_SET)temp;
}

/*******************************************************************************
* ��������     : ��һ�����������³�ʼ������
* �β�     ��  usartBase������ָ������
            format��ָ�����ݸ�ʽ
* ����	   : BiGuangMing	
* ʱ��       : 09/10/2019
* ˵��	   : �Զ�������һ��������
********************************************************************************/
void Phy_UsartTryNewBaudRate(USART_TypeDef * usartBase, USART_FORMAT_SET format)
{
    USART_BAUD_SET currBaudRate;
    if(USART1==usartBase)
    {
#ifdef USART1_USED    
        currBaudRate = xUsart1Ctrl.eUsartBaudSet;
#else
        return;
#endif  
    }
    else if(USART2==usartBase)
    {
#ifdef USART2_USED    
        currBaudRate = xUsart2Ctrl.eUsartBaudSet;
#else
        return;
#endif
    }
    else if(USART3==usartBase)
    {
#ifdef USART3_USED    
        currBaudRate = xUsart3Ctrl.eUsartBaudSet;
#else
        return;
#endif
    }else{
        return;
    }          
    Phy_UsartInit(usartBase, Phy_UsartNextBaud(currBaudRate), format);
}

/*******************************************************************************
* ��������     : �����жϷ���
* �β�     ��  pxUsartCtrl����������ѡ��
            pData������ָ��
            size��Ҫ���͵���������
* ����ֵ�� HAL status   
* ����	   : BiGuangMing	
* ʱ��       : 09/10/2019
* ˵��	   : 
********************************************************************************/
HAL_StatusTypeDef Phy_UART_Start_Transmit_IT(USART_CTRL * pxUsartCtrl, const uint8_t *pData, uint8_t Size)
{
    UART_HandleTypeDef *huart;
    uint32_t time;
    if(pxUsartCtrl == NULL)
        return HAL_ERROR;
    huart = pxUsartCtrl->huart;
    /* Check that a Tx process is not already ongoing */
    if(huart->gState == HAL_UART_STATE_READY)
    {
        if((pData == NULL) || (Size == 0U)) 
        {
            return HAL_ERROR;
        }

        ( void ) memcpy( ( void * )pxUsartCtrl->xTxBuf.ucData, (const void * )pData, ( size_t ) Size ); 
        pxUsartCtrl->xTxBuf.ucLen = Size;
        pxUsartCtrl->ucTxFrameEndTimer = 0;
        //���㱾֡�����ķ���ʱ��
        time = (uint32_t)Size * 10 /35 + 2;
        time *= cucFrameEndTime[pxUsartCtrl->eUsartBaudSet];
        time /= 5;
        pxUsartCtrl->ulTxTime = time + 1;//����������������1ms
            
        huart->pTxBuffPtr = pxUsartCtrl->xTxBuf.ucData;
        huart->TxXferSize = Size;
        huart->TxXferCount = Size;

        huart->ErrorCode = HAL_UART_ERROR_NONE;
        huart->gState = HAL_UART_STATE_BUSY_TX;

        /* Enable the UART Transmit data register empty Interrupt */
        __HAL_UART_ENABLE_IT(huart, UART_IT_TXE);

        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

/*******************************************************************************
* ��������     : ���������жϷ���֮ǰ���͵�����
* �β�     ��  pxUsartCtrl����������ѡ��
* ����ֵ�� HAL status   
* ����	   : BiGuangMing	
* ʱ��       : 09/10/2019
* ˵��	   : 
********************************************************************************/
HAL_StatusTypeDef Phy_UART_ReStart_Transmit_IT(USART_CTRL * pxUsartCtrl)
{
    UART_HandleTypeDef *huart;
    if(pxUsartCtrl == NULL)
        return HAL_ERROR;    
    huart = pxUsartCtrl->huart;
    /* Check that a Tx process is not already ongoing */
    if(huart->gState == HAL_UART_STATE_READY)
    {
        if(pxUsartCtrl->xTxBuf.ucLen == 0U) 
        {
            return HAL_ERROR;
        }
        /* Process Locked */
        __HAL_LOCK(huart);
        pxUsartCtrl->ucTxFrameEndTimer = 0;        
        huart->pTxBuffPtr = pxUsartCtrl->xTxBuf.ucData;
        huart->TxXferSize = pxUsartCtrl->xTxBuf.ucLen;
        huart->TxXferCount = pxUsartCtrl->xTxBuf.ucLen;
        huart->ErrorCode = HAL_UART_ERROR_NONE;
        huart->gState = HAL_UART_STATE_BUSY_TX;
        /* Process Unlocked */
        __HAL_UNLOCK(huart);
        /* Enable the UART Transmit data register empty Interrupt */
        __HAL_UART_ENABLE_IT(huart, UART_IT_TXE);
        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}


/*******************************************************************************
* ��������     : ���������ж�
* �β�     ��  pxUsartCtrl����������ѡ��
* ����ֵ�� HAL status   
* ����	   : BiGuangMing	
* ʱ��       : 09/10/2019
* ˵��	   : �����������busy�����������Դ�����ʱ�ᶪʧ�µ����ݣ�
*            �Ѿ����յ����ݲ��ᱻ���ǡ�
*            �˺��������жϺͷ��ж��е��ã����뱣֤ԭ�Ӳ���
********************************************************************************/
HAL_StatusTypeDef Phy_UART_Start_Receive_IT(USART_CTRL * pxUsartCtrl)
{
    UART_HandleTypeDef *huart;
    HAL_StatusTypeDef res;
    if(pxUsartCtrl == NULL)
        return HAL_ERROR;  
    huart = pxUsartCtrl->huart;
    /* Process Locked ��������ʼ,��֤ԭ�Ӳ���*/
    __HAL_LOCK(huart);
    /* Check that a Rx process is not already ongoing */
    if(huart->RxState == HAL_UART_STATE_READY)
    {
        if(Phy_UsartGetIdleRxBuf(pxUsartCtrl)==HAL_OK)
        {
            pxUsartCtrl->ucRxFrameEndTimer = 0;
            
            huart->pRxBuffPtr = pxUsartCtrl->pxRxBuf->ucData;
            //huart->RxXferSize = Size;
            huart->RxXferCount = 0;

            huart->ErrorCode = HAL_UART_ERROR_NONE;
            huart->RxState = HAL_UART_STATE_BUSY_RX;

            /* Enable the UART Parity Error Interrupt */
            __HAL_UART_ENABLE_IT(huart, UART_IT_PE);

            /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
            __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);

            /* Enable the UART Data Register not empty Interrupt */
            __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);

            res =  HAL_OK;
        }else{
            res =  HAL_BUSY;
        }        
    }else{
        res = HAL_BUSY;
    }
    /* Process Unlocked */
    __HAL_UNLOCK(huart);
    return res;
}

/*******************************************************************************
* ��������     : ��ȡ���еĽ��ջ�����
* �β�     ��  pxUsartCtrl����������ѡ��
* ����ֵ�� HAL status   
* ����	   : BiGuangMing	
* ʱ��       : 09/10/2019
* ˵��	   : ����HAL_OKʱ��pxUsartCtrl->pxRxBuf�����ҵ��Ľ��
********************************************************************************/
static HAL_StatusTypeDef Phy_UsartGetIdleRxBuf(USART_CTRL * pxUsartCtrl)
{
    uint8_t i;
    for(i=0; i<USART_RX_BUF_NUM; i++)
    {
        if(0==pxUsartCtrl->xRxBuf[i].ucBusyFlag)
        {
            pxUsartCtrl->pxRxBuf = &(pxUsartCtrl->xRxBuf[i]);
            return HAL_OK;
        }
    }
    return HAL_BUSY;
}



/**
  * @brief  This function handles UART interrupt request.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void Phy_UART_IRQHandler(USART_CTRL * pxUsartCtrl)
{
    UART_HandleTypeDef *huart;
    uint32_t isrflags;
    uint32_t cr1its;
    uint32_t cr3its;
    uint32_t errorflags = 0x00U;

    huart = pxUsartCtrl->huart;
    isrflags   = READ_REG(huart->Instance->SR);
    cr1its     = READ_REG(huart->Instance->CR1);
    cr3its     = READ_REG(huart->Instance->CR3);
    /* If no error occurs */
    errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
    if(errorflags == RESET)
    {
        /* UART in mode Receiver -------------------------------------------------*/
        if(((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
        {
            Phy_UART_Receive_IT(pxUsartCtrl);
        }
    }
    /* If some errors occur */
    else if((errorflags != RESET) 
      && (((cr3its & USART_CR3_EIE) != RESET) || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)))
    {
        /* UART parity error interrupt occurred ----------------------------------*/
        if(((isrflags & USART_SR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
        {
            huart->ErrorCode |= HAL_UART_ERROR_PE;
        }

        /* UART noise error interrupt occurred -----------------------------------*/
        if(((isrflags & USART_SR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
        {
            huart->ErrorCode |= HAL_UART_ERROR_NE;
        }

        /* UART frame error interrupt occurred -----------------------------------*/
        if(((isrflags & USART_SR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
        {
            huart->ErrorCode |= HAL_UART_ERROR_FE;
        }

        /* UART Over-Run interrupt occurred --------------------------------------*/
        if(((isrflags & USART_SR_ORE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
        { 
            huart->ErrorCode |= HAL_UART_ERROR_ORE;
        }

        /* Call UART Error Call back function if need be --------------------------*/
        if(huart->ErrorCode != HAL_UART_ERROR_NONE)
        {
            /* UART in mode Receiver -----------------------------------------------*/
            if(((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
            {
                Phy_UART_Receive_IT(pxUsartCtrl);
            }

            /* If Overrun error occurs, or if any error occurs in DMA mode reception,
            consider error as blocking */
            if(((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET))
            {
                /* Blocking error : transfer is aborted
                Set the UART state ready to be able to start again the process,
                Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
                Phy_UART_EndRxTransfer(huart);
                huart->RxState = HAL_UART_STATE_ERROR;
                /* Call user error callback */
                //HAL_UART_ErrorCallback(huart);               
            }
            else
            {
                /* Non Blocking error : transfer could go on. 
                Error is notified to user through user error callback */
                //HAL_UART_ErrorCallback(huart);
                huart->ErrorCode = HAL_UART_ERROR_NONE;
            }
        }
    } /* End if some error occurs */

    /* UART in mode Transmitter ------------------------------------------------*/
    if(((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
    {
        Phy_UART_Transmit_IT(huart);
        return;
    }

    /* UART in mode Transmitter end --------------------------------------------*/
    if(((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
    {
        Phy_UART_EndTransmit_IT(huart);
        return;
    }
}


/**
  * @brief  Sends an amount of data in non blocking mode.
  * @param  huart: Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef Phy_UART_Transmit_IT(UART_HandleTypeDef *huart)
{
    /* Check that a Tx process is ongoing */
    if(huart->gState == HAL_UART_STATE_BUSY_TX)
    {
        huart->Instance->DR = (uint8_t)(*huart->pTxBuffPtr++ & (uint8_t)0x00FF);

        if(--huart->TxXferCount == 0U)
        {
            /* Disable the UART Transmit Complete Interrupt */
            __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

            /* Enable the UART Transmit Complete Interrupt */    
            __HAL_UART_ENABLE_IT(huart, UART_IT_TC);
        }
        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

/**
  * @brief  Wraps up transmission in non blocking mode.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef Phy_UART_EndTransmit_IT(UART_HandleTypeDef *huart)
{
    /* Disable the UART Transmit Complete Interrupt */    
    __HAL_UART_DISABLE_IT(huart, UART_IT_TC);
    huart->TxXferSize = 0;
    return HAL_OK;
}

/**
  * @brief  Receives an amount of data in non blocking mode 
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef Phy_UART_Receive_IT(USART_CTRL * pxUsartCtrl)
{
    UART_HandleTypeDef *huart;
    
    huart = pxUsartCtrl->huart;
    
    /* Check that a Rx process is ongoing */
    if(huart->RxState == HAL_UART_STATE_BUSY_RX) 
    {
        if(huart->RxXferCount>=FRAME_MAX_LEN)   //�������������������ʱ����
            return HAL_ERROR;
        pxUsartCtrl->ucRxFrameEndTimer = 0;     //�������ݾ������ʱ��
        huart->RxXferCount++;
        *huart->pRxBuffPtr++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
        return HAL_OK;
    }
    else
    {
        return HAL_BUSY;
    }
}

/**
  * @brief  End ongoing Rx transfer on UART peripheral (following error detection or Reception completion).
  * @param  huart: UART handle.
  * @retval None
  */
static void Phy_UART_EndRxTransfer(UART_HandleTypeDef *huart)
{
  /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* At end of Rx process, restore huart->RxState to Ready */
  huart->RxState = HAL_UART_STATE_READY;
}


/*******************************************************************************
* ��������     : ����֡��������
* ����	   : BiGuangMing	
* ʱ��       : 09/10/2019
* ˵��	   : 
********************************************************************************/
static void Phy_UsartFrameEndDeal(USART_CTRL * pxUsartCtrl)
{
    UART_HandleTypeDef *huart;
    BaseType_t xHigherPriorityTaskWoken;
    huart = pxUsartCtrl->huart;
    if(huart->RxState==HAL_UART_STATE_BUSY_RX)
    {
        if(huart->RxXferCount>0)
        {
            pxUsartCtrl->ucRxFrameEndTimer++;
            if(pxUsartCtrl->ucRxFrameEndTimer>cucFrameEndTime[pxUsartCtrl->eUsartBaudSet])//
            {   
                Phy_UART_EndRxTransfer(huart);      //�ؽ����ж�             
                pxUsartCtrl->pxRxBuf->ucLen = huart->RxXferCount;
                pxUsartCtrl->pxRxBuf->ucBusyFlag = 1;

                //����Ϣ���з�����Ϣ����Ϣ�����ǻ�������ָ��
                if(pxUsartCtrl->huart==&huart1)
                {

                }

                huart->RxState = HAL_UART_STATE_READY;//��λ����״̬   
                Phy_UART_Start_Receive_IT(pxUsartCtrl);
            }
        }
    }
    else if((huart->RxState==HAL_UART_STATE_ERROR)||(huart->RxState==HAL_UART_STATE_READY))   //���ճ�����ʱһ��֡β��ʱ������¿�������|| (huart->RxState==HAL_UART_STATE_READY)
    {
        pxUsartCtrl->ucRxFrameEndTimer++;
        if(pxUsartCtrl->ucRxFrameEndTimer>cucFrameEndTime[pxUsartCtrl->eUsartBaudSet])
        {            
            huart->RxState = HAL_UART_STATE_READY;
            Phy_UART_Start_Receive_IT(pxUsartCtrl);
        }
    }
    

    if(huart->gState==HAL_UART_STATE_BUSY_TX)
    {
        if(huart->TxXferSize==0)   //���ݷ������
        {            
            if(pxUsartCtrl->ucTxFrameEndTimer>cucFrameEndTime[pxUsartCtrl->eUsartBaudSet])
            {
                huart->gState = HAL_UART_STATE_READY;
                //����֪ͨ
                xHigherPriorityTaskWoken = pdFALSE;
                if(pxUsartCtrl->huart==&huart1)
                {
                    vTaskNotifyGiveFromISR(CommEUTHandle, &xHigherPriorityTaskWoken);
                }                
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }else{  //��ӣ�����ڶ��һ��
                pxUsartCtrl->ucTxFrameEndTimer++;
            }            
        }        
    }
}

/*******************************************************************************
* ��������     : TIMER2��ʼ��
* ����	   : BiGuangMing	
* ʱ��       : 09/10/2019
* ˵��	   : ����Ϊ0.2ms�ж�ģʽ��FreeRTOSϵͳ������ȼ�
********************************************************************************/
void Phy_TIM2_Init(void)
{
    uint32_t    uwTimclock = 0;
    uint32_t    uwPrescalerValue = 0;
#if WWDG_EN
    usFeedDog = 0;
#endif
    if(htim2.Instance != TIM2)
    {
        htim2.Instance = TIM2;
        htim2.State = HAL_TIM_STATE_RESET;
    }
    
    if(htim2.State!=HAL_TIM_STATE_READY)
    {
        /* Compute TIM2 clock */
        uwTimclock = HAL_RCC_GetPCLK1Freq()*2;     
        /* Compute the prescaler value to have TIM2 counter clock equal to 1MHz */
        uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000) - 1);

        /* Initialize TIMx peripheral as follow:
        + Period = [(TIM1CLK/200) - 1]. to have a 200 us time base.
        + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
        + ClockDivision = 0
        + Counter direction = Up
        */
        htim2.Init.Period = (1000000 / 5000) - 1;
        htim2.Init.Prescaler = uwPrescalerValue;        
        
        htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
        if (HAL_TIM_Base_Init(&htim2) == HAL_OK)
        {
            HAL_TIM_Base_Start_IT(&htim2);
            htim2.State = HAL_TIM_STATE_READY;
        }       
    }
}

/*******************************************************************************
* ��������     : TIMER2��ʱ�жϴ�����
* ����	   : BiGuangMing	
* ʱ��       : 09/10/2019
* ˵��	   : 
********************************************************************************/
void Phy_TIM2_IRQHandler(void)
{
    /* TIM Update event */
    if(__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_UPDATE) != RESET)
    {
        if(__HAL_TIM_GET_IT_SOURCE(&htim2, TIM_IT_UPDATE) !=RESET)
        {
            __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
#ifdef USART1_USED            
            Phy_UsartFrameEndDeal(&xUsart1Ctrl);
#endif
#ifdef USART2_USED
            Phy_UsartFrameEndDeal(&xUsart2Ctrl);
#endif
#ifdef USART3_USED
            Phy_UsartFrameEndDeal(&xUsart3Ctrl);
#endif
        }
    }
#if WWDG_EN     
    usFeedDog++;
    if(usFeedDog>10)
    {
        usFeedDog = 0;
        WWDG_Refresh(CNT_TIME);
    }
#endif 
}

/*******************************************************************************
* ��������     : �ͷŴ�����Դ
* ����	   : BiGuangMing	
* ʱ��       : 02/14/2020
* ˵��	   : 
********************************************************************************/
void Phy_UART_Release(USART_RX_BUF * pRxBuffer, void * pUnlock)     
{
    if(NULL!=pRxBuffer)
    {
        pRxBuffer->ucBusyFlag = 0;
    }
    xSemaphoreGive(pUnlock);
}


    //if(pdTRUE != (xSemaphoreTake(pxUsartCtrl->plock, portMAX_DELAY)))  //take�����ź�����һֱ����


