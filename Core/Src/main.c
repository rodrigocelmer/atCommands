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
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BREAKPOINT	asm("nop")
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
const char AT[5]		= {'A','T','\r','\n','\0'};
const char AT_QPOWD[11] = {'A','T','+','Q','P','O','W','D','\r','\n','\0'};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void gpioInit(void);
void uartInit(void);
void delay_ms(uint32_t time_ms);
void txModule_turnOn(void);
void txModule_turnOff(void);
void txModule_reset(void);
void txModule_transmit(const char *txData);
void txModule_receive(char *rxData);
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
  /* USER CODE BEGIN 2 */
  gpioInit();
  uartInit();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  char rxBuf[20] = {'\0'};

  txModule_turnOn();
  txModule_receive(rxBuf);
  txModule_receive(rxBuf);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  txModule_transmit(AT);
	  txModule_receive(rxBuf);
	  BREAKPOINT;
	  txModule_transmit(AT_QPOWD);
	  BREAKPOINT;
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
void delay_ms(uint32_t time_ms)
{
	uint32_t startTick, actualTick;

	startTick = HAL_GetTick();
	actualTick = startTick;

	do
	{
		actualTick = HAL_GetTick();
	}while(actualTick - startTick < time_ms);
}

/*
 * TX Module functions
 */
void txModule_turnOn(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(750);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

void txModule_turnOff(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(1250);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

void txModule_reset(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(2500);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

void txModule_transmit(const char *txData)
{
	uint8_t i = 0, txDataSize = 0;

	txDataSize = strlen(txData);

	for(i = 0; i< txDataSize; i++)
	{
		while(!(USART2->SR & USART_SR_TXE));
		USART2->DR = *txData;
		txData++;
	}
}

void txModule_receive(char *rxData)
{
	uint8_t timeout = 0;
	uint32_t startTick, actualTick;

	actualTick = HAL_GetTick();
	startTick = actualTick;

	while(!timeout)
	{
		if(USART2->SR & USART_SR_RXNE)
		{
			*rxData = USART2->DR;						//store it in data
			rxData++;
			actualTick = HAL_GetTick();
			startTick = actualTick;
		}
		else
		{
			if(actualTick - startTick <= 1)
			{
				actualTick = HAL_GetTick();
			}
			else
			{
				timeout = 1;
			}
		}
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
