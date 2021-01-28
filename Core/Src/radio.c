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
//	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
//	delay_ms(1250);
//	GPIOA->ODR	 |= GPIO_ODR_OD1;
	radioModule_transmit(AT_QPOWD, rxBuf);
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
	radioModule_transmit(ATE0, rxBuf);
	breakpoint();

	radioModule_transmit(AT_CPIN, rxBuf);
	breakpoint();

	radioModule_transmit(AT_QCFG_NWSCANMODE, rxBuf);
	breakpoint();

	radioModule_transmit(AT_QCFG_NWSCANSEQ, rxBuf);
	breakpoint();

	radioModule_transmit(AT_QCFG_IOTOPMODE, rxBuf);
	breakpoint();

	radioModule_transmit(AT_QCFG_BAND, rxBuf);
	breakpoint();

	radioModule_transmit(AT_QICSGP, rxBuf);
	breakpoint();
}

void radio_connect(void)
{
	char creg = 0, cgreg = 0, cereg = 0;

	radioModule_transmit(AT_CFUN0, rxBuf);
	breakpoint();

	delay_ms(5000);

	radioModule_transmit(AT_CFUN1, rxBuf);
	breakpoint();

	do
	{
		radioModule_transmit(AT_CREG, rxBuf);
		creg = rxBuf[9];

		radioModule_transmit(AT_CGREG, rxBuf);
		cgreg = rxBuf[10];

		radioModule_transmit(AT_CEREG, rxBuf);
		cereg = rxBuf[10];
	}while(	((creg != '1')	|| (creg != '5'))	||
			((cgreg != '1')	|| (cgreg != '5'))	||
			((cereg != '1')	|| (cereg != '5'))	);
	breakpoint();

	radioModule_transmit(AT_QNWINFO, rxBuf);
	breakpoint();

	radioModule_transmit(AT_CSQ, rxBuf);
	breakpoint();

	radioModule_transmit(AT_CGATT1, rxBuf);
	breakpoint();

	radioModule_transmit(AT_QMTOPEN, rxBuf);
	breakpoint();

	radioModule_transmit(AT_QMTCONN, rxBuf);
	breakpoint();
}

void radio_publish(void)	//const char *msg)
{
	radioModule_transmit(AT_QMTPUBEX, rxBuf);
	breakpoint();
}

void radio_disconnect(void)
{
	radioModule_transmit(AT_QMTDISC, rxBuf);
	breakpoint();

	radioModule_transmit(AT_QMTCLOSE, rxBuf);
	breakpoint();

	radioModule_transmit(AT_CGATT0, rxBuf);
	breakpoint();
}
