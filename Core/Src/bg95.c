#include "stm32f411xe.h"
#include "stdint.h"
#include "string.h"
#include "main.h"
#include "radio.h"	//because of delay_ms()
#include "bg95.h"

//AT commands
const char AT[]					= {"AT\r\n\0"};															//resp: OK
const char QPOWD[] 				= {"AT+QPOWD\r\n\0"};													//resp: OK POWERED DOWN
const char CPIN[]				= {"AT+CPIN?\r\n\0"};													//resp: +CPIN: [7]R[8]EADY OK
const char QCFG_NWSCANMODE[]	= {"AT+QCFG=\"NWSCANMODE\",0\r\n\0"};									//resp: OK
const char QCFG_NWSCANSEQ[]		= {"AT+QCFG=\"NWSCANSEQ\",020103\r\n\0"};								//resp: OK
const char QCFG_IOTOPMODE[]		= {"AT+QCFG=\"IOTOPMODE\",2\r\n\0"};									//resp: OK
const char QCFG_BAND[]			= {"AT+QCFG=\"BAND\",0XF,0,0X8000004\r\n\0"};							//resp: OK
const char QICSGP[]				= {"AT+QICSGP=1,1,\"virtueyes.com.br\",\"virtu\",\"virtu\",3\r\n\0"};	//resp: OK
const char CFUN0[]				= {"AT+CFUN=0\r\n\0"};													//resp: OK
const char CFUN1[]				= {"AT+CFUN=1\r\n\0"};													//resp: OK
const char CREG[]				= {"AT+CREG?\r\n\0"};													//resp: +CREG: 0,[9]2
const char CGREG[]				= {"AT+CGREG?\r\n\0"};													//resp: +CGREG: 0,[10]4
const char CEREG[]				= {"AT+CEREG?\r\n\0"};													//resp: +CEREG: 0,[10]2
const char CGATT0[]				= {"AT+CGATT=0\r\n\0"};													//resp: OK
const char CGATT1[]				= {"AT+CGATT=1\r\n\0"};													//resp: OK
const char CSQ[]				= {"AT+CSQ\r\n\0"};														//resp: +CSQ: [6]2[7]6,99
const char ATE0[]				= {"ATE0\r\n\0"};														//resp:
const char QNWINFO[]			= {"AT+QNWINFO\r\n\0"};													//resp: +QNWINFO: "GPRS","72405,"GSM 1800", 594

//MQTT commands
const char QMTOPEN[]			= {"AT+QMTOPEN=1,\"tailor.cloudmqtt.com\",13291\r\n\0"};				//resp: OK +QMTOPEN: [13]1,[15]0
const char QMTCLOSE[]			= {"AT+QMTCLOSE=1\r\n\0"};												//resp:
const char QMTCONN[]			= {"AT+QMTCONN=1,\"usrCelmer\",\"zgxbgfsy\",\"H7Mnnfi0_2rk\"\r\n\0"};	//resp: OK +QMTCONN: [13]1,[15]0,[17]0
const char QMTDISC[]			= {"AT+QMTDISC=1\r\n\0"};												//resp: OK +QMTDISC: [13]1,[15]0
const char QMTSUB[]				= {"AT+QMTSUB=1,1,\"celmer\",1\r\n\0"};									//resp: OK +QMTSUB: [12]1,[14]1,[16]0,[18]1 +QMTRECV: 1,1,"topic","msg"
const char QMTUNS[]				= {"AT+QMTUNS=1,1,\"celmer\"\r\n\0"};									//resp: OK +QMTUNS: [12]1,[14]1,[16]0
const char QMTPUBEX[]			= {"AT+QMTPUBEX=1,1,1,1,\"celmer\",\"something else\"\r\n\0"};		//resp: OK +QMTPUB: [12]1,[14]3,[16]0

