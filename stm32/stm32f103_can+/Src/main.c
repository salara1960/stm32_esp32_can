/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/*
 *
 *                       with : I2C2, CAN, SPI1(+dma), USART1(+dma)
 *
post-build steps command:
arm-none-eabi-objcopy -O binary "${BuildArtifactFileBaseName}.elf" "${BuildArtifactFileBaseName}.bin" && ls -la | grep "${BuildArtifactFileBaseName}.*"

LINK:
-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -u_printf_float -T"../STM32F407VGTx_FLASH.ld" -Wl,-Map=output.map -Wl,--gc-sections -lm

-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -u_printf_float -specs=nosys.specs -T"../STM32F407VGTx_FLASH.ld" -Wl,-Map=output.map -Wl,--gc-sections -lm

-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -specs=nosys.specs -specs=nano.specs -u_printf_float -T"../STM32F407VGTx_FLASH.ld" -Wl,-Map=output.map -Wl,--gc-sections -lm
-specs=nano.specs
*/

#include "ssd1306.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#ifdef SET_W25FLASH
	#include "w25.h"
#endif
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

osThreadId defTaskHandle;
/* USER CODE BEGIN PV */

#ifdef SET_OLED_SPI
	SPI_HandleTypeDef *portOLED;//hspi1
#endif

#ifdef SET_W25FLASH
	SPI_HandleTypeDef *portFLASH;//hspi2
#endif
#ifdef SET_SEM_UART
	osSemaphoreId semUART;
#endif
#ifdef SET_MUTEX_UART
	osMutexId mutexUART;
#endif

#ifdef SET_SEM_LCD
	osSemaphoreId semLCD;
#endif
#ifdef SET_MUTEX_CLD
	osMutexId mutexLCD;
#endif

CAN_TxHeaderTypeDef   TxHdr;
CAN_RxHeaderTypeDef   RxHdr;
uint8_t               TxData[MAX_CAN_BUF];
uint8_t               RxData[MAX_CAN_BUF];
uint32_t              TxMailbox;
static uint32_t TxCanLen = 8;
static uint32_t CanMode = CAN_MODE_NORMAL;
QueueHandle_t CanQueue;
static uint32_t can_speed = 0;
static uint32_t new_can_speed = 0;
const char *TAGCAN = "sCAN";

static uint32_t new_page = 0;
volatile uint8_t page_flag = 0;
volatile uint8_t restart_flag = 0;

const char *eol = "\n";



#ifdef SET_OLED_I2C
	I2C_HandleTypeDef *portSSD;
#endif
UART_HandleTypeDef huart1;
TIM_HandleTypeDef htim2;
HAL_StatusTypeDef i2cError = HAL_OK;
volatile static uint32_t secCounter = 0;
volatile static uint64_t HalfSecCounter = 0;
volatile static float dataADC = 0.0;
volatile static bool setDate = false;
static const char *_extDate = "date=";
static const char *_canSpeed = "cspeed=";
static const char *_canPage = "page=";//const char *_canSpeed = "page=";
static const char *_canPAGE = "PAGE=";//const char *_canSpeed = "PAGE=";
static const char *_restart = "restart";
volatile uint8_t csd_flag = 0;
volatile uint32_t extDate = 0;
static char RxBuf[MAX_UART_BUF];
volatile uint8_t rx_uk;
volatile uint8_t uRxByte = 0;


uint8_t GoTxDMA = 0;

//static char buff[MAX_UART_BUF] = {0};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM1_Init(void);
static void MX_SPI1_Init(void);
static void MX_CAN_Init(void);
static void MX_SPI2_Init(void);
void TaskDef(void const * argument);

/* USER CODE BEGIN PFP */
uint32_t get_tmr(uint32_t sec);
void Report(const char *tag, bool addTime, const char *fmt, ...);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_RTC_Init();
  MX_TIM1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(GPIOB, LED1_Pin | LED_ERROR_Pin, GPIO_PIN_SET);//LEDs OFF

  // start timer1 + interrupt
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_Base_Start_IT(&htim1);

  //start ADC1 + interrupt
  HAL_ADC_Start_IT(&hadc1);

  //"start" rx_interrupt
  HAL_UART_Receive_IT(&huart1, (uint8_t *)&uRxByte, 1);

  CanQueue = xQueueCreate(5, sizeof(s_rx_can_t));

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
#ifdef SET_MUTEX_LCD
  osMutexDef(itMutex);
  mutexLCD = osMutexCreate(osMutex(itMutex));
