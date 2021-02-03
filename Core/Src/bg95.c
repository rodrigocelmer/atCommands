#include "stm32f411xe.h"
#include "stdint.h"
#include "string.h"
#include "main.h"
#include "radio.h"	//because of delay_ms()
#include "bg95.h"

void bg95_sendCmd(const char *txData, uint32_t txDataSize);
void bg95_response(char *response, uint32_t respTimeout, uint8_t isMqttCmd);

void bg95_turnOn(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(750);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

eBg95Status_t bg95_transmit(const char *txData, char *rxData, uint32_t timeout, uint32_t txDataSize)
{
	char 	c0, c1;
	uint8_t	flagMqttCmd = 0;

	if(memcmp(txData, "AT+QMT", 6) == 0)
	{
		flagMqttCmd = 1;
	}

	bg95_sendCmd(txData, txDataSize);

	bg95_response(rxData, timeout, flagMqttCmd);

	c0 = *rxData++;
	if(c0 == '+')
	{
		if(memcmp(rxData, "CPIN", 4) == 0)
		{
			asm("nop"); //parser = cpin_parser;
		}
		else if(memcmp(rxData, "CREG", 4) == 0)
		{
			asm("nop"); //parser = creg_parser;
			if(rxData[8] == '1')
				return bg95_ok;
		}
		else if(memcmp(rxData, "CGREG", 5) == 0)
		{
			asm("nop"); //parser = cgreg_parser;
			if(rxData[9] == '1')
				return bg95_ok;
		}
		else if(memcmp(rxData, "CEREG", 5) == 0)
		{
			asm("nop"); //parser = cereg_parser;
			if(rxData[9] == '1')
				return bg95_ok;
		}
		else if(memcmp(rxData, "CSQ", 3) == 0)
		{
			asm("nop"); //parser = csq_parser;
		}
	}
	else
	{
		c1 = *rxData++;
		rxData = rxData + 2;
		if(memcmp(rxData, "QMT", 3) == 0)
		{
			asm("nop");	//parser = mqtt_parser (each mqtt cmd?)
		}
		else if((c0 = 'O') && (c1 = 'K'))
		{
			return bg95_ok;
		}
	}

	return bg95_error;
}

void bg95_sendCmd(const char *txData, uint32_t txDataSize)
{
	uint8_t i = 0;

	for(i = 0; i< txDataSize; i++)
	{
		while(!(USART2->SR & USART_SR_TXE));
		USART2->DR = *txData;
		txData++;
	}
}

void bg95_response(char *response, uint32_t respTimeout, uint8_t isMqttCmd)
{
	uint8_t		respACK		= 0;
	uint32_t 	bufCount 	= 0,
				startTick	= HAL_GetTick();

	do
	{
		if(USART2->SR & USART_SR_RXNE)
		{
			USART2->SR &= ~USART_SR_RXNE;
			if((USART2->DR != '\r') && (USART2->DR != '\n'))
			{
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
		}

		if(isMqttCmd)
		{
			if((response[bufCount-3] == ',') && (response[bufCount-2] != '\0') && (response[bufCount-1] == ' '))
			{
				response[bufCount - 1] = '\0';
				respACK = 1;
			}
		}
		else
		{
			if((response[bufCount-2] == 'O') && (response[bufCount-1] == 'K'))
			{
				respACK = 1;
			}
		}

	}while((respACK == 0) && ((HAL_GetTick() - startTick) <= respTimeout));
	asm("nop");
}
