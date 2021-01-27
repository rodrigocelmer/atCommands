#include "stm32f411xe.h"
#include "stdint.h"
#include "string.h"
#include "main.h"

void radioResponse(char *response);

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

void radio_transmit(const char *txData, char *rxData)
{
	uint8_t i = 0, txDataSize = 0;

	txDataSize = strlen(txData);

	for(i = 0; i< txDataSize; i++)
	{
		while(!(USART2->SR & USART_SR_TXE));
		USART2->DR = *txData;
		txData++;
	}
	radioResponse(rxData);
}

void radioResponse(char *response)
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
				response[bufCount] = USART2->DR;
				bufCount++;
			}
			else
			{
				if((bufCount != 0) && (response[bufCount - 1] != ' '))
				{
					response[bufCount] = ' ';
					bufCount++;
				}
			}
			actualTick = HAL_GetTick();
			startTick = actualTick;
		}
		else
		{
			if(actualTick - startTick <= 1000)
			{
				actualTick = HAL_GetTick();
			}
			else
			{
				response[bufCount - 1] = '\0';
				timeout = 1;
			}
		}
	}
}
