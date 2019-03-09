/*
    2018 Grope

Description: key driver implementation, used for KEY.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Derek Chen
*/

#include "board.h"
#include "led_board.h"

Gpio_t led_green;
Gpio_t led_red;

void led_Init(void)
{
    //初始化LED
    GpioInit(&led_green,LED_GREEN_PINNAME,PIN_OUTPUT,PIN_PUSH_PULL,PIN_PULL_UP,0);

    GpioInit(&led_red,LED_RED_PINNAME,PIN_OUTPUT,PIN_PUSH_PULL,PIN_PULL_UP,0);
}

void led_DeInit(void)
{
    GpioWrite(&led_green,GPIO_PIN_RESET);
    GpioWrite(&led_red,GPIO_PIN_RESET);
    __GPIOA_CLK_DISABLE();
    __GPIOC_CLK_DISABLE();
}

void led_set(Gpio_t* led,GPIO_PinState pinState)
{
    GpioWrite(led,pinState);
}

void led_toggle(Gpio_t* led)
{
    GpioToggle(led);
}

