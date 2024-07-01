/**
 * @file port_button.h
 * @brief Header for port_button.c file.
 * @author Rodrigo Pérez del Pulgar Almodóvar
 * @author David Hernández Sola
 * @date 22/03/2023
 */

#ifndef PORT_BUTTON_H_
#define PORT_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>
#include <port_system.h>

/* HW dependent includes */

/* Defines and enums ----------------------------------------------------------*/
/* Defines */

#define BUTTON_0_ID 0 /*!< Button identifier*/
#define BUTTON_0_GPIO GPIOC /*!< Button GPIO port*/
#define BUTTON_0_PIN 13 /*!< Button GPIO pin*/
#define BUTTON_0_DEBOUNCE_TIME_MS 150 /*!< Button debounce time */

/* Typedefs --------------------------------------------------------------------*/

/// @brief Structure to define the HW dependencies of a button.
typedef struct
{
    GPIO_TypeDef *p_port; /*!< GPIO where the button is connected*/
    uint8_t pin; /*!< Pin/line where the button is connected */
    bool flag_pressed; /*!< Flag to indicate that the button has been pressed*/
} port_button_hw_t; 

/* Global variables */

/// @brief global variable to define a HW dependencies of a button.
extern port_button_hw_t buttons_arr[];

/* Function prototypes and explanation -------------------------------------------------*/

/// @brief Configure the HW specifications of a given button.
/// @param button_id Button ID. This index is used to select the element of the buttons_arr[] array
void port_button_init (uint32_t button_id);

/// @brief Return the status of the button (pressed or not)
/// @param button_id Button ID. This index is used to select the element of the buttons_arr[] array
/// @return true If the button has been pressed , false If the button has not been pressed , boolean
bool port_button_is_pressed (uint32_t button_id);

/// @brief Return the count of the System tick in milliseconds.
/// @return uint32_t
uint32_t port_button_get_tick ();

#endif
