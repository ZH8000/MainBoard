#ifndef __UARTHELPER_H__
#define __UARTHELPER_H__

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define CONTENT_QUEUE_NUM		30
#define CONTENT_QUEUE_SIZE	100

typedef struct {
	UART_HandleTypeDef uartHandler;
	uint8_t rxBuffer;
	char buffer[CONTENT_QUEUE_SIZE];
	int bufferCounter;
	bool hasData[CONTENT_QUEUE_NUM];
	char content[CONTENT_QUEUE_NUM][CONTENT_QUEUE_SIZE];
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
void initUARTInterface(UartInterface * uartInterface);
#endif
