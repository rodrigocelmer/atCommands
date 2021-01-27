#include "stdint.h"
#include "main.h"
#include "bg95.h"

char rxBuf[100] = {'\0'};

void breakpoint(void);

void breakpoint(void)
{
	asm("nop");

	uint8_t i = 0;
	for(i=0; i<100; i++)
	{
		rxBuf[i] = '\0';
	}
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
	GPIOA->ODR	 &= ~GPIO_ODR_OD1;
	delay_ms(750);
	GPIOA->ODR	 |= GPIO_ODR_OD1;
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
	radioModule_transmit(AT_CFUN0, rxBuf);
	breakpoint();

	delay_ms(5000);

	radioModule_transmit(AT_CFUN1, rxBuf);
	breakpoint();

	radioModule_transmit(AT_CREG, rxBuf);
	breakpoint();

	radioModule_transmit(AT_CGREG, rxBuf);
	breakpoint();

	radioModule_transmit(AT_CEREG, rxBuf);
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
