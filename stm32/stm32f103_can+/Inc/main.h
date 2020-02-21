/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include "cmsis_os.h"

//#include "stm32f1xx_hal_uart.h"
//#include "stm32f1xx_hal_can.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

#define LOOP_FOREVER() while(1) {}

#define SET_W25FLASH

#define SET_OLED_SPI
//#define SET_OLED_I2C
//#define SET_MUTEX_LCD
//#define SET_SEM_LCD
#define SET_SSD1306_INVERT

#define SET_FLOAT_PART

//#define SET_MUTEX_UART
#define SET_SEM_UART

#define MAX_UART_BUF 1024//256
#define MAX_CAN_BUF 8


/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define W25_CS_Pin GPIO_PIN_2
#define W25_CS_GPIO_Port GPIOA
#define LED_ERROR_Pin GPIO_PIN_3
#define LED_ERROR_GPIO_Port GPIOA
#define OLED_RST_Pin GPIO_PIN_4
#define OLED_RST_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_6
#define OLED_CS_GPIO_Port GPIOA
#define OLED_DC_Pin GPIO_PIN_7
#define OLED_DC_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOB
#define CAN_LOOP_Pin GPIO_PIN_1
#define CAN_LOOP_GPIO_Port GPIOB
#define W25_SCK_Pin GPIO_PIN_13
#define W25_SCK_GPIO_Port GPIOB
#define W25_MISO_Pin GPIO_PIN_14
#define W25_MISO_GPIO_Port GPIOB
#define W25_MOSI_Pin GPIO_PIN_15
#define W25_MOSI_GPIO_Port GPIOB
#define TxD_Pin GPIO_PIN_9
#define TxD_GPIO_Port GPIOA
#define RxD_Pin GPIO_PIN_10
#define RxD_GPIO_Port GPIOA
#define OLED_SCK_Pin GPIO_PIN_3
#define OLED_SCK_GPIO_Port GPIOB
#define OLED_MOSI_Pin GPIO_PIN_5
#define OLED_MOSI_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#define MSG_PACK 0x321 //message packet[8] = {{vcc.cel, vcc.dro}, ts}
#define MSG_TIME_SET 0x320

#ifdef SET_OLED_SPI
	SPI_HandleTypeDef *portOLED;

	#define CS_OLED_SELECT() HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET)
	#define CS_OLED_DESELECT() HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET)
#endif
#ifdef SET_OLED_I2C
	I2C_HandleTypeDef *portSSD;
#endif

#ifdef SET_W25FLASH
    #define CAN_SPEED_NAME "cspeed"
	SPI_HandleTypeDef *portFLASH;//hspi2
#endif

#ifdef SET_FLOAT_PART
	typedef struct {
		uint16_t cel;
		uint16_t dro;
	} s_float_t;
#endif

#pragma pack(push,1)
typedef struct {
	CAN_RxHeaderTypeDef hdr;
	uint8_t             data[MAX_CAN_BUF];
} s_rx_can_t;
#pragma pack(pop)

UART_HandleTypeDef huart1;
TIM_HandleTypeDef htim2;
HAL_StatusTypeDef i2cError;
osSemaphoreId semLCD;
osMutexId mutexLCD;
QueueHandle_t CanQueue;

const char *eol;

void Report(const char *tag, bool addTime, const char *fmt, ...);
void errLedOn(const char *from);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
