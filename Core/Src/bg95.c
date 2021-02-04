#include "stm32f411xe.h"
#include "stdint.h"
#include "string.h"
#include "main.h"
#include "radio.h"	//because of delay_ms()
#include "bg95.h"
#include "atCommands.h"

eBg95Status_t bg95_sendAtCmd(const char *txData, char *rxData, uint32_t timeout, uint32_t txDataSize);
eBg95Status_t bg95_serialTx(const char *txData, char *rxData, uint32_t timeout, uint32_t txDataSize, uint8_t mqttCmd);
eBg95Status_t bg95_serialRX(char *response, uint32_t respTimeout, uint8_t isMqttCmd);
eBg95Status_t bg95_parseResponse(char *respToParse);

void bg95_turnOn(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(750);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

void bg95_turnOff(void)
{
	char rxBuf[5] = {'\0'};

	bg95_sendAtCmd(AT_QPOWD, rxBuf, CONFIG_TIMEOUT, strlen(AT_QPOWD));
}

void bg95_reset(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(2500);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

#define CONF_RXBUF_SIZE	20
void bg95_config(void)
{
	char rxBuf[CONF_RXBUF_SIZE] = {'\0'};

	bg95_sendAtCmd(AT_ATE0, rxBuf, CONFIG_TIMEOUT, strlen(AT_ATE0));

	bg95_sendAtCmd(AT_CPIN, rxBuf, CPIN_TIMEOUT, strlen(AT_CPIN));

	bg95_sendAtCmd(AT_QCFG_NWSCANMODE, rxBuf, CONFIG_TIMEOUT, strlen(AT_QCFG_NWSCANMODE));

	bg95_sendAtCmd(AT_QCFG_NWSCANSEQ, rxBuf, CONFIG_TIMEOUT, strlen(AT_QCFG_NWSCANSEQ));

	bg95_sendAtCmd(AT_QCFG_IOTOPMODE, rxBuf, CONFIG_TIMEOUT, strlen(AT_QCFG_IOTOPMODE));

	bg95_sendAtCmd(AT_QCFG_BAND, rxBuf, CONFIG_TIMEOUT, strlen(AT_QCFG_BAND));

	bg95_sendAtCmd(AT_QICSGP, rxBuf, CONFIG_TIMEOUT, strlen(AT_QICSGP));

	bg95_sendAtCmd(AT_CFUN0, rxBuf, CFUN_TIMEOUT, strlen(AT_CFUN0));

	delay_ms(5000);

	bg95_sendAtCmd(AT_CFUN1, rxBuf, CFUN_TIMEOUT, strlen(AT_CFUN1));
}

#define CONN_RXBUF_SIZE	100
void bg95_connect(void)
{
	char 			rxBuf[CONN_RXBUF_SIZE] = {'\0'};
	eBg95Status_t 	creg	= bg95_error,
					cgreg	= bg95_error,
					cereg	= bg95_error;

	do
	{
		creg = bg95_sendAtCmd(AT_CREG, rxBuf, CONFIG_TIMEOUT, strlen(AT_CREG));

		cgreg = bg95_sendAtCmd(AT_CGREG, rxBuf, CONFIG_TIMEOUT, strlen(AT_CGREG));

		cereg = bg95_sendAtCmd(AT_CEREG, rxBuf, CONFIG_TIMEOUT, strlen(AT_CEREG));
	}while(	(creg != bg95_ok)	&&
			(cgreg != bg95_ok)	&&
			(cereg != bg95_ok)	);

	bg95_sendAtCmd(AT_QNWINFO, rxBuf, CONFIG_TIMEOUT, strlen(AT_QNWINFO));

	bg95_sendAtCmd(AT_CSQ, rxBuf, CONFIG_TIMEOUT, strlen(AT_CSQ));

	bg95_sendAtCmd(AT_CGATT1, rxBuf, CGATT_TIMEOUT, strlen(AT_CGATT1));

	bg95_sendAtCmd(AT_QMTOPEN, rxBuf, MQTT_TIMEOUT, strlen(AT_QMTOPEN));

	bg95_sendAtCmd(AT_QMTCONN, rxBuf, MQTT_TIMEOUT, strlen(AT_QMTCONN));
}

#define PUB_RXBUF_SIZE	100
void bg95_publish(void)	//const char *msg)
{
	char rxBuf[PUB_RXBUF_SIZE] = {'\0'};

	bg95_sendAtCmd(AT_QMTPUBEX, rxBuf, MQTT_TIMEOUT, strlen(AT_QMTPUBEX));
}

#define DISC_RXBUF_SIZE	20
void bg95_disconnect(void)
{
	char rxBuf[DISC_RXBUF_SIZE] = {'\0'};

	bg95_sendAtCmd(AT_QMTDISC, rxBuf, MQTT_TIMEOUT, strlen(AT_QMTDISC));

	bg95_sendAtCmd(AT_CGATT0, rxBuf, CGATT_TIMEOUT, strlen(AT_CGATT0));
}

eBg95Status_t bg95_sendAtCmd(const char *txData, char *rxData, uint32_t timeout, uint32_t txDataSize)
{
	uint8_t	flagMqttCmd = 0;

	if(memcmp(txData, "AT+QMT", 6) == 0)
	{
		flagMqttCmd = 1;
	}

	bg95_serialTx(txData, rxData, timeout, txDataSize, flagMqttCmd);

	return bg95_parseResponse(rxData);
}

eBg95Status_t bg95_serialTx(const char *txData, char *rxData, uint32_t timeout, uint32_t txDataSize, uint8_t mqttCmd)
{
	uint8_t i = 0;

	for(i = 0; i< txDataSize; i++)
	{
		while(!(USART2->SR & USART_SR_TXE));
		USART2->DR = *txData;
		txData++;
	}

	return bg95_serialRX(rxData, timeout, mqttCmd);
}

eBg95Status_t bg95_serialRX(char *response, uint32_t respTimeout, uint8_t isMqttCmd)
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
			if((bufCount >=3) && (response[bufCount-3] == ',') && (response[bufCount-2] != '\0') && (response[bufCount-1] == ' '))
			{
				response[bufCount - 1] = '\0';
				respACK = 1;
				return bg95_ok;
			}
		}
		else
		{
			if((bufCount >=2) && (response[bufCount-2] == 'O'))
			{
				if((response[bufCount-1] == 'K') || (response[bufCount-1] == 'R'))	//"OK" or "errOR"
				{
					response[bufCount] = '\0';
					respACK = 1;
					return bg95_ok;
				}
			}
		}
		//#TODO what about +CME ERROR: xxx ?
	}while((respACK == 0) && ((HAL_GetTick() - startTick) <= respTimeout));

	return bg95_error;
}

