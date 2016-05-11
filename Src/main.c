/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 *
 * COPYRIGHT(c) 2016 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm32f4xx_hal.h"
#include "UARTHelper.h"
#include "global.h"
#include "UARTHandler.h"


/* Private variables ---------------------------------------------------------*/
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart5_rx;
DMA_HandleTypeDef hdma_uart7_rx;
DMA_HandleTypeDef hdma_uart8_rx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart6_rx;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void initUART(void);



void initUART(void) {
    MX_UART_Init(&(uartInterfaces[0].uartHandler), USART1, 9600);
    MX_UART_Init(&(uartInterfaces[1].uartHandler), USART2, 9600);
    MX_UART_Init(&(uartInterfaces[2].uartHandler), USART3, 9600);
    MX_UART_Init(&(uartInterfaces[3].uartHandler), UART4, 9600);
    MX_UART_Init(&(uartInterfaces[4].uartHandler), UART5, 9600);
    MX_UART_Init(&(uartInterfaces[5].uartHandler), USART6, 9600);
    MX_UART_Init(&(uartInterfaces[6].uartHandler), UART7, 9600);
    MX_UART_Init(&(uartInterfaces[7].uartHandler), UART8, 9600);
    namedInterface.pcUART = &uartInterfaces[0];

    for (int i = 0; i < 7; i++) {
        namedInterface.daughterBoards[i].uartInterface = &uartInterfaces[i+1];
        namedInterface.daughterBoards[i].isBoardInserted[0] = false;
        namedInterface.daughterBoards[i].isBoardInserted[1] = false;
        memset(&namedInterface.daughterBoards[i].uuid[0], 0, 41);
        memset(&namedInterface.daughterBoards[i].uuid[1], 0, 41);
    }

    for (int i = 0; i < 8; i++) {
        initUARTInterface(&uartInterfaces[i]);          
    }

}

int main(void)
{


    /* MCU Configuration----------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    initUART();
    startUARTReceiveDMA(&uartInterfaces[0]);
    startUARTReceiveDMA(&uartInterfaces[1]);
    startUARTReceiveDMA(&uartInterfaces[2]);
    startUARTReceiveDMA(&uartInterfaces[3]);
    startUARTReceiveDMA(&uartInterfaces[4]);
    startUARTReceiveDMA(&uartInterfaces[5]);
    startUARTReceiveDMA(&uartInterfaces[6]);
    startUARTReceiveDMA(&uartInterfaces[7]);
    uint32_t lastTime = 0;

    /*      Infinite loop */
    while (1)
    {
        processUARTContent(uartReceiverCallback);
        uint32_t tick = HAL_GetTick();
        uint32_t round = tick / 1000;

        if (lastTime != round) {
            //debugMessage("Hello World:%d\n", tick);
            //sendToUART(namedInterface.pcUART, "Tick: %d\n", tick);
            //sendToUART(&uartInterfaces[0], "Hello World:%d\n", round);
        }
        lastTime = round;               
    }

}


UartInterface * getUARTInterface(UART_HandleTypeDef *huart) {
    if (huart == &(uartInterfaces[0].uartHandler)) {
        return &(uartInterfaces[0]);
    }
    else if (huart == &(uartInterfaces[1].uartHandler)) {
        return &(uartInterfaces[1]);
    }
    else if (huart == &(uartInterfaces[2].uartHandler)) {
        return &(uartInterfaces[2]);
    }
    else if (huart == &(uartInterfaces[3].uartHandler)) {
        return &(uartInterfaces[3]);
    }
    else if (huart == &(uartInterfaces[4].uartHandler)) {
        return &(uartInterfaces[4]);
    }
    else if (huart == &(uartInterfaces[5].uartHandler)) {
        return &(uartInterfaces[5]);
    }
    else if (huart == &(uartInterfaces[6].uartHandler)) {
        return &(uartInterfaces[6]);
    }
    else if (huart == &(uartInterfaces[7].uartHandler)) {
        return &(uartInterfaces[7]);
    } else {
        return NULL;
    }
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    __PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
        |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}



/** 
 * Enable DMA controller clock
 */
void MX_DMA_Init(void) 
{
    /* DMA controller clock enable */
    __DMA1_CLK_ENABLE();
    __DMA2_CLK_ENABLE();

    /* DMA interrupt init */
    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
    HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
    HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}

/** Configure pins as 
 * Analog 
 * Input 
 * Output
 * EVENT_OUT
 * EXTI
 */
void MX_GPIO_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

    /*Configure GPIO pin : PB12 */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */

}

#endif
