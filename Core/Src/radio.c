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
	bg95_turnOn();
}

void radio_turnOff(void)
{
	bg95_transmit(AT_QPOWD, rxBuf);
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
	bg95_transmit(AT_ATE0, rxBuf);
	breakpoint();

	bg95_transmit(AT_CPIN, rxBuf);
	breakpoint();

	bg95_transmit(AT_QCFG_NWSCANMODE, rxBuf);
	breakpoint();

	bg95_transmit(AT_QCFG_NWSCANSEQ, rxBuf);
	breakpoint();

	bg95_transmit(AT_QCFG_IOTOPMODE, rxBuf);
	breakpoint();

	bg95_transmit(AT_QCFG_BAND, rxBuf);
	breakpoint();

	bg95_transmit(AT_QICSGP, rxBuf);
	breakpoint();
}

void radio_connect(void)
{
	eBg95Status_t creg = bg95_error, cgreg = bg95_error, cereg = bg95_error;

	bg95_transmit(AT_CFUN0, rxBuf);
	breakpoint();

	delay_ms(5000);

	bg95_transmit(AT_CFUN1, rxBuf);
	breakpoint();

	do
	{
		creg = bg95_transmit(AT_CREG, rxBuf);

		cgreg = bg95_transmit(AT_CGREG, rxBuf);

		cereg = bg95_transmit(AT_CEREG, rxBuf);
	}while(	(creg != bg95_ok)	&&
			(cgreg != bg95_ok)	&&
			(cereg != bg95_ok)	);
	breakpoint();

	bg95_transmit(AT_QNWINFO, rxBuf);
	breakpoint();

	bg95_transmit(AT_CSQ, rxBuf);
	breakpoint();

	bg95_transmit(AT_CGATT1, rxBuf);
	breakpoint();

	bg95_transmit(AT_QMTOPEN, rxBuf);
	breakpoint();

	bg95_transmit(AT_QMTCONN, rxBuf);
	breakpoint();
}

void radio_publish(void)	//const char *msg)
{
	bg95_transmit(AT_QMTPUBEX, rxBuf);
	breakpoint();
}

void radio_disconnect(void)
{
	bg95_transmit(AT_QMTDISC, rxBuf);
	breakpoint();

	bg95_transmit(AT_CGATT0, rxBuf);
	breakpoint();
}
