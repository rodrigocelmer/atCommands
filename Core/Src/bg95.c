#include "stm32f411xe.h"
#include "main.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "radio.h"
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
eBg95Status_t parser_serialNumber(char *respToParse);
eBg95Status_t parse_mqttOpen(char *respToParse);
eBg95Status_t parse_mqttConn(char *respToParse);
eBg95Status_t parse_mqttDisc(char *respToParse);
eBg95Status_t parse_mqttSub(char *respToParse);
eBg95Status_t parse_mqttUns(char *respToParse);
eBg95Status_t parse_mqttPubex(char *respToParse);

void breakpoint(void);

void breakpoint(void)
{
	GPIOA->ODR	 |= GPIO_ODR_OD5;
	while(GPIOC->IDR & GPIO_IDR_IDR_13);
	GPIOA->ODR	 &= ~GPIO_ODR_OD5;
}

void bg95_turnOn(void)
{
	char rxBuf[20];

	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(750);
	GPIOA->ODR	 |= GPIO_ODR_OD1;

	  delay_ms(10000);

	bg95_sendAtCmd(AT_ATE0, rxBuf, CONFIG_TIMEOUT, sizeof(AT_ATE0));
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

eRadioStatus_t bg95_getSN(char *serialNumBuf)
{
	if(bg95_sendAtCmd(AT_GSN, serialNumBuf, CONFIG_TIMEOUT, sizeof(AT_GSN)) == bg95_error)
	{
		breakpoint();
		return radio_error;
	}

	return bg95_ok;
}

#define CONF_RXBUF_SIZE	20
eRadioStatus_t bg95_config(void)
{
	char rxBuf[CONF_RXBUF_SIZE] = {'\0'};
//
//	if(bg95_sendAtCmd(AT_ATE0, rxBuf, CONFIG_TIMEOUT, sizeof(AT_ATE0)) != bg95_ok)
//	{
//		breakpoint();
//		return radio_error;
//	}

	if(bg95_sendAtCmd(AT_CPIN, rxBuf, CPIN_TIMEOUT, sizeof(AT_CPIN)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	if(bg95_sendAtCmd(AT_QCFG_NWSCANMODE, rxBuf, CONFIG_TIMEOUT, sizeof(AT_QCFG_NWSCANMODE)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	if(bg95_sendAtCmd(AT_QCFG_NWSCANSEQ, rxBuf, CONFIG_TIMEOUT, sizeof(AT_QCFG_NWSCANSEQ)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	if(bg95_sendAtCmd(AT_QCFG_IOTOPMODE, rxBuf, CONFIG_TIMEOUT, sizeof(AT_QCFG_IOTOPMODE)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	if(bg95_sendAtCmd(AT_QCFG_BAND, rxBuf, CONFIG_TIMEOUT, sizeof(AT_QCFG_BAND)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	if(bg95_sendAtCmd(AT_QICSGP, rxBuf, CONFIG_TIMEOUT, sizeof(AT_QICSGP)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	if(bg95_sendAtCmd(AT_CFUN0, rxBuf, CFUN_TIMEOUT, sizeof(AT_CFUN0)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	delay_ms(5000);

	if(bg95_sendAtCmd(AT_CFUN1, rxBuf, CFUN_TIMEOUT, sizeof(AT_CFUN1)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	return radio_ok;
}

#define CHECK_SIGNAL_RXBUF_SIZE	100
eRadioStatus_t bg95_checkSignal(void)
{
	char 			rxBuf[CHECK_SIGNAL_RXBUF_SIZE] = {'\0'};

	if(bg95_sendAtCmd(AT_QNWINFO, rxBuf, CONFIG_TIMEOUT, sizeof(AT_QNWINFO)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	return bg95_sendAtCmd(AT_CSQ, rxBuf, CONFIG_TIMEOUT, sizeof(AT_CSQ));
//	if(bg95_sendAtCmd(AT_CSQ, rxBuf, CONFIG_TIMEOUT, sizeof(AT_CSQ)) == bg95_error)
//	{
//		breakpoint();
//		return radio_error;
//	}
//
//	return radio_ok;
}

#define CONN_RXBUF_SIZE		20
#define TDR_UID_STRING_SIZE	28
#define QMTCONN_STRING_SIZE	(TDR_UID_STRING_SIZE + 43)
#define NW_REGISTER_TIMEOUT	300000
eRadioStatus_t bg95_connect(char *mcu_uid)
{
	char 			AT_QMTCONN[QMTCONN_STRING_SIZE] = {'\0'};
	char 			rxBuf[CONN_RXBUF_SIZE] = {'\0'};
	uint32_t 		startTick	= HAL_GetTick();

	sprintf(AT_QMTCONN, "AT+QMTCONN=1,\"%s\",\"xzbseimn\",\"ffAc_8Cuxlow\"\r\n", mcu_uid);

	eBg95Status_t 	creg	= bg95_error,
					cgreg	= bg95_error,
					cereg	= bg95_error;

	//#TODO we have to add a timeout here
	do
	{
		creg = bg95_sendAtCmd(AT_CREG, rxBuf, CONFIG_TIMEOUT, sizeof(AT_CREG));

		cgreg = bg95_sendAtCmd(AT_CGREG, rxBuf, CONFIG_TIMEOUT, sizeof(AT_CGREG));

		cereg = bg95_sendAtCmd(AT_CEREG, rxBuf, CONFIG_TIMEOUT, sizeof(AT_CEREG));
	}while(	(creg != bg95_ok)	&&
			(cgreg != bg95_ok)	&&
			(cereg != bg95_ok)	&&
			(HAL_GetTick() - startTick) <= NW_REGISTER_TIMEOUT);

	if(bg95_sendAtCmd(AT_CGATT1, rxBuf, CGATT_TIMEOUT, sizeof(AT_CGATT1)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	if(bg95_sendAtCmd(AT_QMTOPEN, rxBuf, (MQTT_TIMEOUT * 2), sizeof(AT_QMTOPEN)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	if(bg95_sendAtCmd(AT_QMTCONN, rxBuf, MQTT_TIMEOUT, sizeof(AT_QMTCONN)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	return radio_ok;
}

#define PUB_RXBUF_SIZE			100
#define MSG_MAX_SIZE			320
#define QMTPUBEX_STRING_SIZE	(MSG_MAX_SIZE + 83)
eRadioStatus_t bg95_publish(const char *msg)
{
	char rxBuf[PUB_RXBUF_SIZE] = {'\0'};

	char AT_QMTPUBEX[QMTPUBEX_STRING_SIZE] = {'\0'};

	sprintf(AT_QMTPUBEX, "AT+QMTPUBEX=1,1,1,1,\"raks213/central/sensor\",\"%s\"\r\n", msg);


	if(bg95_sendAtCmd(AT_QMTPUBEX, rxBuf, MQTT_TIMEOUT, sizeof(AT_QMTPUBEX)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	return radio_ok;
}

#define DISC_RXBUF_SIZE	20
eRadioStatus_t bg95_disconnect(void)
{
	char rxBuf[DISC_RXBUF_SIZE] = {'\0'};

	if(bg95_sendAtCmd(AT_QMTDISC, rxBuf, MQTT_TIMEOUT, sizeof(AT_QMTDISC)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	if(bg95_sendAtCmd(AT_QIDEACT, rxBuf, QIDEACT_TIMEOUT, sizeof(AT_QIDEACT)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	if(bg95_sendAtCmd(AT_CGATT0, rxBuf, CGATT_TIMEOUT, sizeof(AT_CGATT0)) != bg95_ok)
	{
		breakpoint();
		return radio_error;
	}

	return radio_ok;
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
		breakpoint();
		return bg95_timeout;
	}

	return bg95_parseResponse(rxData);
}

eBg95Status_t bg95_serialTx(const char *txData, char *rxData, uint32_t timeout, uint32_t txDataSize, uint8_t mqttCmd)
{
	while(*txData != '\0')
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
		else
		{
			breakpoint();
			return bg95_cmdNotRegistred;
		}
	}
	else
	{
		c1 = *respToParse++;
//		respToParse = respToParse + 2;
		if(memcmp(respToParse, " +QMT", 5) == 0)
		{
			respToParse = respToParse + 5;
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
			else
			{
				breakpoint();
				return bg95_cmdNotRegistred;
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
			breakpoint();
			return bg95_error;
		}
		else if(((c0 >= '0') && (c0 <= '9'))	&&
				((c1 >= '0') && (c1 <= '9'))	)
		{
			respToParse = respToParse - 2;
			parser = parser_serialNumber;
		}
		else
		{
			breakpoint();
			return bg95_cmdNotRegistred;
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
	breakpoint();
	return bg95_error;
}

eBg95Status_t parse_creg(char *respToParse)
{
	if(respToParse[8] == '1')	//CREG: 0,[8]2 OK
	{
		return bg95_ok;
	}
	breakpoint();
	return bg95_error;
}

eBg95Status_t parse_cgreg(char *respToParse)
{
	if(respToParse[9] == '1')	//CGREG: 0,[9]2 OK
	{
		return bg95_ok;
	}
	breakpoint();
	return bg95_error;
}

eBg95Status_t parse_cereg(char *respToParse)
{
	if(respToParse[9] == '1')	//CEREG: 0,[9]2 OK
	{
		return bg95_ok;
	}
	breakpoint();
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
	breakpoint();
	return bg95_error;
}

eBg95Status_t parse_cme(char *respToParse)
{
	//#TODO error handler
	breakpoint();
}

#define SN_SIZE			15
#define	PARSER_BUF_SIZE	18
eBg95Status_t parser_serialNumber(char *respToParse)
{
	uint8_t i = 0;

	for(i = SN_SIZE; i < PARSER_BUF_SIZE; i++)
	{
		respToParse[i] = '\0';
	}

	return bg95_ok;
}
eBg95Status_t parse_mqttOpen(char *respToParse)
{
	if(respToParse[8] == '0')	//OPEN: 1,[8]0
	{
		return bg95_ok;
	}
	breakpoint();
	return bg95_error;
}

eBg95Status_t parse_mqttConn(char *respToParse)
{
	if(respToParse[10] == '0')	//CONN: 1,0,[10]0
	{
		return bg95_ok;
	}
	breakpoint();
	return bg95_error;
}

eBg95Status_t parse_mqttDisc(char *respToParse)
{
	if(respToParse[8] == '0')	//DISC: 1,[8]0
	{
		return bg95_ok;
	}
	breakpoint();
	return bg95_error;
}

eBg95Status_t parse_mqttSub(char *respToParse)
{
	if(respToParse[9] != '2')	//SUB: 1,1,0,1 +QMTRECV: 1,1,"topic","msg"
	{
		return bg95_ok;
	}
	breakpoint();
	return bg95_error;
}

eBg95Status_t parse_mqttUns(char *respToParse)
{
	if(respToParse[9] != '2')	//UNS: 1,1,[9]0
	{
		return bg95_ok;
	}
	breakpoint();
	return bg95_error;
}

eBg95Status_t parse_mqttPubex(char *respToParse)
{
	if(respToParse[9] != '2')	//PUB: 1,3,[9]0
	{
		return bg95_ok;
	}
	breakpoint();
	return bg95_error;
}