void bg95_sendCmd(const char *txData, uint32_t txDataSize);
uint32_t bg95_response(eAtCmd_t cmdSent, char *response, uint32_t respTimeout);
eBg95Status_t bg95_parser(eAtCmd_t cmdToParse, const char *respToParse, uint32_t respSize);

void bg95_turnOn(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(750);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

eBg95Status_t bg95_transmit(eAtCmd_t atCmd, char*rxData)
{
	uint32_t timeout = 0, responseDataSize = 0;
	eBg95Status_t	debug;

	switch(atCmd)
	{
		case AT_AT:
			bg95_sendCmd(AT, strlen(AT));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QPOWD:
			bg95_sendCmd(QPOWD, strlen(QPOWD));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_CPIN:
			bg95_sendCmd(CPIN, strlen(CPIN));
			timeout = CPIN_TIMEOUT;
			break;
		case AT_QCFG_NWSCANMODE:
			bg95_sendCmd(QCFG_NWSCANMODE, strlen(QCFG_NWSCANMODE));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QCFG_NWSCANSEQ:
			bg95_sendCmd(QCFG_NWSCANSEQ, strlen(QCFG_NWSCANSEQ));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QCFG_IOTOPMODE:
			bg95_sendCmd(QCFG_IOTOPMODE, strlen(QCFG_IOTOPMODE));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QCFG_BAND:
			bg95_sendCmd(QCFG_BAND, strlen(QCFG_BAND));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QICSGP:
			bg95_sendCmd(QICSGP, strlen(QICSGP));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_CFUN0:
			bg95_sendCmd(CFUN0, strlen(CFUN0));
			timeout = CFUN_TIMEOUT;
			break;
		case AT_CFUN1:
			bg95_sendCmd(CFUN1, strlen(CFUN1));
			timeout = CFUN_TIMEOUT;
			break;
		case AT_CREG:
			bg95_sendCmd(CREG, strlen(CREG));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_CGREG:
			bg95_sendCmd(CGREG, strlen(CGREG));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_CEREG:
			bg95_sendCmd(CEREG, strlen(CEREG));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_CGATT0:
			bg95_sendCmd(CGATT0, strlen(CGATT0));
			timeout = CGATT_TIMEOUT;
			break;
		case AT_CGATT1:
			bg95_sendCmd(CGATT1, strlen(CGATT1));
			timeout = CGATT_TIMEOUT;
			break;
		case AT_CSQ:
			bg95_sendCmd(CSQ, strlen(CSQ));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_ATE0:
			bg95_sendCmd(ATE0, strlen(ATE0));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QNWINFO:
			bg95_sendCmd(QNWINFO, strlen(QNWINFO));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QMTOPEN:
			bg95_sendCmd(QMTOPEN, strlen(QMTOPEN));
			timeout = MQTT_TIMEOUT;
			break;
		case AT_QMTCLOSE:
			bg95_sendCmd(QMTCLOSE, strlen(QMTCLOSE));
			timeout = MQTT_TIMEOUT;
			break;
		case AT_QMTCONN:
			bg95_sendCmd(QMTCONN, strlen(QMTCONN));
			timeout = MQTT_TIMEOUT;
			break;
		case AT_QMTDISC:
			bg95_sendCmd(QMTDISC, strlen(QMTDISC));
			timeout = MQTT_TIMEOUT;
			break;
		case AT_QMTSUB:
			bg95_sendCmd(QMTSUB, strlen(QMTSUB));
			timeout = MQTT_TIMEOUT;
			break;
		case AT_QMTUNS:
			bg95_sendCmd(QMTUNS, strlen(QMTUNS));
			timeout = MQTT_TIMEOUT;
			break;
		case AT_QMTPUBEX:
			bg95_sendCmd(QMTPUBEX, strlen(QMTPUBEX));
			timeout = MQTT_TIMEOUT;
			break;
		default:
			//
			break;
	}

	responseDataSize = bg95_response(atCmd, rxData, timeout);
	if(atCmd == AT_ATE0)
		return radio_ok;
	debug = bg95_parser(atCmd, rxData, responseDataSize);
	return debug;
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

uint32_t bg95_response(eAtCmd_t cmdSent, char *response, uint32_t respTimeout)
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

		if(	(cmdSent == AT_QMTOPEN)	||
			(cmdSent == AT_QMTCLOSE)||
			(cmdSent == AT_QMTCONN)	||
			(cmdSent == AT_QMTDISC)	||
			(cmdSent == AT_QMTSUB)	||
			(cmdSent == AT_QMTUNS)	||
			(cmdSent == AT_QMTPUBEX))
		{
			if(memcmp(response, "OK +QMT", 7) == 0)
			{
				if((response[bufCount-3] == ',') && (response[bufCount-2] != '\0') && (response[bufCount-1] == ' '))
				{
					response[bufCount - 1] = '\0';
					respACK = 1;
				}
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
	return (bufCount - 1);
}

eBg95Status_t bg95_parser(eAtCmd_t cmdToParse, const char *respToParse, uint32_t respSize)
{
	switch(cmdToParse)
	{
		case AT_CPIN:		//+CPIN: [7]R[8]EADY OK
			if(respToParse[7] == 'R')
			{
				return bg95_ok;
			}
			break;
		case AT_CREG:		//+CREG: 0,[9]2
			if((respToParse[9] == '1') || (respToParse[9] == '5'))
			{
				return bg95_ok;
			}
			break;
		case AT_CGREG:		//+CGREG: 0,[10]4
			if((respToParse[10] == '1') || (respToParse[10] == '5'))
			{
				return bg95_ok;
			}
			break;
		case AT_CEREG:		//+CEREG: 0,[10]2
			if((respToParse[10] == '1') || (respToParse[10] == '5'))
			{
				return bg95_ok;
			}
			break;
		case AT_CSQ:		//+CSQ: [6]2[7]6,99 - does this work? kkkkk
			if((respToParse[6] == '9') && (respToParse[7] <= '9'))
			{
				return csq_noSignal;
			}
			else
			{
				if((respToParse[6] == '1') && (respToParse[7] <= '7'))
				{
					return csq_poorSignal;
				}
				else
				{
					if((respToParse[6] >= '3') && (respToParse[7] >= '1'))
					{
						return csq_greatSignal;
					}
					else
					{
						return csq_goodSignal;
					}
				}
			}
			break;
		case AT_QNWINFO:	//+QNWINFO: "GPRS","72405,"GSM 1800", 594
			//#TODO ?
			break;
		case AT_QMTOPEN:	//OK +QMTOPEN: [13]1,[15]0
			if(respToParse[15] == '0')
			{
				return bg95_ok;
			}
			break;
		case AT_QMTCONN:	//OK +QMTCONN: [13]1,[15]0,[17]0
			if(respToParse[17] == '0')
			{
				return bg95_ok;
			}
			break;
		case AT_QMTDISC:	//OK +QMTDISC: [13]1,[15]0
			if(respToParse[15] == '0')
			{
				return bg95_ok;
			}
			break;
		case AT_QMTSUB:		//OK +QMTSUB: [12]1,[14]1,[16]0,[18]1 +QMTRECV: 1,1,"topic","msg"	#TODO msg return
			if(respToParse[16] != '2')
			{
				return bg95_ok;
			}
			break;
		case AT_QMTUNS:		//OK +QMTUNS: [12]1,[14]1,[16]0
			if(respToParse[16] != '2')
			{
				return bg95_ok;
			}
			break;
		case AT_QMTPUBEX:	//OK +QMTPUB: [12]1,[14]3,[16]0
			if(respToParse[14] != '2')
			{
				return bg95_ok;
			}
			break;
		default:
			if((respToParse[0] == 'O') && (respToParse[1] == 'K'))
			{
				return bg95_ok;
			}
			break;
	}

	return bg95_error;
}
