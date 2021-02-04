#include "stdint.h"
#include "main.h"
#include "radio.h"
#include "bg95.h"

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

void radio_init(void)
{
	//gpio_init();

	if((GPIOC->IDR & GPIO_IDR_ID1) && !(GPIOC->IDR & GPIO_IDR_ID0))
	{
		//bg95
		radio.turnOn		= bg95_turnOn;
		radio.turnOff		= bg95_turnOff;
		radio.reset			= bg95_reset;
		radio.config		= bg95_config;
		radio.connect		= bg95_connect;
		radio.publish		= bg95_publish;
		radio.disconnect	= bg95_disconnect;
	}
	else if(!(GPIOC->IDR & GPIO_IDR_ID1) && (GPIOC->IDR & GPIO_IDR_ID0))
	{
		//stingr
	}
	else if(!(GPIOC->IDR & GPIO_IDR_ID1) && !(GPIOC->IDR & GPIO_IDR_ID0))
	{
		//no radio connected
	}
}

//void radio_turnOn(void)
//{
//	bg95_turnOn();
//}
//
//void radio_turnOff(void)
//{
//	bg95_turnOff();
//}
//
//void radio_reset(void)
//{
//	bg95_reset();
//}
//
//void radio_config(void)
//{
//	bg95_config();
//}
//
//void radio_connect(void)
//{
//	bg95_connect();
//}
//
//void radio_publish(void)	//const char *msg)
//{
//	bg95_publish();
//}
//
//void radio_disconnect(void)
//{
//	bg95_disconnect();
//}
