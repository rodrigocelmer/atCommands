#include "stdint.h"
#include "main.h"
#include "bg95.h"
#include "string.h"

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
	radioModule_turnOn();
}

void radio_turnOff(void)
{
	radioModule_transmit(AT_QPOWD, rxBuf, CONFIG_TIMEOUT);
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
	radioModule_transmit(ATE0, rxBuf, CONFIG_TIMEOUT);
	breakpoint();

	radioModule_transmit(AT_CPIN, rxBuf, CPIN_TIMEOUT);
	breakpoint();

	radioModule_transmit(AT_QCFG_NWSCANMODE, rxBuf, CONFIG_TIMEOUT);
	breakpoint();

	radioModule_transmit(AT_QCFG_NWSCANSEQ, rxBuf, CONFIG_TIMEOUT);
	breakpoint();

	radioModule_transmit(AT_QCFG_IOTOPMODE, rxBuf, CONFIG_TIMEOUT);
	breakpoint();

	radioModule_transmit(AT_QCFG_BAND, rxBuf, CONFIG_TIMEOUT);
	breakpoint();

	radioModule_transmit(AT_QICSGP, rxBuf, CONFIG_TIMEOUT);
	breakpoint();
}

void radio_connect(void)
{
	char creg = 0, cgreg = 0, cereg = 0;

	radioModule_transmit(AT_CFUN0, rxBuf, CFUN_TIMEOUT);
	breakpoint();

	delay_ms(5000);

	radioModule_transmit(AT_CFUN1, rxBuf, CFUN_TIMEOUT);
	breakpoint();

	do
	{
		radioModule_transmit(AT_CREG, rxBuf, CONFIG_TIMEOUT);
		creg = rxBuf[9];

		radioModule_transmit(AT_CGREG, rxBuf, CONFIG_TIMEOUT);
		cgreg = rxBuf[10];

		radioModule_transmit(AT_CEREG, rxBuf, CONFIG_TIMEOUT);
		cereg = rxBuf[10];
	}while(	((creg != '1')	&& (creg != '5'))	&&
			((cgreg != '1')	&& (cgreg != '5'))	&&
			((cereg != '1')	&& (cereg != '5'))	);
	breakpoint();

	radioModule_transmit(AT_QNWINFO, rxBuf, CONFIG_TIMEOUT);
	breakpoint();

	radioModule_transmit(AT_CSQ, rxBuf, CONFIG_TIMEOUT);
	breakpoint();

	radioModule_transmit(AT_CGATT1, rxBuf, CGATT_TIMEOUT);
	breakpoint();

	radioModule_transmit(AT_QMTOPEN, rxBuf, MQTT_TIMEOUT);
	breakpoint();

	radioModule_transmit(AT_QMTCONN, rxBuf, MQTT_TIMEOUT);
	breakpoint();
}

void radio_publish(void)	//const char *msg)
{
	radioModule_transmit(AT_QMTPUBEX, rxBuf, MQTT_TIMEOUT);
	breakpoint();
}

void radio_disconnect(void)
{
	radioModule_transmit(AT_QMTDISC, rxBuf, MQTT_TIMEOUT);
	breakpoint();

	radioModule_transmit(AT_CGATT0, rxBuf, CGATT_TIMEOUT);
	breakpoint();
}
