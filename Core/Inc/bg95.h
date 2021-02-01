/*
 * AT commands' response timeout for the BG95. All values in milliseconds
 */
#define CPIN_TIMEOUT	5000
#define CFUN_TIMEOUT	15000
#define CGATT_TIMEOUT	140000
#define MQTT_TIMEOUT	15000
#define CONFIG_TIMEOUT	300

typedef enum {
	AT_AT = 0x00,
	AT_QPOWD,
	AT_CPIN,
	AT_QCFG_NWSCANMODE,
	AT_QCFG_NWSCANSEQ,
	AT_QCFG_IOTOPMODE,
	AT_QCFG_BAND,
	AT_QICSGP,
	AT_CFUN0,
	AT_CFUN1,
	AT_CREG,
	AT_CGREG,
	AT_CEREG,
	AT_CGATT0,
	AT_CGATT1,
	AT_CSQ,
	AT_ATE0,
	AT_QNWINFO,
	/*
	 * You should not add non-mqtt related
	 * commands in between the following ones
	 */
	AT_QMTOPEN,
	AT_QMTCLOSE,
	AT_QMTCONN,
	AT_QMTDISC,
	AT_QMTSUB,
	AT_QMTUNS,
	AT_QMTPUBEX
}eAtCmd_t;

typedef enum {
	bg95_ok = 0x00,
	bg95_error,
	csq_noSignal,
	csq_poorSignal,
	csq_goodSignal,
	csq_greatSignal
}eBg95Status_t;

void bg95_turnOn(void);
eBg95Status_t bg95_transmit(eAtCmd_t atCmd, char*rxData);
		//const char *txData, char *rxData, uint32_t timeout, uint32_t txDataSize);
