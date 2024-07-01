/**
 * @file port_buzzer.h
 * @brief Header for port_buzzer.c file.
 * @author David Hernández Sola
 * @author Rodrigo Pérez del Pulgar Almodovar
 * @date fecha
 */
#ifndef PORT_BUZZER_H_
#define PORT_BUZZER_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>
#include "port_system.h"


/* HW dependent includes */

/* Defines and enums ----------------------------------------------------------*/
/* Defines */

#define BUZZER_0_ID 0
#define BUZZER_0_GPIO GPIOA
#define BUZZER_0_PIN 6
#define BUZZER_PWM_DC 0.5 

/* Typedefs --------------------------------------------------------------------*/
/// @brief Structure to define the HW dependencies of a buzzer melody player.
typedef struct
{
    GPIO_TypeDef *p_port;
    uint8_t pin;
    uint8_t alt_func;
    bool note_end;
}port_buzzer_hw_t;

/* Global variables */

/// @brief Array of elements that represents the HW characteristics of the buzzers.
extern port_buzzer_hw_t buzzers_arr[];

/* Function prototypes and explanation -------------------------------------------------*/

/// @brief Configure the HW specifications of a given buzzer melody player.
/// @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
void port_buzzer_init(uint32_t buzzer_id);

/// @brief Set the duration of the timer that controls the duration of the note.
/// @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
/// @param duration_ms Duration of the note in ms
void port_buzzer_set_note_duration(uint32_t buzzer_id, uint32_t duration_ms);

/// @brief Set the PWM frequency of the timer that controls the frequency of the note.
/// @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
/// @param frequency_hz Frequency of the note in Hz
void port_buzzer_set_note_frequency(uint32_t buzzer_id, double frequency_hz);

/// @brief Retrieve the status of the note end flag.
/// @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
/// @return true;false
bool port_buzzer_get_note_timeout(uint32_t buzzer_id);

/// @brief Disable the PWM output of the timer that controls the frequency of the note and the timer that controls the duration of the note.
/// @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
void port_buzzer_stop(uint32_t buzzer_id);



#endif