#endif
#ifdef SET_MUTEX_UART
  osMutexDef(uitMutex);
  mutexUART = osMutexCreate(osMutex(uitMutex));
#endif
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
#ifdef SET_SEM_UART
  osSemaphoreDef(sem_Uart);
  semUART = osSemaphoreCreate(osSemaphore(sem_Uart), 1);
#endif
#ifdef SET_SEM_LCD
  osSemaphoreDef(semLCD);
  semLCD = osSemaphoreCreate(osSemaphore(semLCD), 1);
#endif
//-------------------------------------------------

#ifdef SET_OLED_I2C
  	  portSSD = &hi2c2;
#endif
#ifdef SET_OLED_SPI
  	  portOLED = &hspi1;//SPI1 - OLED SSD1306
  	  spi_ssd1306_Reset();
#endif
#if defined(SET_OLED_I2C) || defined(SET_OLED_SPI)
  	  ssd1306_on(true);//screen ON
  	  if (!i2cError) {
  		  ssd1306_init();//screen INIT
  		  if (!i2cError) {
  			  ssd1306_pattern();//set any params for screen
  			  if (!i2cError) {
  				  //ssd1306_invert();//set inverse color mode
  				  //if (!i2cError)
  				  	  ssd1306_clear();//clear screen
  			  }
  		  }
  	  }
#endif
  	//MX_SPI2_Init();
  	//MX_CAN_Init();
  	//-------------------------------------------------
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defTask */
  osThreadDef(defTask, TaskDef, osPriorityNormal, 0, 1024);
  defTaskHandle = osThreadCreate(osThread(defTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

	CAN_FilterTypeDef  sFilterConfig;

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

	if (HAL_GPIO_ReadPin(CAN_LOOP_GPIO_Port, CAN_LOOP_Pin) == GPIO_PIN_RESET) CanMode = CAN_MODE_LOOPBACK;//0
	//hcan.Init.Mode = can_mode;//default CAN_MODE_NORMAL -> if CAN_LOOP_Pin == (GPIO_PIN_SET)1
	// 36000 /12 /(3+2+1) = 500 KHz
	// 36000 /24 /(3+2+1) = 250KHz
    // 36000 /48 /(3+2+1) = 125KHz
	// 36000 /18 /(15+4+1) = 100KHz

	//125//hcan.Init.Prescaler = 48;//for 125 KHz
	//hcan.Init.Mode = CanMode;//CAN_MODE_NORMAL;
	//hcan.Init.SyncJumpWidth = CAN_SJW_3TQ;//CAN_SJW_1TQ
	//hcan.Init.TimeSeg1 = CAN_BS1_3TQ;
	//hcan.Init.TimeSeg2 = CAN_BS2_2TQ;

	uint32_t delit = 48;

#ifdef SET_W25FLASH
	int page = W25qxx_readParamExt(CAN_SPEED_NAME, (void *)&can_speed, typeBIT32, NULL, false);
	if (page < 0) {
		can_speed = 125;//default speed
		page = W25qxx_saveParamExt(CAN_SPEED_NAME, (void *)&can_speed, typeBIT32, sizeof(uint32_t), false);
	}
	if (page >= 0) {
		switch (can_speed) {
		    case 1000: delit =  6; break;
			case  500: delit = 12; break;
			case  250: delit = 24; break;
				//default: delit = 48;
		}
	}
#endif

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = delit;//48;
  hcan.Init.Mode = CanMode;//CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_3TQ;//CAN_SJW_1TQ
  hcan.Init.TimeSeg1 = CAN_BS1_3TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  if (!can_speed) can_speed = (36000 / hcan.Init.Prescaler) / (1 + 3 + 2);
  //can_speed = (36000 / hcan.Init.Prescaler) / (1 + 3 + 2);//250KHz
  //can_speed = (36000 / hcan.Init.Prescaler) / (1 + 3 + 2);//125KHz
  //can_speed = (36000 / hcan.Init.Prescaler) / (1 + 15 + 4);//100KHz


  /* Configure the CAN Filter */
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK) {
	  errLedOn(__func__);
    Error_Handler();
  }

  /* Activate CAN RX notification */
  if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
	  errLedOn(__func__);
    Error_Handler();
  }

  /* Configure Transmission process */
  /*TxHdr.StdId = MSG_PACK;
  TxHdr.ExtId = 0x01;
  TxHdr.RTR = CAN_RTR_DATA;
  TxHdr.IDE = CAN_ID_STD;
  TxHdr.DLC = TxCanLen;
  TxHdr.TransmitGlobalTime = DISABLE;*/

  /* Start the CAN peripheral */
  if (HAL_CAN_Start(&hcan) != HAL_OK) {
	  errLedOn(__func__);
    Error_Handler();
  }

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
    
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date 
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 1;
  DateToUpdate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */
