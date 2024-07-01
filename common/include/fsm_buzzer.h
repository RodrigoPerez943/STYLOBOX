/**
 * @file fsm_buzzer.h
 * @brief Header for fsm_buzzer.c file.
 * @author David Hernández Sola
 * @author Rodrigo Pérez del Pulgar Almodovar
 * @date fecha
 */

#ifndef FSM_BUZZER_H_
#define FSM_BUZZER_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>
#include <fsm.h>
#include "melodies.h"

/* Other includes */

/* HW dependent includes */


/* Defines and enums ----------------------------------------------------------*/
/* Enums */

/// @brief Enumerator for the buzzer finite state machine.
enum FSM_BUZZER {
  WAIT_START = 0,
  WAIT_NOTE,
  PLAY_NOTE,
  WAIT_MELODY,
  PAUSE_NOTE
};
/// @brief Enumeration of possible states actions on the player, either from the user or from the player itself.
enum  USER_ACTIONS {
  STOP = 0,
  PLAY,
  PAUSE
};

/* Typedefs --------------------------------------------------------------------*/

/// @brief Structure to define the Buzzer FSM
typedef struct 
{
    fsm_t f;
    melody_t * p_melody;
    uint32_t note_index;
    uint8_t buzzer_id;
    uint8_t user_action;
    double player_speed;
} fsm_buzzer_t;

/* Function prototypes and explanation -------------------------------------------------*/

/// @brief  This function sets the melody to play. The user must pass a pointer to the melody to play.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t struct
/// @param p_melody Pointer to the melody to play
void fsm_buzzer_set_melody (fsm_t *p_this, const melody_t *p_melody);
 
/// @brief This function sets the speed of the player. The user must pass a double value with the speed of the player.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t struct
/// @param speed  Speed of the player
void fsm_buzzer_set_speed (fsm_t *p_this, double speed);


/// @brief Sets the player's action based on a USER_ACTIONS value.his function sets the action for the player (e.g., stop, pause, start) using a USER_ACTIONS flag.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t struct
/// @param action Action to perform on the player
void fsm_buzzer_set_action (fsm_t *p_this, uint8_t action);
 
/// @brief This function returns the action currently being performed on the player,indicating whether the player is playing, paused, stopped, or if a melody has ended.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t struct
/// @return  Return the field user_action
uint8_t fsm_buzzer_get_action (fsm_t *p_this);
 
/// @brief Creates a new buzzer finite state machine.
/// @param buzzer_id Unique buzzer identifier number 
/// @return A pointer to the new buzzer finite state machine

fsm_t * fsm_buzzer_new (uint32_t buzzer_id);
 
/// @brief This function initializes the default values of the FSM struct and calls to the port to initialize the HW of associated to the given ID.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t struct
/// @param buzzer_id Unique buzzer identifier number
void fsm_buzzer_init (fsm_t *p_this, uint32_t buzzer_id);

/// @brief Check if the buzzer finite state machine is playing a melody.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_buzzer_t struct 
/// @return  True if the player is playing or paused. False if the player is stopped.
bool fsm_buzzer_check_activity (fsm_t *p_this);

#endif /* FSM_BUZZER_H_ */