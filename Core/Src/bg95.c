#include "stm32f411xe.h"
#include "stdint.h"
#include "string.h"
#include "main.h"
#include "radio.h"	//because of delay_ms()

void radioModuleResponse(char *response);

void radioModule_turnOn(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(750);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

void radioModule_transmit(const char *txData, char *rxData, uint32_t timeout)
{
	uint8_t i = 0, txDataSize = 0;

	txDataSize = strlen(txData);

	for(i = 0; i< txDataSize; i++)
	{
		while(!(USART2->SR & USART_SR_TXE));
		USART2->DR = *txData;
		txData++;
	}
	radioModuleResponse(rxData);
}

void radioModuleResponse(char *response)
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
