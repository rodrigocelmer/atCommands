#include "stdint.h"

#define UID_SIZE		12				/*!< Actual STM32L053's unique ID size	*/
#define UID_STRING_SIZE	(UID_SIZE + 16)	/*!< 16 bytes offset for uidString		*/

void system_readUID(char *uid);
uint8_t system_getResetSource(void);
