
#include "main.h"
#include "cmsis_os.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
//#include "Phy_Uart.h"
#include "sdio_sdcard.h"
#include "LCD_DRV.h"
#include "KeyScan.h"
#include <string.h>
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_examples.h"



/************************************/
/************************************/

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LIGHT_MAX_NUM   99
#define LIGHT_MIN_NUM   10

#define LED_ON() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
#define LED_OFF() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);



/* USER CODE END PD */



/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;
SRAM_HandleTypeDef hsram1;
WWDG_HandleTypeDef hwwdg;

osThreadId Task1Handle;
osThreadId CommEUTHandle;
osThreadId HmiDisplayHandle;
osThreadId TaskKeyHandle;

unsigned char ucLightNessVal;


#define SHAKEHANDS  1
#define RTCEN       1


//unsigned char ucFontBuf[1024] = {0};


/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_FSMC_Init(void);
static void MX_CRC_Init(void);
static void BackLightInit();



void Task1_Function(void const * argument);
void Hmi_Display(void const * argument);
void Task_Key(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

long length=0;



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

int main(void){
  /* USER CODE BEGIN 1 */
  //xHmiCiperHint.pcCiperFalse = CIPER_FALSE;
  //HMI_DatInit();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */

  MX_GPIO_Init();
  MX_CRC_Init();
  MX_FSMC_Init();
  /* USER CODE BEGIN 2 */
  delay_init();
  //Phy_RtcTest();

  //Phy_Spi1Init();
#if 0
  xSysTime.year = 2020;
xSysTime.month = 03;
xSysTime.day = 31;
xSysTime.week = 2;
xSysTime.hour = 15;
xSysTime.min = 50;
xSysTime.sec = 0;
  Phy_RtcSetTime(&xSysTime);
#endif
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  //vPortDefineHeapRegions();

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Task1 */

  /* definition and creation of CommEUT */
  xTaskCreate(	(TaskFunction_t) Hmi_Display,
				(const char *) "Hmi",
				(uint16_t) 1500,
				(void *) NULL,
				(UBaseType_t) osPriorityHigh,//osPriorityAboveNormal
				(TaskHandle_t *) HmiDisplayHandle) ;
  //osThreadDef(Hmi, Hmi_Display, osPriorityAboveNormal, 0, 1500);
  //HmiDisplayHandle = osThreadCreate(osThread(Hmi), NULL);

  /* definition and creation of CommEUT */
  xTaskCreate(	(TaskFunction_t) Task_Key,
				(const char *) "Key",
				(uint16_t) 256,
				(void *) NULL,
				(UBaseType_t) osPriorityRealtime,
				(TaskHandle_t *) TaskKeyHandle) ;
  //osThreadDef(Key, Task_Key, osPriorityHigh, 0, 256);  //TaskKeyHandle = osThreadCreate(osThread(Key), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */


  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();//�ᴴ��һ����Ϊ��IDLE���Ŀ��������������ȼ���ͣ� ��������֮����л�

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON; //������
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
#ifdef Operators
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_1;//����
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#else
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif
  GPIO_InitStruct.Pin = GPIO_PIN_9;         //����
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;//����
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* FSMC initialization function */
static void MX_FSMC_Init(void)
{
  FSMC_NORSRAM_TimingTypeDef Timing;
#if 0
  FSMC_NORSRAM_TimingTypeDef ExtTiming;
#endif
  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FSMC_NORSRAM_DEVICE;
  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
#ifdef Operators
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK1;
#else
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK4;
#endif
  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;//FSMC_WAIT_SIGNAL_POLARITY_LOW
  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  /* Timing д*/
  Timing.AddressSetupTime = 0;//0x00
  Timing.AddressHoldTime = 0;
  Timing.DataSetupTime = 1;//0x03
  Timing.BusTurnAroundDuration = 0;
  Timing.CLKDivision = 1;
  Timing.DataLatency = 2;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
#if 0
  ExtTiming.AddressSetupTime = 0;//0x00
  ExtTiming.AddressHoldTime = 0;
  ExtTiming.DataSetupTime = 1;//0x03
  ExtTiming.BusTurnAroundDuration = 0;
  ExtTiming.CLKDivision = 1;
  ExtTiming.DataLatency = 2;
  ExtTiming.AccessMode = FSMC_ACCESS_MODE_B;
#endif
  if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK)
  {
    Error_Handler( );
  }

  /** Disconnect NADV
  */

  __HAL_AFIO_FSMCNADV_DISCONNECTED();

}


/* USER CODE BEGIN 4 */

/*
* 1/FΪ��������
*F = 1/T = 1/72Mhz
*��ʱ������T = ��������*(ARR+1) = (ARR+1)/72Mhz
*��ʱ��Ƶ��Timerf = 72Mhz*(1/(Arr+1))
*�����ʱ���ٷ�Ƶ Ft = 72Mhz/(PSC+1)
*�ܽ�Timerf = 72Mhz/((PSC+1)(Arr+1))
*/
static void BackLightInit()
{
  TIM_HandleTypeDef htim3;
  GPIO_InitTypeDef GPIO_InitStruct;
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_TIM3_CLK_ENABLE();

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 71;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = LIGHT_MAX_NUM;//
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = LIGHT_MAX_NUM;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
}

void SetLightVal(unsigned char ucVal)
{
    if(ucVal>=LIGHT_MAX_NUM)
        ucVal = LIGHT_MAX_NUM;
    if(ucVal<=LIGHT_MIN_NUM)
        ucVal = LIGHT_MIN_NUM;
    TIM3->ARR = LIGHT_MAX_NUM;
    TIM3->CCR4 = ucVal;
}
#if WWDG_EN
/*****************************
   /|\�ݼ�������
    |
   T|\
    | \���ɸ���
    |  \
   W|___\����
    |   |\
    |   | \�ɸ���
    |   |  \
0x3F|___|___\����
    |   |   |
    |___|___|______\ʱ��
                   /
T[6:0]ÿ(4096x2^WDGTB)��PCLK1���ڼ�1
�����һ�Ƕ೤ʱ�䣬�����������µ������ޣ��Է�Ƶ8���м���
Tgog = (1/36MHZ)*4096*2^3    //����36Mhz����Ϊʱ��ԴΪ36Mhz
Tdog = 9.102*10^-4s = 910us
��������ʱ����Ҫ5ms��������Ҫ25ms����ô
Tmp = 5ms/Tdog = 5000/910 = 5;
����tmp����ι��ʱ��
******************************/
void MX_WWDG_Init(void)
{
  __HAL_RCC_WWDG_CLK_ENABLE();
  hwwdg.Instance = WWDG;
  hwwdg.Init.Prescaler = WWDG_PRESCALER_8;
  hwwdg.Init.Window = WINDOW_TIME;
  hwwdg.Init.Counter = CNT_TIME;
  hwwdg.Init.EWIMode = WWDG_EWI_DISABLE;
  if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
  {
    Error_Handler();
  }

}

void WWDG_Refresh(unsigned long ulCnt)
{
  WRITE_REG(WWDG->CR, ulCnt);
}

#endif

/* USER CODE END 4 */

/* USER CODE BEGIN Header_Task1_Function */
/**
  * @brief  Function implementing the Task1 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Task1_Function */
void Task1_Function(void const * argument)
{
  /* init code for FATFS */

  /* USER CODE BEGIN 5 */

  /* Infinite loop */
  for(;;)
  {
    //Redraw_cbBK();
    while(1)
    {
        osDelay(1000);
    }
    //while(HAL_UART_Transmit_IT(&huart3, txData, 1)!=HAL_OK);
  }
  /* USER CODE END 5 */
}


/* USER CODE BEGIN Header_HMIDisplay */
/**
  * @brief  Function implementing the HMI_Display thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_HMIDisplay */
void Hmi_Display(void const * argument)
{
  FRESULT res;


  //unsigned char i;

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    //MX_FATFS_Init();//SD����ʼ��
    //res=f_mount(&SDFatFS, SDPath, 1);//����SD��

    //if(res!=FR_OK)
    {
    	/*��ʾ����*/

    //	BackLightInit();
    //	ucLightNessVal = 100;
    //	SetLightVal(ucLightNessVal);
    //	for(;;);
    }
    lv_init();
    lv_port_disp_init();//��ʾ�����������ڴ���ڴ˺�����

    BackLightInit();
    ucLightNessVal = 100;
    SetLightVal(ucLightNessVal);

    lv_demo_stress();

    while(1)
    {
        lv_task_handler();
    	osDelay(10);
    }
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_TASKKey */
/**
* @brief Function implementing the TASK_KeyScan thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TASKKey */
void Task_Key(void const * argument)
{
  /* USER CODE BEGIN TASKKey */
  /* Infinite loop */
  for(;;)
  {

	KeyScan_KeyMessage();

    vTaskDelay(50);
  }
  /* USER CODE END TASKKey */
}


/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    lv_tick_inc(1);
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
