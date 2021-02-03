/*
 * AT commands' response timeout for the BG95. All values in milliseconds
 */
#define CPIN_TIMEOUT	5000
#define CFUN_TIMEOUT	15000
#define CGATT_TIMEOUT	140000
#define MQTT_TIMEOUT	15000
#define CONFIG_TIMEOUT	300

typedef enum {
	bg95_ok = 0x00,
	bg95_error,
	csq_noSignal,
	csq_poorSignal,
	csq_goodSignal,
	csq_greatSignal
}eBg95Status_t;

void bg95_turnOn(void);
eBg95Status_t bg95_transmit(const char *txData, char *rxData, uint32_t timeout, uint32_t txDataSize);
		//eAtCmd_t atCmd, char*rxData);
		//