#ifdef SET_W25FLASH
	portFLASH = &hspi2;//SPI2 - W25_Flash
	W25_UNSELECT();

  	W25qxx_Init(false);
#endif
  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 17999;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 499;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, W25_CS_Pin|LED_ERROR_Pin|OLED_RST_Pin|OLED_CS_Pin 
                          |OLED_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : W25_CS_Pin LED_ERROR_Pin */
  GPIO_InitStruct.Pin = W25_CS_Pin|LED_ERROR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : OLED_RST_Pin OLED_CS_Pin OLED_DC_Pin */
  GPIO_InitStruct.Pin = OLED_RST_Pin|OLED_CS_Pin|OLED_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LED1_Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CAN_LOOP_Pin */
  GPIO_InitStruct.Pin = CAN_LOOP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(CAN_LOOP_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
//*************************************************************************************************
osStatus waitEventUart()
{
#ifdef SET_MUTEX_UART
    return osMutexWait(mutexUART, portMAX_DELAY);
#else
	#ifdef SET_SEM_UART
    	return osSemaphoreWait(semUART, portMAX_DELAY);
	#else
    	return osOK;
	#endif
#endif
}
//-----------------------------------------------------------------------------------------
void doneEventUart()
{
#ifdef SET_MUTEX_UART
	osMutexRelease(mutexUART);
#else
	#ifdef SET_SEM_UART
		osSemaphoreRelease(semUART);
	#endif
#endif
}
//-----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// set LED_ERROR when error on and send message to UART1 (in from != NULL)
//     from - name of function where error location
void errLedOn(const char *from)
{
	HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_RESET);//LED ON
	HAL_Delay(250);
	HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);//LED OFF
	HAL_Delay(250);
	HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_RESET);//LED ON

	if (from) Report(NULL, true, "Error in function '%s'\r\n", from);
}
//------------------------------------------------------------------------------------------
void set_Date(time_t epoch)
{
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;
struct tm ts;

	gmtime_r(&epoch, &ts);

	sDate.WeekDay = ts.tm_wday;
	sDate.Month   = ts.tm_mon + 1;
	sDate.Date    = ts.tm_mday;
	sDate.Year    = ts.tm_year;
	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) errLedOn(__func__);
	else {
		sTime.Hours   = ts.tm_hour;
		sTime.Minutes = ts.tm_min;
		sTime.Seconds = ts.tm_sec;
		if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) errLedOn(__func__);
		else {
			setDate = true;
		}
	}
}
//------------------------------------------------------------------------------------------
uint32_t get_Date()
{
	if (!setDate) return get_tmr(0);

	struct tm ts;

	RTC_TimeTypeDef sTime = {0};
	if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) return get_tmr(0);
	ts.tm_hour = sTime.Hours;
	ts.tm_min  = sTime.Minutes;
	ts.tm_sec  = sTime.Seconds;

	RTC_DateTypeDef sDate = {0};
	if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) return get_tmr(0);
	ts.tm_wday = sDate.WeekDay;
	ts.tm_mon  = sDate.Month - 1;
	ts.tm_mday = sDate.Date;
	ts.tm_year = sDate.Year;

	return ((uint32_t)mktime(&ts));
}
//-----------------------------------------------------------------------------
int sec_to_str_time(uint32_t sec, char *stx)
{
int ret = 0;

	if (!setDate) {//no valid date in RTC
		uint32_t day = sec / (60 * 60 * 24);
		sec %= (60 * 60 * 24);
		uint32_t hour = sec / (60 * 60);
		sec %= (60 * 60);
		uint32_t min = sec / (60);
		sec %= 60;
		ret = sprintf(stx, "%lu.%02lu:%02lu:%02lu", day, hour, min, sec);
	} else {//in RTC valid date (epoch time)
		RTC_TimeTypeDef sTime;
		RTC_DateTypeDef sDate;
		if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) errLedOn(__func__);
		else {
			if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) errLedOn(__func__);
			else {
				ret = sprintf(stx, "%02u.%02u %02u:%02u:%02u",
								   sDate.Date, sDate.Month,
								   sTime.Hours, sTime.Minutes, sTime.Seconds);
			}
		}
	}

	return ret;
}
//-----------------------------------------------------------------------------
uint32_t get_secCounter()
{
	return secCounter;
}
//-----------------------------------------------------------------------------
void inc_secCounter()
{
	secCounter++;
}
//-----------------------------------------------------------------------------
uint64_t get_hsCounter()
{
	return HalfSecCounter;
}
//-----------------------------------------------------------------------------
void inc_hsCounter()
{
	HalfSecCounter++;
}
//------------------------------------------------------------------------------------------
uint32_t get_tmr(uint32_t sec)
{
	return (get_secCounter() + sec);
}
//------------------------------------------------------------------------------------------
bool check_tmr(uint32_t sec)
{
	return (get_secCounter() >= sec ? true : false);
}
//------------------------------------------------------------------------------------------
uint64_t get_hstmr(uint64_t hs)
{
	return (get_hsCounter() + hs);
}
//------------------------------------------------------------------------------------------
bool check_hstmr(uint64_t hs)
{
	return (get_hsCounter() >= hs ? true : false);
}
//----------------------------------------------------------------------------------------
int sec_to_string(uint32_t sec, char *stx)
{
int ret = 0;

	if (!setDate) {//no valid date in RTC
		uint32_t day = sec / (60 * 60 * 24);
		sec %= (60 * 60 * 24);
		uint32_t hour = sec / (60 * 60);
		sec %= (60 * 60);
		uint32_t min = sec / (60);
		sec %= 60;
		ret = sprintf(stx, "%03lu.%02lu:%02lu:%02lu ", day, hour, min, sec);
	} else {//in RTC valid date (epoch time)
		RTC_TimeTypeDef sTime;
		RTC_DateTypeDef sDate;
		if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) errLedOn(__func__);
		else {
			if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) errLedOn(__func__);
			else {
				ret = sprintf(stx, "%02u.%02u %02u:%02u:%02u ",
							sDate.Date, sDate.Month, 
							sTime.Hours, sTime.Minutes, sTime.Seconds);
			}
		}
	}

    return ret;
}
//------------------------------------------------------------------------------------------
#ifdef SET_FLOAT_PART
void floatPart(float val, s_float_t *part)
{
	part->cel = (uint16_t)val;
	part->dro = (val - part->cel) * 1000;
}
#endif
//----------------------------------------------------------------------------------------
void Report(const char *tag, bool addTime, const char *fmt, ...)
{
HAL_StatusTypeDef er = HAL_OK;
va_list args;
size_t len = MAX_UART_BUF;
int dl = 0;

	char *buff = (char *)pvPortMalloc(len);//vPortFree(buff);
	if (buff) {
		buff[0] = 0;
		if (addTime) {
			uint32_t ep;
			if (!setDate) ep = get_secCounter();
					 else ep = extDate;
			dl = sec_to_string(ep, buff);
		}
    if (tag) dl += sprintf(buff+strlen(buff), "[%s] ", tag);
		va_start(args, fmt);
		vsnprintf(buff + dl, len - dl, fmt, args);
		if (waitEventUart() == osOK) {
			//
			er = HAL_UART_Transmit_DMA(&huart1, (uint8_t *)buff, strlen(buff));
			while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) {
				if (HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_RX) break;
				HAL_Delay(1);
			}
			//
			doneEventUart();
		} else er = HAL_ERROR;
		va_end(args);
		vPortFree(buff);
	} else er = HAL_ERROR;

	if (er != HAL_OK) errLedOn(NULL);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance == ADC1) {
		dataADC = ((float)HAL_ADC_GetValue(hadc)) * 3.3 / 4096;
	}
}
//------------------------------------------------------------------------------------------
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) {
		RxBuf[rx_uk & 0xff] = (char)uRxByte;
		if (uRxByte == 0x0d) {//end of line
			char *uk = strstr(RxBuf, _extDate);//const char *_extDate = "date=";
			if (uk) {
				uk += strlen(_extDate);
				if (*uk != '?') {
					if (strlen(uk) < 10) setDate = false;
					else {
						extDate = atoi(uk);
						set_Date((time_t)extDate);
					}
				} else setDate = true;
			} else {
				uk = strstr(RxBuf, _canSpeed);//const char *_canSpeed = "cspeed=";
				if (uk) {
					uk += strlen(_canSpeed);
					if (!csd_flag) {
						new_can_speed = atoi(uk);
						csd_flag = 1;
					}
				} else {
					uk = strstr(RxBuf, _canPage);//const char *_canSpeed = "page=";
					if (uk) {
						uk += 5;
						if (!page_flag) {
							new_page = atoi(uk);
							page_flag = 1;
						}
					} else {
						uk = strstr(RxBuf, _canPAGE);//const char *_canSpeed = "PAGE=";
						if (uk) {
							uk += 5;
							if (!page_flag) {
								new_page = atoi(uk);
								page_flag = 2;
							}
						} else {
							if (strstr(RxBuf, _restart)) {//const char *_restart = "restart";
								if (!restart_flag) restart_flag = 1;
							}
						}
					}
				}
			}
			rx_uk = 0;
			memset(RxBuf, 0, sizeof(RxBuf));
		} else rx_uk++;

		HAL_UART_Receive_IT(huart, (uint8_t *)&uRxByte, 1);

	}
}
//-------------------------------------------------------------------------------------------
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hc)
{
    if (hc->Instance == CAN1) {
    	if (HAL_CAN_GetRxMessage(hc, CAN_RX_FIFO0, &RxHdr, RxData) == HAL_OK) {
    		bool yes = false;
    		if (hc->Init.Mode == CAN_MODE_NORMAL) {
    			if (RxHdr.StdId == MSG_TIME_SET) {
    				uint32_t epoch = 0;
    				memcpy((uint8_t *)&epoch, RxData, sizeof(uint32_t));
    				set_Date((time_t)epoch);
    				yes = true;
    			}
    		} else if (hc->Init.Mode == CAN_MODE_LOOPBACK) {
    			if (RxHdr.StdId == MSG_PACK) {
    				yes = true;
    			}
    		}
    		if (yes) {
    			s_rx_can_t evt;
    			memcpy((uint8_t *)&evt, (uint8_t *)&RxHdr, sizeof(CAN_RxHeaderTypeDef));
    			memcpy((uint8_t *)&evt.data, RxData, RxHdr.DLC);
    			BaseType_t PriorityTaskWoken = 1, xCopyPosition = 0;
    			xQueueGenericSendFromISR(CanQueue, (const void *)&evt, &PriorityTaskWoken, xCopyPosition);
    		}
    	} else Error_Handler();
    }
}
//*************************************************************************************************
/* USER CODE END 4 */

