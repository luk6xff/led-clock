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
#include <sx1278-cube-hal.h>
#include "lib/BME280/platform/cube/bme280-cube-hal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/**
 * @brief Msg frame status field description
 */
typedef enum
{
	MSG_NO_ERROR   = 1<<0,
	MSG_READ_ERROR = 1<<1,
	MSG_INIT_ERROR = 1<<2,
	MSG_BATT_LOW   = 1<<3,
} radio_msg_sensor_frame_status;

/**
 * @brief Msg frame footprint, sent to the clock
 */
typedef struct
{
	uint8_t hdr[3];
	uint8_t status;
	uint32_t vbatt;
	float temperature;
	float pressure;
	float humidity;
	uint32_t checksum;
} radio_msg_sensor_frame;

/**
 * @brief Msg frame footprint, received from the clock
 */
typedef struct
{
	uint8_t hdr[3];
	uint8_t status;
	uint32_t update_data_interval; //[s] in seconds
	uint32_t checksum;
} radio_msg_clock_frame;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// Radio LORA settings
#define RF_FREQUENCY                                RF_FREQUENCY_434_0
#define TX_OUTPUT_POWER                             14        // dBm
#define LORA_BANDWIDTH                              LORA_BANDWIDTH_125kHz
#define LORA_SPREADING_FACTOR                       LORA_SF8
#define LORA_CODINGRATE                             LORA_ERROR_CODING_RATE_4_5
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_FHSS_ENABLED                           false
#define LORA_NB_SYMB_HOP                            4
#define LORA_IQ_INVERSION_ON                        false
#define LORA_CRC_ENABLED                            true
#define RX_TIMEOUT_VALUE                            5000      // in ms
#define MAX_PAYLOAD_LENGTH                          60        // bytes

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

WWDG_HandleTypeDef hwwdg;

