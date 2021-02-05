#include "stm32f411xe.h"
#include "stdint.h"
#include "string.h"
#include "main.h"
#include "radio.h"	//because of delay_ms()
#include "bg95.h"
#include "atCommands.h"

typedef eBg95Status_t (*parseFunction)(char *respToParse);

eBg95Status_t bg95_sendAtCmd(const char *txData, char *rxData, uint32_t timeout, uint32_t txDataSize);
eBg95Status_t bg95_serialTx(const char *txData, char *rxData, uint32_t timeout, uint32_t txDataSize, uint8_t mqttCmd);
eBg95Status_t bg95_serialRX(char *response, uint32_t respTimeout, uint8_t isMqttCmd);
eBg95Status_t bg95_parseResponse(char *respToParse);
eBg95Status_t parse_cpin(char *respToParse);
eBg95Status_t parse_creg(char *respToParse);
eBg95Status_t parse_cgreg(char *respToParse);
eBg95Status_t parse_cereg(char *respToParse);
eBg95Status_t parse_csq(char *respToParse);
eBg95Status_t parse_cme(char *respToParse);
eBg95Status_t parse_mqttOpen(char *respToParse);
eBg95Status_t parse_mqttConn(char *respToParse);
eBg95Status_t parse_mqttDisc(char *respToParse);
eBg95Status_t parse_mqttSub(char *respToParse);
eBg95Status_t parse_mqttUns(char *respToParse);
eBg95Status_t parse_mqttPubex(char *respToParse);

