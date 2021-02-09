/*
 * AT commands' response timeout for the BG95. All values in milliseconds
 */
#define CPIN_TIMEOUT	5000
#define CFUN_TIMEOUT	15000
#define CGATT_TIMEOUT	140000
#define MQTT_TIMEOUT	15000
#define CONFIG_TIMEOUT	300
#define QIDEACT_TIMEOUT	40000

typedef enum {
	bg95_ok = 0x00,
	bg95_error,
	bg95_timeout,
	csq_noSignal,
	csq_poorSignal,
	csq_goodSignal,
	csq_greatSignal,
	bg95_cmdNotRegistred
}eBg95Status_t;

void bg95_turnOn(void);
void bg95_turnOff(void);
void bg95_reset(void);
eRadioStatus_t bg95_getSerialNumber(char *serialNumBuf);
eRadioStatus_t bg95_config(void);
eRadioStatus_t bg95_checkSignal(void);
eRadioStatus_t bg95_connect(char *mcu_uid, uint32_t uidSize);
eRadioStatus_t bg95_publish(void);	//const char *msg);
eRadioStatus_t bg95_disconnect(void);
