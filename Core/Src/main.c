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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
const char AT[]					= {"AT\r\n\0"};
const char AT_QPOWD[] 			= {"AT+QPOWD\r\n\0"};
const char AT_CPIN[]			= {"AT+CPIN?\r\n\0"};
const char AT_QCFG_NWSCANMODE[]	= {"AT+QCFG=\"NWSCANMODE\",0\r\n\0"};
const char AT_QCFG_NWSCANSEQ[]	= {"AT+QCFG=\"NWSCANSEQ\",020103\r\n\0"};
const char AT_QCFG_IOTOPMODE[]	= {"AT+QCFG=\"IOTOPMODE\",2\r\n\0"};
const char AT_QCFG_BAND[]		= {"AT+QCFG=\"BAND\",0XF,0,0X8000004\r\n\0"};
const char AT_QICSGP[]			= {"AT+QICSGP=1,1,\"virtueyes.com.br\",\"virtu\",\"virtu\",3\r\n\0"};
const char AT_CFUN0[]			= {"AT+CFUN=0\r\n\0"};
const char AT_CFUN1[]			= {"AT+CFUN=1\r\n\0"};
const char AT_CREG[]			= {"AT+CREG?\r\n\0"};
const char AT_CGREG[]			= {"AT+CGREG?\r\n\0"};
const char AT_CEREG[]			= {"AT+CEREG?\r\n\0"};
const char AT_CGATT0[]			= {"AT+CGATT=0\r\n\0"};
const char AT_CGATT1[]			= {"AT+CGATT=1\r\n\0"};
const char AT_CSQ[]				= {"AT+CSQ\r\n\0"};
const char ATE0[]				= {"ATE0\r\n\0"};
const char AT_QNWINFO[]			= {"AT+QNWINFO\r\n\0"};

//MQTT commands
const char AT_QMTOPEN[]			= {"AT+QMTOPEN=1,\"tailor.cloudmqtt.com\",13291\r\n\0"};
const char AT_QMTCLOSE[]		= {"AT+QMTCLOSE=1\r\n\0"};
const char AT_QMTCONN[]			= {"AT+QMTCONN=1,\"usrCelmer\",\"zgxbgfsy\",\"H7Mnnfi0_2rk\"\r\n\0"};
const char AT_QMTDISC[]			= {"AT+QMTDISC=1\r\n\0"};
const char AT_QMTSUB[]			= {"AT+QMTSUB=1,1,\"celmer\",1\r\n\0"};
const char AT_QMTUNS[]			= {"AT+QMTUNS=1,1,\"celmer\"\r\n\0"};
const char AT_QMTPUBEX[]		= {"AT+QMTPUBEX=1,1,1,1,\"celmer\",\"many man\"\r\n\0"};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void gpioInit(void);
void uartInit(void);
void delay_ms(uint32_t time_ms);
void breakpoint(void);
void radio_turnOn(void);
void radio_turnOff(void);
void radio_reset(void);
void radio_transmit(const char *txData);
void radio_receive(char *rxData);
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
  radio_turnOn();
  delay_ms(10000);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  radio_transmit(ATE0);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_CPIN);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_QCFG_NWSCANMODE);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_QCFG_NWSCANSEQ);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_QCFG_IOTOPMODE);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_QCFG_BAND);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_QICSGP);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_CFUN0);
	  radio_receive(rxBuf);
	  breakpoint();

	  delay_ms(5000);

	  radio_transmit(AT_CFUN1);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_CREG);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_CGREG);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_CEREG);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_QNWINFO);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_CSQ);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_CGATT1);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_QMTOPEN);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_QMTCONN);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_QMTPUBEX);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_QMTDISC);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_QMTCLOSE);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_CGATT0);
	  radio_receive(rxBuf);
	  breakpoint();

	  radio_transmit(AT_QPOWD);
	  radio_receive(rxBuf);
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

void breakpoint(void)
{
	asm("nop");

	uint8_t i = 0;
	for(i=0; i<100; i++)
	{
		rxBuf[i] = '\0';
	}
}
/*
 * TX Module functions
 */
void radio_turnOn(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(750);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

void radio_turnOff(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(1250);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

void radio_reset(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(2500);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

void radio_transmit(const char *txData)
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

void radio_receive(char *rxData)
{
	uint8_t timeout = 0;
	uint32_t startTick, actualTick;
	uint32_t bufCount = 0;

	actualTick = HAL_GetTick();
	startTick = actualTick;

	while(!timeout)
	{
		if(USART2->SR & USART_SR_RXNE)
		{
			USART2->SR &= ~USART_SR_RXNE;
			if((USART2->DR != '\r') && (USART2->DR != '\n'))
			{
//				*rxData = USART2->DR;						//store it in data
//				rxData++;
				rxData[bufCount] = USART2->DR;
				bufCount++;
			}
			else
			{
				if((bufCount != 0) && (rxData[bufCount - 1] != ' '))
				{
					rxData[bufCount] = ' ';
					bufCount++;
				}
			}
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
				rxData[bufCount - 1] = '\0';
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
