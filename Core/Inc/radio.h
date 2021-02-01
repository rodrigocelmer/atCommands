#include "stdint.h"

typedef enum {
	radio_ok = 0x00,
	radio_error,
	noSignal,
	poorSignal,
	goodSignal,
	greatSignal,
}eRadioStatus_t;

void delay_ms(uint32_t time_ms);
void radio_reset(void);

void radio_turnOn(void);
void radio_turnOff(void);
void radio_config(void);
void radio_connect(void);
void radio_publish(void);	//const char *msg);
void radio_disconnect(void);
