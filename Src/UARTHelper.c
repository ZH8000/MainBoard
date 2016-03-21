#include "UARTHelper.h"
#include <string.h>

extern UartInterface * getUARTInterface(UART_HandleTypeDef *huart, int * whichUART);

void startUARTReceiveDMA(UartInterface * interface) {
	HAL_UART_Receive_DMA(&(interface->uartHandler), &(interface->rxBuffer), 1);	
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	
	//__HAL_UART_FLUSH_DRREGISTER(huart);
	uint8_t receiveChar = *(huart->pRxBuffPtr);
	int whichUART;
	
	UartInterface * uartInterface = getUARTInterface(huart, &whichUART);
	
	if (uartInterface != NULL) {
		if (receiveChar == '\r' || receiveChar == '\n') {
			char message[100] = {0};
			sprintf(message, "Command[%d] = (%d, %d, %s)\r\n", uartInterface->commandCount, whichUART, strlen(uartInterface->buffer), uartInterface->buffer);
			HAL_UART_Transmit(&uartInterface->uartHandler, (uint8_t *) &message, strlen(message), 100);
			memset(uartInterface->buffer, 0, 100);
			uartInterface->bufferCounter = 0;
			uartInterface->commandCount++;
		} else {
			uartInterface->buffer[uartInterface->bufferCounter] = receiveChar;
			uartInterface->bufferCounter++;
		}
	}

	HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr, 1);	
}

void MX_UART_Init(UART_HandleTypeDef * uartHandler, USART_TypeDef * uartInstance)
{

  uartHandler->Instance = uartInstance;
  uartHandler->Init.BaudRate = 115200;
  uartHandler->Init.WordLength = UART_WORDLENGTH_8B;
  uartHandler->Init.StopBits = UART_STOPBITS_1;
  uartHandler->Init.Parity = UART_PARITY_NONE;
  uartHandler->Init.Mode = UART_MODE_TX_RX;
  uartHandler->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uartHandler->Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(uartHandler);
}
