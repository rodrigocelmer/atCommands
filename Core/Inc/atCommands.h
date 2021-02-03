
//AT commands
const char AT_AT[]					= {"AT\r\n\0"};															//resp: OK
const char AT_QPOWD[] 				= {"AT+QPOWD\r\n\0"};													//resp: OK POWERED DOWN
const char AT_QCFG_NWSCANMODE[]		= {"AT+QCFG=\"NWSCANMODE\",0\r\n\0"};									//resp: OK
const char AT_QCFG_NWSCANSEQ[]		= {"AT+QCFG=\"NWSCANSEQ\",020103\r\n\0"};								//resp: OK
const char AT_QCFG_IOTOPMODE[]		= {"AT+QCFG=\"IOTOPMODE\",2\r\n\0"};									//resp: OK
const char AT_QCFG_BAND[]			= {"AT+QCFG=\"BAND\",0XF,0,0X8000004\r\n\0"};							//resp: OK
const char AT_QICSGP[]				= {"AT+QICSGP=1,1,\"virtueyes.com.br\",\"virtu\",\"virtu\",3\r\n\0"};	//resp: OK
const char AT_CFUN0[]				= {"AT+CFUN=0\r\n\0"};													//resp: OK
const char AT_CFUN1[]				= {"AT+CFUN=1\r\n\0"};													//resp: OK
const char AT_CGATT0[]				= {"AT+CGATT=0\r\n\0"};													//resp: OK
const char AT_CGATT1[]				= {"AT+CGATT=1\r\n\0"};													//resp: OK
const char AT_ATE0[]				= {"ATE0\r\n\0"};														//resp:
const char AT_CPIN[]				= {"AT+CPIN?\r\n\0"};													//resp: +CPIN: [7]R[8]EADY OK
const char AT_CREG[]				= {"AT+CREG?\r\n\0"};													//resp: +CREG: 0,[9]2 OK
const char AT_CGREG[]				= {"AT+CGREG?\r\n\0"};													//resp: +CGREG: 0,[10]4 OK
const char AT_CEREG[]				= {"AT+CEREG?\r\n\0"};													//resp: +CEREG: 0,[10]2 OK
const char AT_CSQ[]					= {"AT+CSQ\r\n\0"};														//resp: +CSQ: [6]2[7]6,99 OK
const char AT_QNWINFO[]				= {"AT+QNWINFO\r\n\0"};													//resp: +QNWINFO: "GPRS","72405,"GSM 1800", 594

//MQTT commands
const char AT_QMTOPEN[]				= {"AT+QMTOPEN=1,\"tailor.cloudmqtt.com\",13291\r\n\0"};				//resp: OK +QMTOPEN: [13]1,[15]0
const char AT_QMTCLOSE[]			= {"AT+QMTCLOSE=1\r\n\0"};												//resp:
const char AT_QMTCONN[]				= {"AT+QMTCONN=1,\"usrCelmer\",\"zgxbgfsy\",\"H7Mnnfi0_2rk\"\r\n\0"};	//resp: OK +QMTCONN: [13]1,[15]0,[17]0
const char AT_QMTDISC[]				= {"AT+QMTDISC=1\r\n\0"};												//resp: OK +QMTDISC: [13]1,[15]0
const char AT_QMTSUB[]				= {"AT+QMTSUB=1,1,\"celmer\",1\r\n\0"};									//resp: OK +QMTSUB: [12]1,[14]1,[16]0,[18]1 +QMTRECV: 1,1,"topic","msg"
const char AT_QMTUNS[]				= {"AT+QMTUNS=1,1,\"celmer\"\r\n\0"};									//resp: OK +QMTUNS: [12]1,[14]1,[16]0
const char AT_QMTPUBEX[]			= {"AT+QMTPUBEX=1,1,1,1,\"celmer\",\"careless whisper\"\r\n\0"};				//resp: OK +QMTPUB: [12]1,[14]3,[16]0
