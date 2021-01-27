/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "stm32f411xe.h"
#include "stdint.h"
#include "stdio.h"
#include "radio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct SIMcard_check {
	char 		cmd[14];		//AT+CPIN?\r\n\0
	uint32_t	timeout;
}sSIMcardCheck_t;

typedef struct radioConfig {
	char		cmd[62];		//AT+QICSGP=1,1,\"virtueyes.com.br\",\"virtu\",\"virtu\",3\r\n\0
	uint32_t	timeout;
}sRadioConfig_t;

typedef struct mqttCmds {
	char		cmd[62];		//AT+QMTCONN=1,\"usrCelmer\",\"zgxbgfsy\",\"H7Mnnfi0_2rk\"\r\n\0
	uint32_t	timeout;
}sMqttCmds_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SIM_CARD_CHECK_TIMEOUT	5000
#define	RADIO_CONFIG_TIMEOUT	300
#define MQTT_CMDS_TIMEOUT		15000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void gpioInit(void);
void uartInit(void);
void breakpoint(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char rxBuf[100] = {'\0'};
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
//	sSIMcardCheck_t	simCard;
//	sRadioConfig_t	radioConfig;
//	sMqttCmds_t		mqttCmds;
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
  /* USER CODE BEGIN 2 */
  gpioInit();
  uartInit();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//  simCard.timeout 		= SIM_CARD_CHECK_TIMEOUT;
//  radioConfig.timeout	= RADIO_CONFIG_TIMEOUT;
//  mqttCmds.timeout		= MQTT_CMDS_TIMEOUT;

  radio_turnOn();
  delay_ms(10000);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  radio_transmit(ATE0, rxBuf);
	  breakpoint();

	  radio_transmit(AT_CPIN, rxBuf);
	  breakpoint();

	  radio_transmit(AT_QCFG_NWSCANMODE, rxBuf);
	  breakpoint();

	  radio_transmit(AT_QCFG_NWSCANSEQ, rxBuf);
	  breakpoint();

	  radio_transmit(AT_QCFG_IOTOPMODE, rxBuf);
	  breakpoint();

	  radio_transmit(AT_QCFG_BAND, rxBuf);
	  breakpoint();

	  radio_transmit(AT_QICSGP, rxBuf);
	  breakpoint();

	  radio_transmit(AT_CFUN0, rxBuf);
	  breakpoint();

	  delay_ms(5000);

	  radio_transmit(AT_CFUN1, rxBuf);
	  breakpoint();

	  radio_transmit(AT_CREG, rxBuf);
	  breakpoint();

	  radio_transmit(AT_CGREG, rxBuf);
	  breakpoint();

	  radio_transmit(AT_CEREG, rxBuf);
	  breakpoint();

	  radio_transmit(AT_QNWINFO, rxBuf);
	  breakpoint();

	  radio_transmit(AT_CSQ, rxBuf);
	  breakpoint();

	  radio_transmit(AT_CGATT1, rxBuf);
	  breakpoint();

	  radio_transmit(AT_QMTOPEN, rxBuf);
	  breakpoint();

	  radio_transmit(AT_QMTCONN, rxBuf);
	  breakpoint();

	  radio_transmit(AT_QMTPUBEX, rxBuf);
	  breakpoint();

	  radio_transmit(AT_QMTDISC, rxBuf);
	  breakpoint();

	  radio_transmit(AT_QMTCLOSE, rxBuf);
	  breakpoint();

	  radio_transmit(AT_CGATT0, rxBuf);
	  breakpoint();

	  radio_transmit(AT_QPOWD, rxBuf);
	  breakpoint();

//	  if(!(GPIOC->IDR & GPIO_IDR_ID13))
//	  {
//		  while(!(GPIOC->IDR & GPIO_IDR_ID13));
//		  radio_transmit(AT_QPOWD);
		  while(1);
//	  }
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/*
 * Peripheral initialization functions
 */
void gpioInit(void)
{
	RCC->AHB1ENR	|= 	RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER	|= 	(0b01 << GPIO_MODER_MODE1_Pos);
	GPIOA->ODR		|= 	GPIO_ODR_OD1;

	RCC->AHB1ENR	|=	RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER	|=	(0b00 << GPIO_MODER_MODE13_Pos);
	GPIOC->PUPDR	|=	(0b00 << GPIO_PUPDR_PUPD13_Pos);
}

void uartInit(void)
{
	RCC->APB1ENR 	|= 	RCC_APB1ENR_USART2EN;

	GPIOA->MODER	|=	(0b10	<< GPIO_MODER_MODE3_Pos) 	|
						(0b10	<< GPIO_MODER_MODE2_Pos) 	;
	GPIOA->OSPEEDR	|=	(0b11	<< GPIO_OSPEEDR_OSPEED3_Pos)|
						(0b11	<< GPIO_OSPEEDR_OSPEED2_Pos);
	GPIOA->AFR[0]	|=	(0b0111 << GPIO_AFRL_AFSEL3_Pos) 	|
						(0b0111 << GPIO_AFRL_AFSEL2_Pos) 	;

	SystemCoreClockUpdate();								//update system core clock
	USART2->BRR 	= 	SystemCoreClock/115200;				//baud rate set to 115.200
	USART2->CR1		|= 	USART_CR1_UE |						//enable USART1
						USART_CR1_RE |
						USART_CR1_TE ;						//enable TX
}

/*
 * General functions
 */
void breakpoint(void)
{
	asm("nop");

	uint8_t i = 0;
	for(i=0; i<100; i++)
	{
		rxBuf[i] = '\0';
	}
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
