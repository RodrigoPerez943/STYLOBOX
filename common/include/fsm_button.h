/**
 * @file fsm_button.h
 * @brief Header for fsm_button.c file.
 * @author Rodrigo Pérez del Pulgar Almodóvar
 * @author David Hernández Sola
 * @date 22/03/2023
 */

#ifndef FSM_BUTTON_H_
#define FSM_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* Other includes */
#include <fsm.h>

/* Defines and enums ----------------------------------------------------------*/
/* Enums */

/// @brief Enumerator for the button finite state machine.
enum FSM_BUTTON{
    BUTTON_RELEASED = 0,  /*!< Starting state. Also comes here when the button has been released */
    BUTTON_RELEASED_WAIT,  /*!< State to perform the anti-debounce mechanism for a falling edge*/
    BUTTON_PRESSED,  /*!< State while the button is being pressed */
    BUTTON_PRESSED_WAIT  /*!< State to perform the anti-debounce mechanism for a rising edge */
};

/* Typedefs --------------------------------------------------------------------*/

/// @brief Structure to define the Button FSM
typedef struct 
{
    fsm_t f; /*!< Button FSM*/
    uint32_t debounce_time; /*!< Button debounce time in ms*/
    uint32_t next_timeout; /*!< Next timeout for the anti-debounce in ms*/
    uint32_t tick_pressed; /*!<Number of ticks when the button was pressed */
    uint32_t duration; /*!<How much time the button has been pressed */
    uint32_t button_id; /*!< Button ID. Must be unique. */
} fsm_button_t;


/* Function prototypes and explanation -------------------------------------------------*/

/// @brief Create a new button FSM.
/// @param debounce_time Anti-debounce time in milliseconds
/// @param button_id Unique button identifier number
/// @return A pointer to the button FSM
fsm_t * fsm_button_new ( uint32_t debounce_time, uint32_t button_id );

/// @brief Initialize a button FSM.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_button_t
/// @param debounce_time Anti-debounce time in milliseconds
/// @param button_id 	Unique button identifier number
void fsm_button_init ( fsm_t *p_this , uint32_t debounce_time, uint32_t button_id  );

/// @brief Return the duration of the last button press.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
/// @return uint32_t
uint32_t fsm_button_get_duration ( fsm_t *p_this );

/// @brief Reset the duration of the last button press.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
void fsm_button_reset_duration ( fsm_t *p_this );

/// @brief Check if the button FSM is active, or not.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
/// @return returns a boolean
bool fsm_button_check_activity(fsm_t *p_this);

#endif