/* USER CODE BEGIN PV */
char dbg_buf[32];
static void dbg(const char* msg)
{
	/* Place your implementation of fputc here */
	uint16_t len = strlen(msg);
	HAL_UART_Transmit(&huart2, msg, len, 500);
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_WWDG_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
uint16_t radio_msg_frame_checksum(const uint8_t *data, const uint8_t data_len);
static void radio_init();
static void sensors_init();
static void parse_incoming_msg_clock(uint8_t *payload, uint16_t size);
static void standby_state_enter();
void on_tx_done(void *args);
void on_tx_timeout(void *args);
void on_rx_done(void *args, uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// LORA SX1278 RADIO
sx1278_cube_hal radio_cube_hal_dev =
{
	.spi = &hspi1,
	.nss_port = SX1278_NSS_GPIO_Port,
	.nss_pin = SX1278_NSS_Pin,
	.reset_port = SX1278_RESET_GPIO_Port,
	.reset_pin = SX1278_RESET_Pin,
};
RadioEvents_t radio_events;
sx1278 radio_dev;

// BME280 SENSOR
bme280 bme280_dev =
{
	.intf = BME280_INTF_I2C,
	.i2c_addr = 0xEC,
	.t_fine = 0,
	.t_fine_adjust = 0,
};
bme280_cube_hal bme280_cube_hal_dev =
{
	.i2c = &hi2c1,
};

// Last measured values
static float temperature, pressure, humidity;

// Update sensor data interval
static uint32_t update_data_interval = 10;

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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  //MX_WWDG_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  dbg("LUK6\n\r");

  // Check and handle if the system was resumed from StandBy mode
  if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
  {
    // Clear Standby flag
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  }

  radio_init();
  sensors_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

      // Sleep a given time in seconds
      sx1278_delay_ms(update_data_interval*1000);

      // Read data from the sensor
      if (bme280_read_data(&bme280_dev, &temperature, &pressure, &humidity))
      {
          sprintf(dbg_buf, "T:%d, P:%d, H:%d\n\r", (int)temperature, (int)pressure, (int)humidity);
          dbg(dbg_buf);
          msgf.status = MSG_NO_ERROR;
      }
      else
      {
          dbg("RD_F\n");
          msgf.status = MSG_READ_ERROR;
      }
      msgf.temperature = temperature;
      msgf.pressure = pressure;
      msgf.humidity = humidity;
      msgf.checksum = radio_msg_frame_checksum((const uint8_t*)&msgf, (sizeof(msgf)-sizeof(msgf.checksum)));
      // Send result data
      sx1278_send(&radio_dev, (uint8_t*)&msgf, sizeof(msgf));
      dbg("DT_S\n\r");
      sx1278_set_rx(&radio_dev, 0);
      //standby_state_enter();
      /**
       * @note WWDG Watchdog init function done in wwdg.c (MX_WWDG_Init(void))
       *	   Window time configured with following values:
       * 	   (PCLK1 (1048000[Hz]) / 4096 / LL_WWDG_PRESCALER_8) = 31.99[Hz] (~30[ms])
       * 	   WWDG Window value = 80 means that the WWDG counter should be refreshed only
       *   	   when the counter is below 80 (and greater than 64) otherwise a reset will
       *  	   be generated.
       *  	   WWDG Downcounter value = 127, WWDG timeout = 30 ms * 64 = 1920[ms]
       * 	   127 / 32 =~ 4[s]
       */
      //HAL_WWDG_Refresh(&hwwdg);
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
  if (HAL_RTCEx_SetWakeUpTimer(&hrtc, 0, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
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
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_HalfDuplex_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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

  /* USER CODE END WWDG_Init 1 */
  hwwdg.Instance = WWDG;
  hwwdg.Init.Prescaler = WWDG_PRESCALER_8;
  hwwdg.Init.Window = 80;
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == SX1278_DIO0_Pin)
	{
		dbg("EXT\n");
		(radio_dev.dio_irq[0])(&radio_dev);
	}
}

//-----------------------------------------------------------------------------
static void radio_init()
{
	// Set radio_dev
	radio_dev.events = &radio_events;
	radio_dev.events->tx_done = on_tx_done;
	radio_dev.events->rx_done = on_rx_done;
	radio_dev.events->tx_timeout = on_tx_timeout;
	radio_dev.events->rx_timeout = NULL;
	radio_dev.events->rx_error = NULL;
	sx1278_cube_hal_init(&radio_dev, &radio_cube_hal_dev);
	sx1278_set_channel(&radio_dev, RF_FREQUENCY);

	// Verify if SX1278 connected to the the board
	while (sx1278_read(&radio_dev, REG_VERSION) != 0x12)
	{
	    dbg("RD_N\n");
	    sx1278_delay_ms(1000);
	}

	sx1278_set_max_payload_length(&radio_dev, MODEM_LORA, MAX_PAYLOAD_LENGTH);
	sx1278_set_tx_config(&radio_dev, MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
	                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
	                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
	                      LORA_CRC_ENABLED, LORA_FHSS_ENABLED, LORA_NB_SYMB_HOP,
	                      LORA_IQ_INVERSION_ON, 4000);

    sx1278_set_rx_config(&radio_dev, MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                          LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                          LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON, 0,
                          LORA_CRC_ENABLED, LORA_FHSS_ENABLED, LORA_NB_SYMB_HOP,
                          LORA_IQ_INVERSION_ON, true);
}

//-----------------------------------------------------------------------------
void on_tx_done(void *args)
{
    dbg("OT_D\n\r");
}

//-----------------------------------------------------------------------------
void on_tx_timeout(void *args)
{
    dbg("OT_T\n\r");
}

//-----------------------------------------------------------------------------
void on_rx_done(void *args, uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
	dbg("> on_rx_done\n\r");
	parse_incoming_msg_clock(payload, size);
}

//-----------------------------------------------------------------------------
static void sensors_init()
{
	// Set sensors_dev
	if (!bme280_cube_hal_init(&bme280_dev, &bme280_cube_hal_dev))
	{
		dbg("SI_F\n\r");
		Error_Handler();
	}
}

//------------------------------------------------------------------------------
static void parse_incoming_msg_clock(uint8_t *payload, uint16_t size)
{
    const radio_msg_clock_frame *mf = (const radio_msg_clock_frame *)payload;
    const uint32_t checksum = radio_msg_frame_checksum((const uint8_t*)mf, (sizeof(radio_msg_clock_frame)-sizeof(mf->checksum)));
    if (mf->checksum != checksum)
    {
        dbg("INV CHKSUM!");
        return;
    }

    if (~(mf->status & MSG_NO_ERROR))
    {
        dbg("MSG_NO_ERROR\n\r");
        update_data_interval = mf->update_data_interval;
        sprintf(dbg_buf, "UT:%d", mf->update_data_interval);
        dbg(dbg_buf);
    }
}

//-----------------------------------------------------------------------------
static void standby_state_enter()
{
	// Put sensor and radio into sleep mode
	sx1278_set_sleep(&radio_dev);
	HAL_GPIO_WritePin(SENSOR_VDD_GPIO_Port, SENSOR_VDD_Pin, GPIO_PIN_RESET);

	// Enable Ultra low power mode */
	HAL_PWREx_EnableUltraLowPower();

	// Enable the fast wake up from Ultra low power mode
	HAL_PWREx_EnableFastWakeUp();

	// Disable all used wakeup sources
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

	// Clear all related wakeup flags
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	// Disable All interrupts
	__disable_irq();

	// Setting the Wake up time
	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 10,  RTC_WAKEUPCLOCK_CK_SPRE_16BITS);

	// Enter the Standby mode
	HAL_PWR_EnterSTANDBYMode();
}

//-----------------------------------------------------------------------------
uint16_t radio_msg_frame_checksum(const uint8_t *data, const uint8_t data_len)
{
    uint8_t i;
    uint16_t sum = 0;

    for (i = 0; i < data_len; i++)
    {
    	sum = sum ^ data[i];
    }
    return sum;
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