eBg95Status_t bg95_parseResponse(char *respToParse)
{
	char c0, c1;

	c0 = *respToParse++;
	if(c0 == '+')
	{
		if(memcmp(respToParse, "CPIN", 4) == 0)
		{
			asm("nop"); //parser = cpin_parser;
		}
		else if(memcmp(respToParse, "CREG", 4) == 0)
		{
			asm("nop"); //parser = creg_parser;
			if(respToParse[8] == '1')
				return bg95_ok;
		}
		else if(memcmp(respToParse, "CGREG", 5) == 0)
		{
			asm("nop"); //parser = cgreg_parser;
			if(respToParse[9] == '1')
				return bg95_ok;
		}
		else if(memcmp(respToParse, "CEREG", 5) == 0)
		{
			asm("nop"); //parser = cereg_parser;
			if(respToParse[9] == '1')
				return bg95_ok;
		}
		else if(memcmp(respToParse, "CSQ", 3) == 0)
		{
			asm("nop"); //parser = csq_parser;
		}
		else if(memcmp(respToParse, "CME", 3) == 0)
		{
			asm("nop");	//errorHandler;
		}
	}
	else
	{
		c1 = *respToParse++;
		respToParse = respToParse + 2;
		if(memcmp(respToParse, "QMT", 3) == 0)
		{
			asm("nop");	//parser = mqtt_parser (each mqtt cmd?)
		}
		else if((c0 = 'O') && (c1 = 'K'))
		{
			return bg95_ok;
		}
	}

	return bg95_error;	//ERROR
}
