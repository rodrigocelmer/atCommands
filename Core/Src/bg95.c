#include "stm32f411xe.h"
#include "stdint.h"
#include "string.h"
#include "main.h"
#include "radio.h"	//because of delay_ms()
#include "bg95.h"

//AT commands
const char AT[]					= {"AT\r\n\0"};															//resp: OK
const char QPOWD[] 				= {"AT+QPOWD\r\n\0"};													//resp: OK\r\nPOWERED DOWN
const char CPIN[]				= {"AT+CPIN?\r\n\0"};													//resp: +CPIN: READY
const char QCFG_NWSCANMODE[]	= {"AT+QCFG=\"NWSCANMODE\",0\r\n\0"};									//resp: OK
const char QCFG_NWSCANSEQ[]		= {"AT+QCFG=\"NWSCANSEQ\",020103\r\n\0"};								//resp: OK
const char QCFG_IOTOPMODE[]		= {"AT+QCFG=\"IOTOPMODE\",2\r\n\0"};									//resp: OK
const char QCFG_BAND[]			= {"AT+QCFG=\"BAND\",0XF,0,0X8000004\r\n\0"};							//resp: OK
const char QICSGP[]				= {"AT+QICSGP=1,1,\"virtueyes.com.br\",\"virtu\",\"virtu\",3\r\n\0"};	//resp: OK
const char CFUN0[]				= {"AT+CFUN=0\r\n\0"};													//resp: OK
const char CFUN1[]				= {"AT+CFUN=1\r\n\0"};													//resp: OK
const char CREG[]				= {"AT+CREG?\r\n\0"};													//resp: +CREG: 0,2
const char CGREG[]				= {"AT+CGREG?\r\n\0"};													//resp: +CGREG: 0,4
const char CEREG[]				= {"AT+CEREG?\r\n\0"};													//resp: +CEREG: 0,2
const char CGATT0[]				= {"AT+CGATT=0\r\n\0"};													//resp: OK
const char CGATT1[]				= {"AT+CGATT=1\r\n\0"};													//resp: OK
const char CSQ[]				= {"AT+CSQ\r\n\0"};														//resp: +CSQ: 26,99
const char ATE0[]				= {"ATE0\r\n\0"};														//resp:
const char QNWINFO[]			= {"AT+QNWINFO\r\n\0"};													//resp: +QNWINFO: "GPRS","72405,"GSM 1800", 594

//MQTT commands
const char QMTOPEN[]			= {"AT+QMTOPEN=1,\"tailor.cloudmqtt.com\",13291\r\n\0"};				//resp: OK\r\n+QMTOPEN: 1,0
const char QMTCLOSE[]			= {"AT+QMTCLOSE=1\r\n\0"};												//resp:
const char QMTCONN[]			= {"AT+QMTCONN=1,\"usrCelmer\",\"zgxbgfsy\",\"H7Mnnfi0_2rk\"\r\n\0"};	//resp: OK\r\n+QMTCONN: 1,0,0
const char QMTDISC[]			= {"AT+QMTDISC=1\r\n\0"};												//resp: OK\r\n+QMTDISC: 1,0
const char QMTSUB[]				= {"AT+QMTSUB=1,1,\"celmer\",1\r\n\0"};									//resp: OK\r\n+QMTSUB: 1,1,0,1\r\n+QMTRECV: 1,1,"topic","msg"
const char QMTUNS[]				= {"AT+QMTUNS=1,1,\"celmer\"\r\n\0"};									//resp: OK\r\n+QMTUNS: 1,1,0
const char QMTPUBEX[]			= {"AT+QMTPUBEX=1,1,1,1,\"celmer\",\"fall to pieces\"\r\n\0"};			//resp: OK\r\n+QMTPUB: 1,3,0

void radioModule_sendCmd(const char *txData, uint32_t txDataSize);
uint32_t radioModule_response(eAtCmd_t cmdSent, char *response, uint32_t respTimeout);
void radioModule_parser(const char *respToParse, uint32_t respSize);

