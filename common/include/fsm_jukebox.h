/**
 * @file fsm_jukebox.h
 * @brief Header for fsm_jukebox.c file.
 * @author David Hernández Sola
 * @author Rodrigo Pérez del Pulgar Almodovar
 * @date fecha
 */
#ifndef FSM_JUKEBOX_H_
#define FSM_JUKEBOX_H_


/* Includes ------------------------------------------------------------------*/
/* Standard C includes */

#include <stdint.h>
#include <fsm.h>
#include "melodies.h"

/* Other includes */


/* Defines and enums ----------------------------------------------------------*/
/* Defines */

#define MELODIES_MEMORY_SIZE 10

/* Enums */

/// @brief This enumerator defines the different states that the Jukebox finite state machine can be in. Each state represents a specific condition or step in the playing process of the Jukebox.
enum  FSM_JUKEBOX {
  OFF = 0,
  START_UP,
  WAIT_COMMAND,
  SLEEP_WHILE_OFF,
  SLEEP_WHILE_ON
};

/* Typedefs ------------------------------------------------------------------*/

/// @brief Structure that contains the information of a melody
typedef struct 
{
    fsm_t f;
    melody_t melodies[MELODIES_MEMORY_SIZE];
    uint8_t melody_idx;
    char *p_melody;
    fsm_t *p_fsm_button;
    uint32_t on_off_press_time_ms;
    fsm_t *p_fsm_usart;
    fsm_t *p_fsm_buzzer;
    uint32_t next_song_press_time_ms;
    double speed;
} fsm_jukebox_t;

/* Function prototypes and explanation ---------------------------------------*/



/// @brief Creates a new jukebox FSM.This FSM plays melodies using a buzzer and communicates via USART with commands:play`: Play the current melody;`pause`: Pause/resume the melody; `stop`: Stop the melody; `play` restarts it;`next`: Play the next melody;`speed`: Change melody speed (requires parameter > 0.1);`select`: Select a melody by number;`info`: Get current melody info.
/// @param p_fsm_button Pointer to the button FSM
/// @param on_off_press_time_ms Button press time in milliseconds to turn the system ON or OFF 
/// @param p_fsm_usart Pointer to the USART FSM
/// @param p_fsm_buzzer Pointer to the buzzer FSM.
/// @param next_song_press_time_ms Button press time in milliseconds to change to the next song.
/// @return A pointer to the button FSM
fsm_t *fsm_jukebox_new (fsm_t *p_fsm_button, uint32_t on_off_press_time_ms, fsm_t *p_fsm_usart, fsm_t *p_fsm_buzzer, uint32_t next_song_press_time_ms);

/// @brief Initializes the jukebox finite state machine (FSM). This function sets the default values for the jukebox FSM structure and initializes the associated hardware.
/// @param p_this Pointer to the jukebox FSM structure.
/// @param p_fsm_button Pointer to the button FSM.
/// @param on_off_press_time_ms Time in milliseconds to press the on/off button. 
/// @param p_fsm_usart Pointer to the USART FSM.
/// @param p_fsm_buzzer Pointer to the buzzer FSM.
/// @param next_song_press_time_ms Time in milliseconds to press the next song button.
void fsm_jukebox_init (fsm_t *p_this, fsm_t *p_fsm_button, uint32_t on_off_press_time_ms, fsm_t *p_fsm_usart, fsm_t *p_fsm_buzzer, uint32_t next_song_press_time_ms);

#endif /* FSM_JUKEBOX_H_ */