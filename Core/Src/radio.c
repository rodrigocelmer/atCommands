#include "stdint.h"
#include "main.h"
#include "bg95.h"
#include "string.h"
#include "atCommands.h"

char rxBuf[100] = {'\0'};

void breakpoint(void);

void breakpoint(void)
{
	asm("nop");

	memset(rxBuf, '\0', 100);
}

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
	bg95_turnOn();
}

void radio_turnOff(void)
{
	bg95_transmit(AT_QPOWD, rxBuf, CONFIG_TIMEOUT, strlen(AT_QPOWD));
	breakpoint();
}

void radio_reset(void)
{
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(2500);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
}

void radio_config(void)
{
	bg95_transmit(AT_ATE0, rxBuf, CONFIG_TIMEOUT, strlen(AT_ATE0));
	breakpoint();

	bg95_transmit(AT_CPIN, rxBuf, CPIN_TIMEOUT, strlen(AT_CPIN));
	breakpoint();

	bg95_transmit(AT_QCFG_NWSCANMODE, rxBuf, CONFIG_TIMEOUT, strlen(AT_QCFG_NWSCANMODE));
	breakpoint();

	bg95_transmit(AT_QCFG_NWSCANSEQ, rxBuf, CONFIG_TIMEOUT, strlen(AT_QCFG_NWSCANSEQ));
	breakpoint();

	bg95_transmit(AT_QCFG_IOTOPMODE, rxBuf, CONFIG_TIMEOUT, strlen(AT_QCFG_IOTOPMODE));
	breakpoint();

	bg95_transmit(AT_QCFG_BAND, rxBuf, CONFIG_TIMEOUT, strlen(AT_QCFG_BAND));
	breakpoint();

	bg95_transmit(AT_QICSGP, rxBuf, CONFIG_TIMEOUT, strlen(AT_QICSGP));
	breakpoint();
}

void radio_connect(void)
{
	eBg95Status_t creg = bg95_error, cgreg = bg95_error, cereg = bg95_error;

	bg95_transmit(AT_CFUN0, rxBuf, CFUN_TIMEOUT, strlen(AT_CFUN0));
	breakpoint();

	delay_ms(5000);

	bg95_transmit(AT_CFUN1, rxBuf, CFUN_TIMEOUT, strlen(AT_CFUN1));
	breakpoint();

	do
	{
		creg = bg95_transmit(AT_CREG, rxBuf, CONFIG_TIMEOUT, strlen(AT_CREG));

		cgreg = bg95_transmit(AT_CGREG, rxBuf, CONFIG_TIMEOUT, strlen(AT_CGREG));

		cereg = bg95_transmit(AT_CEREG, rxBuf, CONFIG_TIMEOUT, strlen(AT_CEREG));
	}while(	(creg != bg95_ok)	&&
			(cgreg != bg95_ok)	&&
			(cereg != bg95_ok)	);
	breakpoint();

	bg95_transmit(AT_QNWINFO, rxBuf, CONFIG_TIMEOUT, strlen(AT_QNWINFO));
	breakpoint();

	bg95_transmit(AT_CSQ, rxBuf, CONFIG_TIMEOUT, strlen(AT_CSQ));
	breakpoint();

	bg95_transmit(AT_CGATT1, rxBuf, CGATT_TIMEOUT, strlen(AT_CGATT1));
	breakpoint();

	bg95_transmit(AT_QMTOPEN, rxBuf, MQTT_TIMEOUT, strlen(AT_QMTOPEN));
	breakpoint();

	bg95_transmit(AT_QMTCONN, rxBuf, MQTT_TIMEOUT, strlen(AT_QMTCONN));
	breakpoint();
}

void radio_publish(void)	//const char *msg)
{
	bg95_transmit(AT_QMTPUBEX, rxBuf, MQTT_TIMEOUT, strlen(AT_QMTPUBEX));
	breakpoint();
}

void radio_disconnect(void)
{
	bg95_transmit(AT_QMTDISC, rxBuf, MQTT_TIMEOUT, strlen(AT_QMTDISC));
	breakpoint();

	bg95_transmit(AT_CGATT0, rxBuf, CGATT_TIMEOUT, strlen(AT_CGATT0));
	breakpoint();
}