/* USER CODE BEGIN Header_TaskDef */
/**
  * @brief  Function implementing the defTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_TaskDef */
void TaskDef(void const * argument)
{
  /* USER CODE BEGIN 5 */

	//MX_CAN_Init();

char buf[1024] = {0};

TxMailbox = CAN_TX_MAILBOX0;
uint16_t ik = 0;
s_rx_can_t evt = {0};
char stx[MAX_CAN_BUF << 3] = {0};
char screen[64] = {0};
uint8_t col = 0, cnt = 0;
int8_t i;
uint32_t ts = 0, tmr = get_tmr(1);
uint32_t cnt_tx = 0, cnt_rx = 0;
short dl = 0, ldl = 1;

TxCanLen = 8;
TxHdr.DLC = TxCanLen;
TxHdr.StdId = MSG_PACK;
TxHdr.ExtId = 0;//0x01;
TxHdr.RTR = CAN_RTR_DATA;
TxHdr.IDE = CAN_ID_STD;
TxHdr.TransmitGlobalTime = DISABLE;

#ifdef SET_FLOAT_PART
	s_float_t vcc = {0, 0};
#endif
	//
	switch (hcan.Init.Mode) {
		case CAN_MODE_NORMAL ://(0x00000000U)  // Normal mode
			strcpy(buf, "CAN_MODE_NORMAL");
		break;
		case CAN_MODE_LOOPBACK ://((uint32_t)CAN_BTR_LBKM)  //Loopback mode
			strcpy(buf, "CAN_MODE_LOOPBACK");
		break;
		case CAN_MODE_SILENT : //((uint32_t)CAN_BTR_SILM)  //Silent mode
			strcpy(buf, "CAN_MODE_SILENT");
		break;
		case CAN_MODE_SILENT_LOOPBACK : //((uint32_t)(CAN_BTR_LBKM | CAN_BTR_SILM))  //Loopback + silent mode
			strcpy(buf, "CAN_MODE_SILENT_LOOPBACK");
		break;
			default : strcpy(buf, "Unknown CAN_MODE");
	}
	//
	Report(NULL, false, "%sStart default task | CAN : mode=%s speed=%u KHz%s", eol, buf, can_speed, eol);
	//
#ifdef SET_W25FLASH

	uint8_t *fb = NULL;
	int page = -1;
	uint8_t plen = 0;
	bool priz = false;
	fb = (uint8_t *)pvPortMalloc(w25qxx.PageSize);//vPortFree(buff);
/*
	W25qxx_EraseChip(true);
*/
	AboutFlashChip();
	/*
	if (w25qxx.ID) {
		Report(TAGW25, true, "Start format all sectors (%lu)...", w25qxx.SectorCount);
		for (int sek = 0; sek < w25qxx.SectorCount; sek++) formatSector(sek, false);
		Report(NULL, false, " done%s", w25qxx.SectorCount, eol);
	}
	*/
#endif

	//
	ssd1306_clear_line(2);
	ssd1306_text_xy(screen, ssd1306_calcx(sprintf(screen, "Can : %lu KHz", can_speed)), 2);
	//

	/* Infinite loop */
	while (1) {
		if (check_tmr(tmr)) {
			tmr = get_tmr(1);
			if (!i2cError) {
				dl = sec_to_str_time(tmr - 1, buf);
				col = ssd1306_calcx(dl);
				if (dl != ldl) {
					ssd1306_clear_line(1);
					ldl = dl;
				}
#ifdef SET_FLOAT_PART
				floatPart(dataADC, &vcc);
				sprintf(buf+strlen(buf), "\n\nVolt : %u.%u\nFreeMem : %u", vcc.cel, vcc.dro, xPortGetFreeHeapSize());
#else
				sprintf(buf+strlen(buf), "\n\nVolt : %.3f\nFreeMem : %u", dataADC, xPortGetFreeHeapSize());
#endif
				ssd1306_text_xy(buf, col, 1);
			}
			cnt++;
			if (cnt == 10) {
				cnt = 0;
				stx[0] = 0;
				//
				ts = get_Date();
				memcpy(TxData, (uint32_t *)&ts, sizeof(uint32_t));
				memcpy(&TxData[4], (uint8_t *)&vcc, sizeof(uint32_t));
				//
				ik = 0;
				while (!HAL_CAN_GetTxMailboxesFreeLevel(&hcan) && ++ik) {}
				HAL_Delay(5);
				//
				if (HAL_CAN_AddTxMessage(&hcan, &TxHdr, TxData, &TxMailbox) != HAL_OK) errLedOn("HAL_CAN_AddTxMessage()");
				else
				for (i = 0; i < MAX_CAN_BUF; i++) sprintf(stx+strlen(stx), " %02X", TxData[i]);
				cnt_tx++;
				//
                ssd1306_clear_line(6);
                col = ssd1306_calcx(sprintf(screen, "Tx msg #%lu", cnt_tx));
				ssd1306_text_xy(screen, col, 6);
				//
#ifdef SET_FLOAT_PART
				sprintf(buf, "Id=0x%lX #%lu, Vcc=%u.%u Time=%lu TX[%lu]=%s\n",
					     TxHdr.StdId, cnt_tx, vcc.cel, vcc.dro, ts, TxHdr.DLC, stx);
#else
				sprintf(buf, "Id=0x%lX #%lu, Vcc=%.3f Time=%lu TX[%lu]=%s\n",
                                             TxHdr.StdId, cnt_tx, dataADC, TxHdr.DLC, stx);
#endif
				Report(TAGCAN, true, buf);
			}

		}
		//
		if (xQueueReceive(CanQueue, &evt, (TickType_t)0) == pdTRUE) {
			cnt_rx++;
			stx[0] = 0;
			dl = 0;
			for (i = 0; i < evt.hdr.DLC; i++) sprintf(stx+strlen(stx), " %02X", evt.data[i]);
			ssd1306_clear_line(8);
			memcpy((uint8_t *)&ts, (uint8_t *)&evt.data, sizeof(uint32_t));
			if (evt.hdr.StdId == MSG_PACK) {
				dl = sprintf(screen, "Volt : %u.%u", vcc.cel, vcc.dro);
				memcpy((uint8_t *)&vcc, (uint32_t *)&evt.data[4], sizeof(uint32_t));
				Report(TAGCAN, true, "Id=0x%lX #%lu, Vcc=%u.%u Time=%lu RX[%lu]=%s\n",
				                     evt.hdr.StdId, cnt_rx, vcc.cel, vcc.dro, ts, evt.hdr.DLC, stx);
			} else if (evt.hdr.StdId == MSG_TIME_SET) {
				dl = sprintf(screen, "Rx msg #%lu", cnt_rx);
				Report(TAGCAN, true, "Id=0x%lX #%lu, Epoch=%lu RX[%lu]=%s\n",
							         evt.hdr.StdId, cnt_rx, ts, evt.hdr.DLC, stx);
			} else {
				dl = sprintf(screen, "Rx msg #%lu", cnt_rx);
				Report(TAGCAN, true, "Id=0x%lX #%lu, RX[%lu]=%s\n", evt.hdr.StdId, cnt_rx, evt.hdr.DLC, stx);
			}
			if (dl) ssd1306_text_xy(screen, ssd1306_calcx(dl), 8);
		}

		osDelay(1);

#ifdef SET_W25FLASH
		if (csd_flag) {
			csd_flag = 0;
			if (new_can_speed != can_speed) {
				Report(TAGW25, true, "Old_can_speed=%lu New_can_speed=%lu -> save...%s", can_speed, new_can_speed, eol);
				page = W25qxx_saveParamExt(CAN_SPEED_NAME, (void *)&new_can_speed, typeBIT32, sizeof(uint32_t), true);
				if (page != -1) {
					page = W25qxx_readParamExt(CAN_SPEED_NAME, (void *)&can_speed, typeBIT32, &plen, true);//return pageAddr or -1
					Report(TAGW25, true, "Param '%s'=%u KHz present on page #%d%s", CAN_SPEED_NAME, can_speed, page, eol);
					prnPage(page, true);
				}
			}
		}
		if (page_flag) {
			if (page_flag == 2) priz = true; else priz = false;
			page_flag = 0;
			prnPage(new_page, priz);
		}
		if (restart_flag) {
			NVIC_SystemReset();
		}
#endif

	}
#ifdef SET_W25FLASH
	if (fb) vPortFree(fb);
#endif
  /* USER CODE END 5 */ 
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if (htim->Instance == TIM1) {//half seconda
 	  if (get_hsCounter() & 1) {//seconda
 		  inc_secCounter();
 		  HAL_GPIO_TogglePin(GPIOB, LED1_Pin);//set ON/OFF LED1
 		  /*
 		  char buf[32];
 		  sec_to_str_time(get_secCounter(), buf);
 		  ssd1306_text_xy(buf, 2, 8);
 		  */
 	  }
 	  inc_hsCounter();
   }
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
