/*
    2018 Grope

Description: Key driver implementation, used for key press.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Derek Chen
*/
#include <stdio.h>
#include "board.h"
#include "key_board.h"


#ifdef USE_KEY
Gpio_t key;

bool is_debounced = false;

//* Private function prototypes -----------------------------------------------*/
void key_interrupt_callback(void);
void (*application_notification)(void);

/* Private functions ---------------------------------------------------------*/
void key_init( void )
{
    //初始化key为输入/上拉
    GpioInit(&key,KEY_PINNAME,PIN_INPUT,PIN_PUSH_PULL,PIN_PULL_UP,1);

    //设置key为中断/上升沿触发/次高优先级,回调函数为key_interrupt_callback
    GpioSetInterrupt( &key, IRQ_FALLING_EDGE, IRQ_HIGH_PRIORITY, key_interrupt_callback );
    
    application_notification = NULL;
}

void key_DeInit( void )
{
    GpioInit( &key, KEY_PINNAME, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0 );
    __GPIOA_CLK_DISABLE();
}

uint32_t key_readPin(void)
{
    return GpioRead(&key);
}

//按键的回调函数
void key_interrupt_callback(void)
{
    if(application_notification != NULL)
        application_notification();
}


void key_register_callback(void (*fun)(void))
{
    application_notification = fun;
}

#endif
