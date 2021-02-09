#include "stdint.h"

typedef enum {
	radio_ok = 0x00,
	radio_error,
	noSignal,
	poorSignal,
	goodSignal,
	greatSignal,
	radio_cmdNotRegistred
}eRadioStatus_t;

void delay_ms(uint32_t time_ms);	//must be on proper module

/**
  * @brief  Generic radio modules
  */
typedef struct sRadio
{
	/**
	  * @brief  Pointer to
	  * @param	None
	  * @retval None
	  */
	void (*turnOn)(void);

	/**
	  * @brief  Pointer to
	  * @param	None
	  * @retval None
	  */
	void (*turnOff)(void);

	/**
	  * @brief  Pointer to
	  * @param	None
	  * @retval None
	  */
	void (*reset)(void);

	/**
	  * @brief	Pointer to
	  * @param	None
	  * @retval	None
	  */
	eRadioStatus_t (*getSN)(char *serialNumBuf);

	/**
	  * @brief	Pointer to
	  * @param	None
	  * @retval	None
	  */
	eRadioStatus_t (*config)(void);

	/**
	  * @brief	Pointer to
	  * @param	None
	  * @retval	None
	  */
	eRadioStatus_t (*checkSignal)(void);

	/**
	  * @brief	Pointer to
	  * @param	None
	  * @retval None
	  */
	eRadioStatus_t (*connect)(char *mcuUid, uint32_t uidSize);

	/**
	  * @brief  Pointer to
	  * @param	None
	  * @retval None
	  */
	eRadioStatus_t (*publish)(const char *msg, uint32_t msgSize);

	/**
	  * @brief  Pointer to
	  * @param	None
	  * @retval None
	  */
	eRadioStatus_t (*disconnect)(void);
}sRadio_t;

void radio_init(sRadio_t *radioModule);
