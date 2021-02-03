#include "stm32f411xe.h"
#include "stdint.h"
#include "string.h"
#include "main.h"
#include "radio.h"	//because of delay_ms()
#include "bg95.h"
#include "atCommands.h"

eBg95Status_t bg95_transmit(const char *txData, char *rxData, uint32_t timeout, uint32_t txDataSize);
void bg95_sendCmd(const char *txData, uint32_t txDataSize);
void bg95_response(char *response, uint32_t respTimeout, uint8_t isMqttCmd);

char rxBuf[100] = {'\0'};

void bg95_turnOn(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(750);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

void bg95_turnOff(void)
{
	bg95_transmit(AT_QPOWD, rxBuf, CONFIG_TIMEOUT, strlen(AT_QPOWD));
}

void bg95_reset(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(2500);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

void bg95_config(void)
{
	bg95_transmit(AT_ATE0, rxBuf, CONFIG_TIMEOUT, strlen(AT_ATE0));

	bg95_transmit(AT_CPIN, rxBuf, CPIN_TIMEOUT, strlen(AT_CPIN));

	bg95_transmit(AT_QCFG_NWSCANMODE, rxBuf, CONFIG_TIMEOUT, strlen(AT_QCFG_NWSCANMODE));

	bg95_transmit(AT_QCFG_NWSCANSEQ, rxBuf, CONFIG_TIMEOUT, strlen(AT_QCFG_NWSCANSEQ));

	bg95_transmit(AT_QCFG_IOTOPMODE, rxBuf, CONFIG_TIMEOUT, strlen(AT_QCFG_IOTOPMODE));

	bg95_transmit(AT_QCFG_BAND, rxBuf, CONFIG_TIMEOUT, strlen(AT_QCFG_BAND));

	bg95_transmit(AT_QICSGP, rxBuf, CONFIG_TIMEOUT, strlen(AT_QICSGP));
}

void bg95_connect(void)
{
	eBg95Status_t creg = bg95_error, cgreg = bg95_error, cereg = bg95_error;

	bg95_transmit(AT_CFUN0, rxBuf, CFUN_TIMEOUT, strlen(AT_CFUN0));

	delay_ms(5000);

	bg95_transmit(AT_CFUN1, rxBuf, CFUN_TIMEOUT, strlen(AT_CFUN1));

	do
	{
		creg = bg95_transmit(AT_CREG, rxBuf, CONFIG_TIMEOUT, strlen(AT_CREG));

		cgreg = bg95_transmit(AT_CGREG, rxBuf, CONFIG_TIMEOUT, strlen(AT_CGREG));

		cereg = bg95_transmit(AT_CEREG, rxBuf, CONFIG_TIMEOUT, strlen(AT_CEREG));
	}while(	(creg != bg95_ok)	&&
			(cgreg != bg95_ok)	&&
			(cereg != bg95_ok)	);

	bg95_transmit(AT_QNWINFO, rxBuf, CONFIG_TIMEOUT, strlen(AT_QNWINFO));

	bg95_transmit(AT_CSQ, rxBuf, CONFIG_TIMEOUT, strlen(AT_CSQ));

	bg95_transmit(AT_CGATT1, rxBuf, CGATT_TIMEOUT, strlen(AT_CGATT1));

	bg95_transmit(AT_QMTOPEN, rxBuf, MQTT_TIMEOUT, strlen(AT_QMTOPEN));

	bg95_transmit(AT_QMTCONN, rxBuf, MQTT_TIMEOUT, strlen(AT_QMTCONN));
}

void bg95_publish(void)	//const char *msg)
{
	bg95_transmit(AT_QMTPUBEX, rxBuf, MQTT_TIMEOUT, strlen(AT_QMTPUBEX));
}

void bg95_disconnect(void)
{
	bg95_transmit(AT_QMTDISC, rxBuf, MQTT_TIMEOUT, strlen(AT_QMTDISC));

	bg95_transmit(AT_CGATT0, rxBuf, CGATT_TIMEOUT, strlen(AT_CGATT0));
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
