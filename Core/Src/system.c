#include "stm32f411xe.h"
#include "stdint.h"
#include "stdio.h"

void read_uniqueID(char *uid)
{
	sprintf(uid,"0x%08lx%08lx%08lx", *(volatile uint32_t *)(UID_BASE+8), *(volatile uint32_t *)(UID_BASE+4), *(volatile uint32_t *)(UID_BASE));
}
