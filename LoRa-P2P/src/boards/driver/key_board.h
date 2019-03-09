#ifndef __KEY_BOARD_H__
#define __KEY_BOARD_H__

#define KEY_PINNAME         PA_8

extern bool is_debounced;

/*!
 * \brief key init
 */
extern void key_init(void);

/*!
 * \brief key Deinit
 */
extern void key_DeInit( void );

/*!
 * \brief read key volatage level
 */
extern uint32_t key_readPin(void);

/*!
 * \brief register key callback
 */
extern void key_register_callback(void (*fun)(void));

#endif
