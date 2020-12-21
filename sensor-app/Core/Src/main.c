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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "settings.h"
#include "radio.h"
#include "sensor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef hlpuart1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

WWDG_HandleTypeDef hwwdg;

/* USER CODE BEGIN PV */
char dbg_buf[32];
void dbg(const char *msg)
{
	/* Place your implementation of fputc here */
	uint16_t len = strlen(msg);
	uint8_t buf[len+2];
	memcpy(buf, msg, len);
	buf[len] = '\r';
	buf[len+1] = '\n';
	HAL_UART_Transmit(&hlpuart1, buf, len+2, 500);
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_WWDG_Init(void);
static void MX_RTC_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */

static void system_low_power_mode_config(void);
static void enter_low_power_mode();
static uint32_t adc_read_vbatt();
uint8_t PWR_PVDGetLevel(void)
{
	uint8_t temp_val = 0;
  temp_val = (uint8_t) ((PWR->CR >> 5) & 0x07);
  return temp_val;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Last measured values
static float temperature, pressure, humidity;

// Last measured values
const uint8_t radio_health_check_state = 4;
static uint8_t radio_health_check_state_cnt = 0;

// Msg frame
static radio_msg_sensor_frame msgf =
{
	.hdr = {'L','U','6'},
};
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
  /* Check and handle if the system was resumed from StandBy mode */
  if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
  {
		/* Clear Standby flag */
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  }
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_WWDG_Init();
  MX_RTC_Init();
  MX_LPUART1_UART_Init();
  //MX_ADC_Init(); NOT used currently
  /* USER CODE BEGIN 2 */
  dbg("LUK6");

  // Read current config from eprom
  app_settings_init();
  // Enable sensor
  sensor_init();
  // Enable radio
  radio_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	/* Check if we should wake up or go to sleep again */
	if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2) < app_settings_get_current()->update_interval)
	{
		sprintf(dbg_buf, "M_LWP_LOOP:%d, %d", HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2), app_settings_get_current()->update_interval);
		dbg(dbg_buf);
		enter_low_power_mode();
		continue;
	}
	else
	{
		sprintf(dbg_buf, "M_LWP_EXIT:%d", HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2));
		dbg(dbg_buf);
		// Enable sensor pin
		HAL_GPIO_WritePin(SENSOR_VDD_GPIO_Port, SENSOR_VDD_Pin, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, 0);
	}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	/* Read data from the sensor */
	if (sensor_get_data(&temperature, &pressure, &humidity))
	{
		msgf.status = MSG_NO_ERROR;
	}
	else
	{
		dbg("M_MNO");
		msgf.status = MSG_READ_ERROR;
	}
	msgf.temperature = temperature;
	msgf.pressure = pressure;
	msgf.humidity = humidity;
	msgf.vbatt = 3300;//adc_read_vbatt();

	/* Send result data */
	radio_send(&msgf);

	/* Refresh watchdog, no more than 2[s] must expire before next refresh */
	HAL_WWDG_Refresh(&hwwdg);
	uint8_t rx_msg_wait_cnt_secs = 5; // 5 seconds
	while (rx_msg_wait_cnt_secs--)
	{
		/* Wait 1 second for incoming data from ledclock */
		HAL_Delay(1000);
		/* Refresh watchdog, no more than 2[s] must expire before next refresh */
		HAL_WWDG_Refresh(&hwwdg);
	}

	// Check if any data from ledclock came
	if (!radio_was_data_received())
	{
		radio_health_check_state_cnt++;
	}
	else
	{
		radio_health_check_state_cnt = 0;
		radio_data_received_clear();
	}

	// If any problem with radio exists, restrat it
	if (radio_health_check_state_cnt >= radio_health_check_state)
	{
		radio_restart();
	}
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_4;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_RTC;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
  LL_ADC_InitTypeDef ADC_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /** Configure Regular Channel 
  */
  LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_VREFINT);
  LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_VREFINT);
  /** Common config 
  */
  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
  ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
  ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
  ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
  LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
  LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_1CYCLE_5);
  LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
  LL_ADC_REG_SetSequencerScanDirection(ADC1, LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
  LL_ADC_SetCommonFrequencyMode(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_FREQ_MODE_HIGH);
  LL_ADC_DisableIT_EOC(ADC1);
  LL_ADC_DisableIT_EOS(ADC1);
  //LL_ADC_EnableInternalRegulator(ADC1);
  ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV1;
  ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
  ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  ADC_InitStruct.LowPowerMode = LL_ADC_LP_AUTOPOWEROFF;
  LL_ADC_Init(ADC1, &ADC_InitStruct);
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00000202;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 9600;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_HalfDuplex_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

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

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the WakeUp 
  */
//  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
//  {
//    Error_Handler();
//  }
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
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief WWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_WWDG_Init(void)
{

  /* USER CODE BEGIN WWDG_Init 0 */

  /* USER CODE END WWDG_Init 0 */

  /* USER CODE BEGIN WWDG_Init 1 */
	/*##-2- Init & Start WWDG peripheral ######################################*/
	/* WWDG clock counter = (PCLK1 (1MHz/4096)/8) = 30.52 Hz (~32.7 ms)
	 WWDG Window value = 127 means that the WWDG counter should be refreshed only
	 when the counter is below 127 (and greater than 64) otherwise a reset will
	 be generated.
	 WWDG Counter value = 127, WWDG timeout = 32.7 ms * 64 = 2.097 s
	 In this case the refresh window is comprised between : ~32.7 * (127-127) = 0[ms] and ~32.7 * 64 = 2.097[s]
	*/
  /* USER CODE END WWDG_Init 1 */
  hwwdg.Instance = WWDG;
  hwwdg.Init.Prescaler = WWDG_PRESCALER_8;
  hwwdg.Init.Window = 127;
  hwwdg.Init.Counter = 127;
  hwwdg.Init.EWIMode = WWDG_EWI_DISABLE;
  if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN WWDG_Init 2 */

  /* USER CODE END WWDG_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SENSOR_VDD_GPIO_Port, SENSOR_VDD_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SX1278_NSS_GPIO_Port, SX1278_NSS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SX1278_RESET_GPIO_Port, SX1278_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SENSOR_VDD_Pin SX1278_NSS_Pin */
  GPIO_InitStruct.Pin = SENSOR_VDD_Pin|SX1278_NSS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SX1278_DIO0_Pin */
  GPIO_InitStruct.Pin = SX1278_DIO0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(SX1278_DIO0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SX1278_RESET_Pin */
  GPIO_InitStruct.Pin = SX1278_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SX1278_RESET_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

}

/* USER CODE BEGIN 4 */

//-----------------------------------------------------------------------------
/**
  * @brief  System Low Power Configuration
  *         The system Power is configured as follow :
  *            + Regulator in LP mode
  *            + VREFINT OFF, with fast wakeup enabled
  *            + HSI as SysClk after Wake Up
  *            + No IWDG
  *            + Automatic Wakeup using RTC clocked by LSI
  * @param  None
  * @retval None
  */
static void system_low_power_mode_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure = {0};

  /* Enable Ultra low power mode */
  HAL_PWREx_EnableUltraLowPower();

  /* Enable the fast wake up from Ultra low power mode */
  HAL_PWREx_EnableFastWakeUp();

//  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
//  GPIO_InitStructure.Pin = GPIO_PIN_All;
//  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
//  GPIO_InitStructure.Pull = GPIO_NOPULL;
//
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//  /* Disable GPIOs clock */
//  __HAL_RCC_GPIOA_CLK_DISABLE();
//  __HAL_RCC_GPIOB_CLK_DISABLE();
//  __HAL_RCC_GPIOC_CLK_DISABLE();
}

//-----------------------------------------------------------------------------
/**
  * @brief  Puts device into low power STOP mode
  *
  * @param  None
  * @retval None
  */
static void enter_low_power_mode()
{
	/* If defined, device goes into standby mode instead of stop mode */
	//#define LOW_POWER_STANBY_MODE


	/* Put radio into sleep mode */
	radio_sleep();

	/* Disable sensor */
	HAL_GPIO_WritePin(SENSOR_VDD_GPIO_Port, SENSOR_VDD_Pin, GPIO_PIN_RESET);

	/* Disable all used wakeup sources */
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

	/* Clear all related wakeup flags */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	/* Update timeout register */
	uint32_t tmp = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
	tmp += 20; // In seconds
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, tmp);

	/* ## Setting the Wake up time ############################################*/
	/*  RTC Wakeup Interrupt Generation:
	  Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE or LSI))
	  Wakeup Time = Wakeup Time Base * WakeUpCounter
	  = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE or LSI)) * WakeUpCounter
		==> WakeUpCounter = Wakeup Time / Wakeup Time Base

	  To configure the wake up timer to 20s the WakeUpCounter is set to 0xB4D8:
	  RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
	  Wakeup Time Base = 16 /(~37.000KHz) = ~0.432 ms
	  Wakeup Time = ~20s = 0.432ms * WakeUpCounter
		==> WakeUpCounter = ~20s/0.432ms = 20000/0.432 = 46296 = 0xB4D8 */
	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0xB4D8, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

	/* Disable SysTick. */
	//HAL_SuspendTick();
	/* Enable System Low Power */
	system_low_power_mode_config();

#ifdef LOW_POWER_STANBY_MODE
	/* Enter Low Power Standby mode */
	dbg("M_LPM_STANDBY");
	HAL_PWR_EnterSTANDBYMode();
#else
	/* Enter Low Power Stop Mode */
	dbg("M_LPM_STOP");
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
#endif

	/* WakeUp happens here */
	/* Enable SysTick. */
	//HAL_ResumeTick();
	//HAL_GPIO_WritePin(SX1278_RESET_GPIO_Port, SX1278_RESET_Pin, GPIO_PIN_RESET);
	dbg("M_LPM_RUN");
}

//-----------------------------------------------------------------------------
/**
  * @brief  RTC Wake Up callback
  * @param  None
  * @retval None
  */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	/* Clear Wake Up Flag */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
}

//---------------------------------------------------------------------------------
static uint32_t adc_read_vbatt()
{
	LL_ADC_EnableInternalRegulator(ADC1);
	LL_ADC_SetCommonPathInternalCh( __LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_VREFINT );
	do
	{
		LL_ADC_Enable(ADC1);
	} while( !LL_ADC_IsActiveFlag_ADRDY(ADC1));

	LL_ADC_REG_StartConversion(ADC1);
	while( !LL_ADC_IsActiveFlag_EOC(ADC1));

	uint32_t adc_raw = LL_ADC_REG_ReadConversionData12(ADC1);
	do
	{
		LL_ADC_Disable(ADC1);
	} while( LL_ADC_IsEnabled(ADC1) );
	LL_ADC_DisableInternalRegulator(ADC1);
	return __LL_ADC_CALC_VREFANALOG_VOLTAGE(adc_raw, LL_ADC_RESOLUTION_12B);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1);
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
     tex: dbg("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
