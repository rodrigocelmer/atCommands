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
	AT_QMTOPEN,
	AT_QMTCLOSE,
	AT_QMTCONN,
	AT_QMTDISC,
	AT_QMTSUB,
	AT_QMTUNS,
	AT_QMTPUBEX
}eAtCmd_t;

void radioModule_turnOn(void);
void radioModule_transmit(eAtCmd_t atCmd, char*rxData);
		//const char *txData, char *rxData, uint32_t timeout, uint32_t txDataSize);
