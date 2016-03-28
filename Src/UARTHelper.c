#include <string.h>
#include <stdarg.h>
#include "global.h"
#include "UARTHelper.h"
#include <stdlib.h>

void processUARTContent(UartContentCallback callback) {

	for (int i = 0; i < 8; i++) {
		UartInterface * uartInterface = &uartInterfaces[i];
		if (uartInterface->shouldProcessContent) {			
			callback(uartInterface, uartInterface->content);			
			uartInterface->shouldProcessContent = false;			
			memset(uartInterface->content, 0, 100);
		}

	}
}

void debugMessage(char * format, ...) {
	#ifdef DEBUG
		char message[100] = {0};
		va_list argptr;
		va_start(argptr,format);
		vsnprintf(message, 100, format, argptr);
		va_end(argptr);
		HAL_UART_Transmit(&DEBUG_UART->uartHandler, (uint8_t *) message, strlen(message), 100);		
		HAL_Delay(100);
	#endif
}

void sendToUART(UartInterface * uartInterface, char * format, ...) {
	char message[100] = {0};
	va_list argptr;
	va_start(argptr,format);
	vsnprintf(message, 100, format, argptr);
	va_end(argptr);
	HAL_UART_Transmit(&uartInterface->uartHandler, (uint8_t *) message, strlen(message), 100);		
	HAL_Delay(100);	
}


void startUARTReceiveDMA(UartInterface * uartInterface) {
	int retries = 0;
	HAL_StatusTypeDef status = HAL_UART_Receive_DMA(&(uartInterface->uartHandler), &(uartInterface->rxBuffer), 1);
	
	while (status != HAL_OK && retries < 10) {
		uartInterface->busyCount++;
		HAL_UART_DeInit(&(uartInterface->uartHandler));
		HAL_UART_Init(&(uartInterface->uartHandler));
		status = HAL_UART_Receive_DMA(&(uartInterface->uartHandler), &(uartInterface->rxBuffer), 1);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	
	//__HAL_UART_FLUSH_DRREGISTER(huart);
	UartInterface * uartInterface = getUARTInterface(huart);
	uartInterface->receivedBytes++;
	
	if (uartInterface != NULL) {
		
		uint8_t receiveChar = *(huart->pRxBuffPtr);
		
		if (receiveChar == '\n') {
			strncpy(uartInterface->content, uartInterface->buffer, 100);
			uartInterface->shouldProcessContent = true;			
			memset(uartInterface->buffer, 0, 100);
			uartInterface->bufferCounter = 0;
		} else {
			uartInterface->buffer[uartInterface->bufferCounter] = receiveChar;
			uartInterface->bufferCounter++;
		}
		startUARTReceiveDMA(uartInterface);	
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
