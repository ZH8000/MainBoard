#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "UARTHelper.h"
#include <stdbool.h>

#define DEBUG_UART									(&uartInterfaces[0])
#define DEBUG												

typedef struct {
	UartInterface * uartInterface;
	bool isBoardInserted[2];
	char uuid[2][40];
} DaughterBoard;

typedef struct {
	UartInterface * pcUART;
	DaughterBoard daughterBoards[7];
} NamedInterface;

extern UartInterface uartInterfaces[8];
extern NamedInterface namedInterface;

#endif