void bg95_turnOn(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(750);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

void bg95_turnOff(void)
{
	char rxBuf[5] = {'\0'};

	if(bg95_sendAtCmd(AT_QPOWD, rxBuf, CONFIG_TIMEOUT, sizeof(AT_QPOWD)) != bg95_ok)
	{
		while(1);
	}
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

	if(bg95_sendAtCmd(AT_ATE0, rxBuf, CONFIG_TIMEOUT, sizeof(AT_ATE0)) != bg95_ok)
	{
		while(1);
	}

	if(bg95_sendAtCmd(AT_CPIN, rxBuf, CPIN_TIMEOUT, sizeof(AT_CPIN)) != bg95_ok)
	{
		while(1);
	}

	if(bg95_sendAtCmd(AT_QCFG_NWSCANMODE, rxBuf, CONFIG_TIMEOUT, sizeof(AT_QCFG_NWSCANMODE)) != bg95_ok)
	{
		while(1);
	}

	if(bg95_sendAtCmd(AT_QCFG_NWSCANSEQ, rxBuf, CONFIG_TIMEOUT, sizeof(AT_QCFG_NWSCANSEQ)) != bg95_ok)
	{
		while(1);
	}

	if(bg95_sendAtCmd(AT_QCFG_IOTOPMODE, rxBuf, CONFIG_TIMEOUT, sizeof(AT_QCFG_IOTOPMODE)) != bg95_ok)
	{
		while(1);
	}

	if(bg95_sendAtCmd(AT_QCFG_BAND, rxBuf, CONFIG_TIMEOUT, sizeof(AT_QCFG_BAND)) != bg95_ok)
	{
		while(1);
	}

	if(bg95_sendAtCmd(AT_QICSGP, rxBuf, CONFIG_TIMEOUT, sizeof(AT_QICSGP)) != bg95_ok)
	{
		while(1);
	}

	if(bg95_sendAtCmd(AT_CFUN0, rxBuf, CFUN_TIMEOUT, sizeof(AT_CFUN0)) != bg95_ok)
	{
		while(1);
	}

	delay_ms(5000);

	if(bg95_sendAtCmd(AT_CFUN1, rxBuf, CFUN_TIMEOUT, sizeof(AT_CFUN1)) != bg95_ok)
	{
		while(1);
	}
}

#define CONN_RXBUF_SIZE	100
void bg95_connect(void)
void bg95_connect(char *mcu_uid, uint32_t uidSize)
{
	char 			rxBuf[CONN_RXBUF_SIZE] = {'\0'};
	eBg95Status_t 	creg	= bg95_error,
					cgreg	= bg95_error,
					cereg	= bg95_error;

	do
	{
		creg = bg95_sendAtCmd(AT_CREG, rxBuf, CONFIG_TIMEOUT, sizeof(AT_CREG));

		cgreg = bg95_sendAtCmd(AT_CGREG, rxBuf, CONFIG_TIMEOUT, sizeof(AT_CGREG));

		cereg = bg95_sendAtCmd(AT_CEREG, rxBuf, CONFIG_TIMEOUT, sizeof(AT_CEREG));
	}while(	(creg != bg95_ok)	&&
			(cgreg != bg95_ok)	&&
			(cereg != bg95_ok)	);

	if(bg95_sendAtCmd(AT_QNWINFO, rxBuf, CONFIG_TIMEOUT, sizeof(AT_QNWINFO)) != bg95_ok)
	{
		while(1);
	}

	if(bg95_sendAtCmd(AT_CSQ, rxBuf, CONFIG_TIMEOUT, sizeof(AT_CSQ)) == bg95_error)
	{
		while(1);
	}

	if(bg95_sendAtCmd(AT_CGATT1, rxBuf, CGATT_TIMEOUT, sizeof(AT_CGATT1)) != bg95_ok)
	{
		while(1);
	}

	if(bg95_sendAtCmd(AT_QMTOPEN, rxBuf, MQTT_TIMEOUT, sizeof(AT_QMTOPEN)) != bg95_ok)
	{
		while(1);
	}

	if(bg95_sendAtCmd(AT_QMTCONN, rxBuf, MQTT_TIMEOUT, sizeof(AT_QMTCONN)) != bg95_ok)
	{
		while(1);
	}
}

#define PUB_RXBUF_SIZE	100
void bg95_publish(void)	//const char *msg)
{
	char rxBuf[PUB_RXBUF_SIZE] = {'\0'};

	if(bg95_sendAtCmd(AT_QMTPUBEX, rxBuf, MQTT_TIMEOUT, sizeof(AT_QMTPUBEX)) != bg95_ok)
	{
		while(1);
	}
}

#define DISC_RXBUF_SIZE	20
void bg95_disconnect(void)
{
	char rxBuf[DISC_RXBUF_SIZE] = {'\0'};

	if(bg95_sendAtCmd(AT_QMTDISC, rxBuf, MQTT_TIMEOUT, sizeof(AT_QMTDISC)) != bg95_ok)
	{
		while(1);
	}

	if(bg95_sendAtCmd(AT_CGATT0, rxBuf, CGATT_TIMEOUT, sizeof(AT_CGATT0)) != bg95_ok)
	{
		while(1);
	}
}

eBg95Status_t bg95_sendAtCmd(const char *txData, char *rxData, uint32_t timeout, uint32_t txDataSize)
{
	uint8_t	flagMqttCmd = 0;

	if(memcmp(txData, "AT+QMT", 6) == 0)
	{
		flagMqttCmd = 1;
	}

	if(bg95_serialTx(txData, rxData, timeout, txDataSize, flagMqttCmd) == bg95_timeout)
	{
		return bg95_timeout;
	}

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

			if(isMqttCmd)
			{
				if(bufCount > 10)	//it is safe to check for MQTT response after receiving 10 chars
				{
					if((response[bufCount-3] == ',') && (response[bufCount-2] != '\0') && (response[bufCount-1] == ' '))
					{
						response[bufCount - 1] = '\0';
						return bg95_ok;
					}
				}
			}
//			else if(bufCount >=2)
//			{
//				if(	(memcmp((void *)&response[bufCount-2], "OK", 2) == 0)	||
//					(memcmp((void *)&response[bufCount-2], "OR", 2) == 0)	);
//				{
			else if((bufCount >=2) && (response[bufCount-2] == 'O'))
			{
				if((response[bufCount-1] == 'K') || (response[bufCount-1] == 'R'))	//"OK" or "errOR"
				{
					response[bufCount] = '\0';
					return bg95_ok;
				}
			}
		}
		//#TODO what about +CME ERROR: xxx ?
	}while((HAL_GetTick() - startTick) <= respTimeout);

	return bg95_timeout;
}

