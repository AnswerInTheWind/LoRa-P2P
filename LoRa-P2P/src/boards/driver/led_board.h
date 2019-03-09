/*
    2018 Grope

Description: key driver implementation, used for KEY.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Derek Chen
*/

#ifndef __LED_BOARD_H__

#define LED_GREEN_PINNAME   PC_13
#define LED_RED_PINNAME     PA_0

#define LED_ON      GPIO_PIN_SET
#define LED_OFF      GPIO_PIN_RESET

extern Gpio_t led_green;
extern Gpio_t led_red;

extern void led_Init(void);

extern void led_DeInit(void);

extern void led_set(Gpio_t* led,GPIO_PinState pinState);

extern void led_toggle(Gpio_t* led);

#endif