void radioModule_turnOn(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(750);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

void radioModule_transmit(eAtCmd_t atCmd, char*rxData)
{
	uint32_t timeout = 0, responseDataSize = 0;

	switch(atCmd)
	{
		case AT_AT:
			radioModule_sendCmd(AT, strlen(AT));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QPOWD:
			radioModule_sendCmd(QPOWD, strlen(QPOWD));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_CPIN:
			radioModule_sendCmd(CPIN, strlen(CPIN));
			timeout = CPIN_TIMEOUT;
			break;
		case AT_QCFG_NWSCANMODE:
			radioModule_sendCmd(QCFG_NWSCANMODE, strlen(QCFG_NWSCANMODE));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QCFG_NWSCANSEQ:
			radioModule_sendCmd(QCFG_NWSCANSEQ, strlen(QCFG_NWSCANSEQ));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QCFG_IOTOPMODE:
			radioModule_sendCmd(QCFG_IOTOPMODE, strlen(QCFG_IOTOPMODE));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QCFG_BAND:
			radioModule_sendCmd(QCFG_BAND, strlen(QCFG_BAND));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QICSGP:
			radioModule_sendCmd(QICSGP, strlen(QICSGP));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_CFUN0:
			radioModule_sendCmd(CFUN0, strlen(CFUN0));
			timeout = CFUN_TIMEOUT;
			break;
		case AT_CFUN1:
			radioModule_sendCmd(CFUN1, strlen(CFUN1));
			timeout = CFUN_TIMEOUT;
			break;
		case AT_CREG:
			radioModule_sendCmd(CREG, strlen(CREG));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_CGREG:
			radioModule_sendCmd(CGREG, strlen(CGREG));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_CEREG:
			radioModule_sendCmd(CEREG, strlen(CEREG));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_CGATT0:
			radioModule_sendCmd(CGATT0, strlen(CGATT0));
			timeout = CGATT_TIMEOUT;
			break;
		case AT_CGATT1:
			radioModule_sendCmd(CGATT1, strlen(CGATT1));
			timeout = CGATT_TIMEOUT;
			break;
		case AT_CSQ:
			radioModule_sendCmd(CSQ, strlen(CSQ));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_ATE0:
			radioModule_sendCmd(ATE0, strlen(ATE0));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QNWINFO:
			radioModule_sendCmd(QNWINFO, strlen(QNWINFO));
			timeout = CONFIG_TIMEOUT;
			break;
		case AT_QMTOPEN:
			radioModule_sendCmd(QMTOPEN, strlen(QMTOPEN));
			timeout = MQTT_TIMEOUT;
			break;
		case AT_QMTCLOSE:
			radioModule_sendCmd(QMTCLOSE, strlen(QMTCLOSE));
			timeout = MQTT_TIMEOUT;
			break;
		case AT_QMTCONN:
			radioModule_sendCmd(QMTCONN, strlen(QMTCONN));
			timeout = MQTT_TIMEOUT;
			break;
		case AT_QMTDISC:
			radioModule_sendCmd(QMTDISC, strlen(QMTDISC));
			timeout = MQTT_TIMEOUT;
			break;
		case AT_QMTSUB:
			radioModule_sendCmd(QMTSUB, strlen(QMTSUB));
			timeout = MQTT_TIMEOUT;
			break;
		case AT_QMTUNS:
			radioModule_sendCmd(QMTUNS, strlen(QMTUNS));
			timeout = MQTT_TIMEOUT;
			break;
		case AT_QMTPUBEX:
			radioModule_sendCmd(QMTPUBEX, strlen(QMTPUBEX));
			timeout = MQTT_TIMEOUT;
			break;
		default:
			//
			break;
	}

	responseDataSize = radioModule_response(atCmd, rxData, timeout);
	radioModule_parser(rxData, responseDataSize);
}

void radioModule_sendCmd(const char *txData, uint32_t txDataSize)
{
	uint8_t i = 0;

	for(i = 0; i< txDataSize; i++)
	{
		while(!(USART2->SR & USART_SR_TXE));
		USART2->DR = *txData;
		txData++;
	}
}

uint32_t radioModule_response(eAtCmd_t cmdSent, char *response, uint32_t respTimeout)
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

		if((response[bufCount-2] == 'O') && (response[bufCount-1] == 'K'))
		{
			respACK = 1;
		}

	}while((respACK == 0) && ((HAL_GetTick() - startTick) <= respTimeout));
	asm("nop");
	return (bufCount - 1);
}

void radioModule_parser(const char *respToParse, uint32_t respSize)
{

}
