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
#include "radio.h"
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UID_SIZE			12				/*!< Actual STM32L053's unique ID size	*/
#define UID_STRING_SIZE		(UID_SIZE + 16)	/*!< 16 bytes offset for uidString		*/
#define SERIAL_NUMBER_SIZE	18
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
void read_uniqueID(char *uid);
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
	char		uidString[UID_STRING_SIZE] = {'\0'};
	char		radioSerialNumber[SERIAL_NUMBER_SIZE] = {'\0'};
	sRadio_t 	radio;

	char logBuff[320] = {'\0'};
	int testCounter = 0, signal = 0, errCnt = 0;
	eRadioStatus_t	radioSignal;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  read_uniqueID(uidString);

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  gpioInit();
  uartInit();
  radio_init(&radio);	//gpio and uartInit() must be inside radio_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  radio.turnOn();

  if(radio.config() == radio_error)							//error on SIM card check
  {
	   Error_Handler();
  }


  if(radio.getSN(radioSerialNumber) == radio_error)
  {
	   Error_Handler();
  }

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	   radioSignal = radio.checkSignal();
	   switch(radioSignal)
	   {
	   	   case radio_error:
	   		   signal = -2;	//fixed value
	   		   break;
	   	   case noSignal:
	   		   signal = 0;
	   		   break;
	   	   case poorSignal:
	   		   signal = 1;
	   		   break;
	   	   case goodSignal:
	   		   signal = 2;
	   		   break;
	   	   case	greatSignal:
	   		   signal = 3;
	   		   break;
	   	   default:
	   		GPIOA->ODR	 |= GPIO_ODR_OD5;
	   		while(GPIOC->IDR & GPIO_IDR_IDR_13);
	   		GPIOA->ODR	 &= ~GPIO_ODR_OD5;
	   		   signal = -1;	//fixed value
	   		   break;
	   }



	   while((radio.connect(uidString) == radio_error) && (errCnt < 3))	//cannot attach or open/connect do mqtt
	   {
//		   Error_Handler();
//		   goto turnOff;
		   errCnt++;
	   }

	   if(errCnt >= 3)
	   {
		   goto disconnect;
	   }


//	   sprintf(logBuff,"{\"raw_moisture\":0,\"battery\":0,\"v_anl\":0,\"v_refint\":0,\"t_pcb\":%d,\"t_uc\":0,\"t_air\":0,\"adc_media\":0,\"adc_comp\":0,\"adc_ref\":0,\"snr\":0,\"rssi\":0,\"ping\":0,\"ts\":%d}", signal, testCounter);
	   sprintf(logBuff,"{\"uid\":\"0x383633393235510200320029\",\"id\":131,\"referencia\":1658,\"umidade\":%d,\"v_bat\":90,\"v_anlarm\":%d,\"t_ntc\":23,\"t_arm\":0,\"v_dgtarm\":0,\"t_ar\":21,\"u_ar\":0.00,\"adc_media\":2409,\"adc_mediaMovel\":0,\"adc_comp\":0,\"airRef\":0,\"waterRef\":0,\"fwVersion\":\"0.0.0\",\"snr\":-63,\"rssi\":%d,\"ping\":0,\"lastData\":\"22.02.2021 15:22:01\"}", testCounter, errCnt, signal);

	   if(radio.publish(logBuff) == radio_error)							//error on publish
	   {
//		   Error_Handler();
		   goto turnOff;
	   }
	   testCounter++;


disconnect:
	   if(radio.disconnect() == radio_error)						//I don't think we're going to have problems here
	   {
//		   Error_Handler();
		   goto turnOff;
	   }


turnOff:
	   radio.turnOff();												//just AT+QPOWD command


	   delay_ms(3600000);

	   radio.turnOn();
	   delay_ms(10000);

	   errCnt = 0;
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
	GPIOA->MODER	|= 	(0b01 << GPIO_MODER_MODE1_Pos)	|
						(0b01 << GPIO_MODER_MODE5_Pos)	;
	GPIOA->ODR		|= 	GPIO_ODR_OD1;

	RCC->AHB1ENR	|=	RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER	|=	(0b00 << GPIO_MODER_MODE13_Pos)	|
						(0b00 << GPIO_MODER_MODE1_Pos)	|
						(0b00 << GPIO_MODER_MODE0_Pos)	;
	GPIOC->PUPDR	|=	(0b00 << GPIO_PUPDR_PUPD13_Pos)	|
						(0b10 << GPIO_PUPDR_PUPD1_Pos)	|
						(0b10 << GPIO_PUPDR_PUPD0_Pos)	;
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
void read_uniqueID(char *uid)
{
	sprintf(uid,"0x%08lx%08lx%08lx", *(volatile uint32_t *)(UID_BASE+8), *(volatile uint32_t *)(UID_BASE+4), *(volatile uint32_t *)(UID_BASE));
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
