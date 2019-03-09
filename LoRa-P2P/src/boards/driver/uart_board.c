/*
    2018 Grope

Description: Uart driver implementation, used for Uart.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Derek Chen
*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "uart_board.h"
#include "stm32l0xx_hal.h"

#define UART1_RX_BUFF_SIZE  128

/* Public variables ---------------------------------------------------------*/
uart_t uart1;

/* Private define ------------------------------------------------------------*/
static UART_HandleTypeDef UartHandle;
static uint8_t rx1_len = 0;
static bool power_on = false;
static uint8_t uart1_buf[UART1_RX_BUFF_SIZE] = {0};

void UART_init()
{
    UartHandle.Instance        = USARTx;
    UartHandle.Init.BaudRate   = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode       = UART_MODE_TX_RX;

    HAL_UART_Init(&UartHandle);

    if(power_on == false)
    {
        //上电第一次需要设置uart1结构体的初值
        uart1.p_rx_buf = uart1_buf;//设置串口接收指针指向相应存储空间
        uart1.rx_len = 0;
        uart1.tx_len = 0;
        uart1.uart_RxDone_flag = 0;
        uart1.uart_TxDone_flag = 1;
        uart1.rx_complete_callback = NULL;//将回调函数的初值设置为NULL
        uart1.tx_complete_callback = NULL;//将回调函数的初值设置为NULL

        power_on = true;
    }

    /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_PE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_ERR);

    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);

    /* Enable the UART Idle line detection  Interrupt */
    __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_IDLE);

}

void UART_DeInit(void)
{
    HAL_UART_DeInit(&UartHandle);
}

/**
  * @brief UART MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - NVIC configuration for UART interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    USARTx_TX_GPIO_CLK_ENABLE();
    USARTx_RX_GPIO_CLK_ENABLE();
    /* Enable USART2 clock */
    USARTx_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = USARTx_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;//GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = USARTx_TX_AF;

    HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USARTx_RX_PIN;
    GPIO_InitStruct.Alternate = USARTx_RX_AF;

    HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

    /*##-3- Configure the NVIC for UART ########################################*/
    /* NVIC for USART1 */
    HAL_NVIC_SetPriority(USARTx_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(USARTx_IRQn);
}



/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    /*##-1- Reset peripherals ##################################################*/
    USARTx_FORCE_RESET();
    USARTx_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks #################################*/
    /* Configure UART Tx as alternate function  */
    HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
    /* Configure UART Rx as alternate function  */
    HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

    /*##-3- Disable the NVIC for UART ##########################################*/
    HAL_NVIC_DisableIRQ(USARTx_IRQn);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    /* Set transmission flag: trasfer complete*/
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    /* Set transmission flag: trasfer complete*/
}

void HAL_UART_SendBytes(uint8_t * str,uint16_t count)
{
    if(HAL_UART_Transmit(&UartHandle, str, count, 5000)!= HAL_OK)
    {
        while(1)
        {
        }
    }
}

uint8_t HAL_USART_GET_FLAG(USART_TypeDef * usartx,uint32_t flag)
{
    if((usartx->ISR & flag) == flag)
        return SET;
    else
        return RESET;
}

void error_process(UART_HandleTypeDef *huart)
{
    /* UART parity error interrupt occurred ------------------------------------*/

    if((__HAL_UART_GET_IT(huart, UART_IT_PE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE) != RESET))
    {
        __HAL_UART_CLEAR_IT(huart, UART_CLEAR_PEF);

        huart->ErrorCode |= HAL_UART_ERROR_PE;
        /* Set the UART state ready to be able to start again the process */
        huart->gState = HAL_UART_STATE_READY;
        huart->RxState = HAL_UART_STATE_READY;
    }

    /* UART frame error interrupt occured --------------------------------------*/
    if((__HAL_UART_GET_IT(huart, UART_IT_FE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR) != RESET))
    {
        __HAL_UART_CLEAR_IT(huart, UART_CLEAR_FEF);

        huart->ErrorCode |= HAL_UART_ERROR_FE;
        /* Set the UART state ready to be able to start again the process */
        huart->gState = HAL_UART_STATE_READY;
        huart->RxState = HAL_UART_STATE_READY;
    }

    /* UART noise error interrupt occured --------------------------------------*/
    if((__HAL_UART_GET_IT(huart, UART_IT_NE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR) != RESET))
    {
        __HAL_UART_CLEAR_IT(huart, UART_CLEAR_NEF);

        huart->ErrorCode |= HAL_UART_ERROR_NE;
        /* Set the UART state ready to be able to start again the process */
        huart->gState = HAL_UART_STATE_READY;
        huart->RxState = HAL_UART_STATE_READY;
    }

    /* UART Over-Run interrupt occurred -----------------------------------------*/
    if((__HAL_UART_GET_IT(huart, UART_IT_ORE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR) != RESET))
    {
        __HAL_UART_CLEAR_IT(huart, UART_CLEAR_OREF);

        huart->ErrorCode |= HAL_UART_ERROR_ORE;
        /* Set the UART state ready to be able to start again the process */
        huart->gState = HAL_UART_STATE_READY;
        huart->RxState = HAL_UART_STATE_READY;
    }

    /* Call UART Error Call back function if need be --------------------------*/
    if(huart->ErrorCode != HAL_UART_ERROR_NONE)
    {
        /* Set the UART state ready to be able to start again the process */
        huart->gState = HAL_UART_STATE_READY;
        huart->RxState = HAL_UART_STATE_READY;

        HAL_UART_ErrorCallback(huart);
    }

    /* UART Wake Up interrupt occured ------------------------------------------*/
    if((__HAL_UART_GET_IT(huart, UART_IT_WUF) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_WUF) != RESET))
    {
        __HAL_UART_CLEAR_IT(huart, UART_CLEAR_WUF);

        /* Set the UART state ready to be able to start again the process */
        huart->gState = HAL_UART_STATE_READY;
        huart->RxState = HAL_UART_STATE_READY;

        HAL_UARTEx_WakeupCallback(huart);
    }
}


/******************************************************************************/
/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA stream
  *         used for USART data transmission
  */
void USART1_IRQHandler(void)
{
    UART_HandleTypeDef *huart = &UartHandle;

    /* UART in mode Receiver ---------------------------------------------------*/
    if((__HAL_UART_GET_IT(huart, UART_IT_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE) != RESET))
    {
        __HAL_UART_CLEAR_IT(huart, UART_IT_RXNE);
        huart->RxState = HAL_UART_STATE_READY;

        uart1.p_rx_buf[rx1_len++]=(uint8_t)(USART1->RDR);

    }

    /* UART_IT_IDLE interrupt occured ------------------------------------------*/
    if((__HAL_UART_GET_IT(huart, UART_IT_IDLE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE) != RESET))
    {
        __HAL_UART_CLEAR_IT(huart, UART_CLEAR_IDLEF);
        /* Set the UART state ready to be able to start again the process */
        huart->RxState = HAL_UART_STATE_READY;

        uart1.rx_len = rx1_len;
        uart1.uart_RxDone_flag = 1;
        rx1_len = 0;

        if(uart1.rx_len != 0 )
        {
            if(uart1.rx_complete_callback != NULL)
            {
                uart1.rx_complete_callback();
            }
        }
    }

    error_process(huart);
}

#ifndef USE_LOG_PRINTF
int invalid_LOG_PRINTF(const char *format,...)
{
    return 0;
}
#endif

int fputc(int ch, FILE *f)
{
    while( HAL_USART_GET_FLAG(USART1,UART_FLAG_TC)== RESET);

    USART1->TDR =  (uint8_t)ch;
    return ch;
}
