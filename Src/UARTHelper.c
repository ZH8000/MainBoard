#include <string.h>
#include <stdarg.h>
#include "global.h"
#include "UARTHelper.h"
#include <stdlib.h>

void initUARTInterface(UartInterface * uartInterface) {
	uartInterface->readCounter = 0;
	uartInterface->writeCounter = 0;
	for (int j = 0; j < CONTENT_QUEUE_NUM; j++) {
		uartInterface->hasData[j] = false;
		memset(uartInterface->content[j], 0, CONTENT_QUEUE_SIZE);
	}
	
}

void processUARTContent(UartContentCallback callback) {
	char message[CONTENT_QUEUE_SIZE] = {0};
	
	for (int i = 0; i < 8; i++) {
		UartInterface * uartInterface = &uartInterfaces[i];
		
		while (uartInterface->hasData[uartInterface->readCounter]) {
			memset(message, 0, CONTENT_QUEUE_SIZE);
			strncpy(message, (char *) uartInterface->content[uartInterface->readCounter], CONTENT_QUEUE_SIZE);
			callback(uartInterface, message);
			uartInterface->hasData[uartInterface->readCounter] = false;
			uartInterface->readCounter = (uartInterface->readCounter+1) % CONTENT_QUEUE_NUM;
		}

	}
	
}

void debugMessage(char * format, ...) {
	#ifdef DEBUG
		char message[CONTENT_QUEUE_SIZE] = {0};
		va_list argptr;
		va_start(argptr,format);
		vsnprintf(message, CONTENT_QUEUE_SIZE, format, argptr);
		va_end(argptr);
		HAL_UART_Transmit(&DEBUG_UART->uartHandler, (uint8_t *) message, strlen(message), CONTENT_QUEUE_SIZE);		
	#endif
}

HAL_StatusTypeDef sendToUART(UartInterface * uartInterface, char * format, ...) {
	char message[CONTENT_QUEUE_SIZE] = {0};
	va_list argptr;
	va_start(argptr,format);
	vsnprintf(message, CONTENT_QUEUE_SIZE, format, argptr);
	va_end(argptr);
	return HAL_UART_Transmit(&uartInterface->uartHandler, (uint8_t *) message, strlen(message), CONTENT_QUEUE_SIZE);		
}


void startUARTReceiveDMA(UartInterface * uartInterface) {
	int retries = 0;
	HAL_StatusTypeDef status = HAL_UART_Receive_DMA(&(uartInterface->uartHandler), &(uartInterface->rxBuffer), 1);
	
	while (status != HAL_OK && retries < 10) {
		HAL_UART_DeInit(&(uartInterface->uartHandler));
		HAL_UART_Init(&(uartInterface->uartHandler));
		status = HAL_UART_Receive_DMA(&(uartInterface->uartHandler), &(uartInterface->rxBuffer), 1);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	
	//__HAL_UART_FLUSH_DRREGISTER(huart);
	UartInterface * uartInterface = getUARTInterface(huart);
	
	if (uartInterface != NULL) {
		
		uint8_t receiveChar = *(huart->pRxBuffPtr);
		
		if (receiveChar == '\n') {
			
			int currentWriteCount = uartInterface->writeCounter;
			memset(uartInterface->content[currentWriteCount], 0, CONTENT_QUEUE_SIZE);
			strncpy((char *)uartInterface->content[currentWriteCount], uartInterface->buffer, CONTENT_QUEUE_SIZE);
			uartInterface->hasData[currentWriteCount] = true;
			uartInterface->writeCounter = (currentWriteCount + 1) % CONTENT_QUEUE_NUM;
			
			memset(uartInterface->buffer, 0, CONTENT_QUEUE_SIZE);
			uartInterface->bufferCounter = 0;
		} else {
			uartInterface->buffer[uartInterface->bufferCounter] = receiveChar;
			uartInterface->bufferCounter++;
		}
	}
}

void MX_UART_Init(UART_HandleTypeDef * uartHandler, USART_TypeDef * uartInstance, int baudRate)
{

  uartHandler->Instance = uartInstance;
  uartHandler->Init.BaudRate = baudRate;
  uartHandler->Init.WordLength = UART_WORDLENGTH_8B;
  uartHandler->Init.StopBits = UART_STOPBITS_1;
  uartHandler->Init.Parity = UART_PARITY_NONE;
  uartHandler->Init.Mode = UART_MODE_TX_RX;
  uartHandler->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uartHandler->Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(uartHandler);
}
