/*
  ______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Target board general functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#ifndef __BOARD_H__
#define __BOARD_H__


#define USE_UART
#define USE_KEY
#define USE_LED
#define USE_OLED
#define xUSE_LOW_POWER_MODE//测试开发板功耗为1.2uA,xUSE_UART xUSE_KEY xUSE_LED xUSE_OLED xUSE_LOG_PRINTF
#define USE_MSG_DBG


#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32l0xx.h"
#include "stm32l0xx_hal.h"
#include "utilities.h"

#include "gpio.h"
#include "spi.h"
#include "timer.h"
#include "rtc-board.h"
#include "radio.h"

#include "delay.h"
#include "uart_board.h"

#include "sx1276-board.h"
#include "sx1276/sx1276.h"


#ifdef USE_BATTERY_DETECTION
    #include "battery.h"
#endif

#ifdef USE_KEY
    #include "key_board.h"
#endif

#ifdef USE_LED
    #include "led_board.h"
#endif

#ifdef USE_OLED
    #include "oled_board.h"
#endif

/*!
 * Generic definition
 */
#ifndef SUCCESS
    #define SUCCESS                                     1
#endif

#ifndef FAIL
    #define FAIL                                        0
#endif

/*!
 * Board MCU pins definition
 */
#define RADIO_NSS                                   PA_4
#define RADIO_SCLK                                  PA_5
#define RADIO_MISO                                  PA_6
#define RADIO_MOSI                                  PA_7

#define RADIO_RESET                                 PB_10

#define RADIO_DIO_0                                 PB_0
#define RADIO_DIO_1                                 PB_1
#define RADIO_DIO_3                                 PB_2    


#ifdef USE_MSG_DBG
    #define LOG_PRINTF(args...) printf(args)

#else
    #define LOG_PRINTF(args...)
#endif

#define MSG_DBG(format, ...) LOG_PRINTF("[ %d ] "format"\r\n",HAL_GetTick(),##__VA_ARGS__)
#define LOG_PRINTF_Arrays(data,len)         \
do                                          \
{                                           \
    for(int z = 0;z<len;z++)                \
        LOG_PRINTF("%02x ",data[z]);        \
    LOG_PRINTF("\r\n");                     \
}while(0)     


/*!
 * \brief Disable interrupts
 *
 * \remark IRQ nesting is managed
 */
void BoardDisableIrq( void );

/*!
 * \brief Enable interrupts
 *
 * \remark IRQ nesting is managed
 */
void BoardEnableIrq( void );

/*!
 * board reinit after wake up
 *
 */
void HAL_MspWakeUp(void);

#endif // __BOARD_H__
