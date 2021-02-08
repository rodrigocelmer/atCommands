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

void radio_init(sRadio_t *radioModule)
{
	//gpio_init();

	if((GPIOC->IDR & GPIO_IDR_ID1) && (GPIOC->IDR & GPIO_IDR_ID0))
	{
		//bg95
		radioModule->turnOn		 = bg95_turnOn;
		radioModule->turnOff	 = bg95_turnOff;
		radioModule->reset		 = bg95_reset;
		radioModule->config		 = bg95_config;
		radioModule->checkSignal = bg95_checkSignal;
		radioModule->connect	 = bg95_connect;
		radioModule->publish	 = bg95_publish;
		radioModule->disconnect	 = bg95_disconnect;
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