eBg95Status_t bg95_parseResponse(char *respToParse)
{
	parseFunction	parser;
	char c0, c1;

	c0 = *respToParse++;
	if(c0 == '+')
	{
		if(memcmp(respToParse, "CPIN", 4) == 0)
		{
			parser = parse_cpin;
		}
		else if(memcmp(respToParse, "CREG", 4) == 0)
		{
			parser = parse_creg;
		}
		else if(memcmp(respToParse, "CGREG", 5) == 0)
		{
			parser = parse_cgreg;
		}
		else if(memcmp(respToParse, "CEREG", 5) == 0)
		{
			parser = parse_cereg;
		}
		else if(memcmp(respToParse, "CSQ", 3) == 0)
		{
			parser = parse_csq;
		}
		else if(memcmp(respToParse, "CME", 3) == 0)
		{
			parser = parse_cme;
		}
		else if(memcmp(respToParse, "QNWINFO", 7) == 0)	//#TODO
		{
			return bg95_ok;
		}
	}
	else
	{
		c1 = *respToParse++;
		respToParse = respToParse + 2;
		if(memcmp(respToParse, "QMT", 3) == 0)
		{
			respToParse = respToParse + 3;
			if(memcmp(respToParse, "OPEN", 4) == 0)
			{
				parser = parse_mqttOpen;
			}
			else if(memcmp(respToParse, "CONN", 4) == 0)
			{
				parser = parse_mqttConn;
			}
			else if(memcmp(respToParse, "DISC", 4) == 0)
			{
				parser = parse_mqttDisc;
			}
			else if(memcmp(respToParse, "SUB", 3) == 0)
			{
				parser = parse_mqttSub;
			}
			else if(memcmp(respToParse, "UNS", 3) == 0)
			{
				parser = parse_mqttUns;
			}
			else if(memcmp(respToParse, "PUB", 3) == 0)
			{
				parser = parse_mqttPubex;
			}
		}
		else if((c0 == 'O') && (c1 == 'K'))
		{
			return bg95_ok;
		}
		else if((c0 == 'A') && (c1 == 'T'))	//ATE0
		{
			return bg95_ok;
		}
		else if((c0 == 'E') && (c1 == 'R'))	//ERROR
		{
			return bg95_error;
		}
	}

	return parser(respToParse);
}

eBg95Status_t parse_cpin(char *respToParse)
{
	if(respToParse[6] == 'R')	//CPIN: [6]READY OK
	{
		return bg95_ok;
	}
	return bg95_error;
}

eBg95Status_t parse_creg(char *respToParse)
{
	if(respToParse[8] == '1')	//CREG: 0,[8]2 OK
	{
		return bg95_ok;
	}
	return bg95_error;
}

eBg95Status_t parse_cgreg(char *respToParse)
{
	if(respToParse[9] == '1')	//CGREG: 0,[9]2 OK
	{
		return bg95_ok;
	}
	return bg95_error;
}

eBg95Status_t parse_cereg(char *respToParse)
{
	if(respToParse[9] == '1')	//CEREG: 0,[9]2 OK
	{
		return bg95_ok;
	}
	return bg95_error;
}

eBg95Status_t parse_csq(char *respToParse)
{
	//CSQ: [5]2[6]6,99 OK
	if((respToParse[5] == '9') && (respToParse[6] == '9'))
	{
		return csq_noSignal;
	}
	else if((respToParse[5] <= '1') && (respToParse[6] <= '7'))
	{
		return csq_poorSignal;
	}
	else if((respToParse[5] >= '3') && (respToParse[6] >= '1'))
	{
		return csq_greatSignal;
	}
	else
	{
		return csq_goodSignal;
	}
	return bg95_error;
}

eBg95Status_t parse_cme(char *respToParse)
{
	//#TODO error handler
}

eBg95Status_t parse_mqttOpen(char *respToParse)
{
	if(respToParse[8] == '0')	//OPEN: 1,[8]0
	{
		return bg95_ok;
	}
	return bg95_error;
}

eBg95Status_t parse_mqttConn(char *respToParse)
{
	if(respToParse[10] == '0')	//CONN: 1,0,[10]0
	{
		return bg95_ok;
	}
	return bg95_error;
}

eBg95Status_t parse_mqttDisc(char *respToParse)
{
	if(respToParse[8] == '0')	//DISC: 1,[8]0
	{
		return bg95_ok;
	}
	return bg95_error;
}

eBg95Status_t parse_mqttSub(char *respToParse)
{
	if(respToParse[9] != '2')	//SUB: 1,1,0,1 +QMTRECV: 1,1,"topic","msg"
	{
		return bg95_ok;
	}
	return bg95_error;
}

eBg95Status_t parse_mqttUns(char *respToParse)
{
	if(respToParse[9] != '2')	//UNS: 1,1,[9]0
	{
		return bg95_ok;
	}
	return bg95_error;
}

eBg95Status_t parse_mqttPubex(char *respToParse)
{
	if(respToParse[9] != '2')	//PUB: 1,3,[9]0
	{
		return bg95_ok;
	}
	return bg95_error;
}
