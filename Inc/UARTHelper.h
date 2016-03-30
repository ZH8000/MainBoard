#ifndef __UARTHELPER_H__
#define __UARTHELPER_H__

#include "stm32f4xx_hal.h"
#include <stdbool.h>

typedef struct {
	UART_HandleTypeDef uartHandler;
	uint8_t rxBuffer;
	int bufferCounter;
	char buffer[100];
	char content[100];
	bool shouldProcessContent;
	uint32_t busyCount;
	uint32_t receivedBytes;
	bool hasData[10];
	char buffer2[10][100];
	int writeCounter;
	int readCounter;	
} UartInterface ;


typedef void (*UartContentCallback)(UartInterface *, char *);

void startUARTReceiveDMA(UartInterface * interface);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void MX_UART_Init(UART_HandleTypeDef * uartHandler, USART_TypeDef * uartInstance, int baudRate);
HAL_StatusTypeDef sendToUART(UartInterface * uartInterface, char * format, ...);
void debugMessage(char * format, ...);
void processUARTContent(UartContentCallback callback);
extern UartInterface * getUARTInterface(UART_HandleTypeDef *huart);

#endif
