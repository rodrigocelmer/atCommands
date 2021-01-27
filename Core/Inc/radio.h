#include "stdint.h"

const char AT[]					= {"AT\r\n\0"};
const char AT_QPOWD[] 			= {"AT+QPOWD\r\n\0"};
const char AT_CPIN[]			= {"AT+CPIN?\r\n\0"};
const char AT_QCFG_NWSCANMODE[]	= {"AT+QCFG=\"NWSCANMODE\",0\r\n\0"};
const char AT_QCFG_NWSCANSEQ[]	= {"AT+QCFG=\"NWSCANSEQ\",020103\r\n\0"};
const char AT_QCFG_IOTOPMODE[]	= {"AT+QCFG=\"IOTOPMODE\",2\r\n\0"};
const char AT_QCFG_BAND[]		= {"AT+QCFG=\"BAND\",0XF,0,0X8000004\r\n\0"};
const char AT_QICSGP[]			= {"AT+QICSGP=1,1,\"virtueyes.com.br\",\"virtu\",\"virtu\",3\r\n\0"};
const char AT_CFUN0[]			= {"AT+CFUN=0\r\n\0"};
const char AT_CFUN1[]			= {"AT+CFUN=1\r\n\0"};
const char AT_CREG[]			= {"AT+CREG?\r\n\0"};
const char AT_CGREG[]			= {"AT+CGREG?\r\n\0"};
const char AT_CEREG[]			= {"AT+CEREG?\r\n\0"};
const char AT_CGATT0[]			= {"AT+CGATT=0\r\n\0"};
const char AT_CGATT1[]			= {"AT+CGATT=1\r\n\0"};
const char AT_CSQ[]				= {"AT+CSQ\r\n\0"};
const char ATE0[]				= {"ATE0\r\n\0"};
const char AT_QNWINFO[]			= {"AT+QNWINFO\r\n\0"};

//MQTT commands
const char AT_QMTOPEN[]			= {"AT+QMTOPEN=1,\"tailor.cloudmqtt.com\",13291\r\n\0"};
const char AT_QMTCLOSE[]		= {"AT+QMTCLOSE=1\r\n\0"};
const char AT_QMTCONN[]			= {"AT+QMTCONN=1,\"usrCelmer\",\"zgxbgfsy\",\"H7Mnnfi0_2rk\"\r\n\0"};
const char AT_QMTDISC[]			= {"AT+QMTDISC=1\r\n\0"};
const char AT_QMTSUB[]			= {"AT+QMTSUB=1,1,\"celmer\",1\r\n\0"};
const char AT_QMTUNS[]			= {"AT+QMTUNS=1,1,\"celmer\"\r\n\0"};
const char AT_QMTPUBEX[]		= {"AT+QMTPUBEX=1,1,1,1,\"celmer\",\"rose tattoo\"\r\n\0"};

void delay_ms(uint32_t time_ms);
void radio_turnOn(void);
void radio_turnOff(void);
void radio_reset(void);
void radio_transmit(const char *txData, char *rxData);
