/**
 * @file port_button.c
 * @brief File containing functions related to the HW of the button.
 *
 * This file defines an internal struct which contains the HW information of the button.
 *
 * @author Rodrigo Pérez del Pulgar Almodóvar
 * @author David Hernández Sola
 * @date 22/03/2023
 */

/* Includes ------------------------------------------------------------------*/

#include "port_button.h"

/* Global variables ------------------------------------------------------------*/

/// @brief Array of elements that represents the HW characteristics of the buttons.
port_button_hw_t buttons_arr[] = {
    [BUTTON_0_ID] = {.p_port = BUTTON_0_GPIO, .pin = BUTTON_0_PIN, .flag_pressed = false},
};

/// @brief Configure the HW specifications of a given button.
/// Call function port_system_gpio_config() with the right arguments to configure the button as input and no pull up neither pull down connection
/// Call function port_system_gpio_config_exti() with the right arguments to configure interruption mode in both rising and falling edges, and enable the interrupt request
/// Call function port_system_gpio_exti_enable() with the right parameters to enable the interrupt line and set the priority level to 1 and the subpriority level to 0
/// @param button_id Button ID. This index is used to select the element of the buttons_arr[] array.
void port_button_init(uint32_t button_id)
{
    GPIO_TypeDef *p_port = buttons_arr[button_id].p_port;
    uint8_t pin = buttons_arr[button_id].pin;

    port_system_gpio_config(p_port,pin,GPIO_MODE_IN,GPIO_PUPDR_NOPULL);
    port_system_gpio_config_exti(p_port,pin,TRIGGER_BOTH_EDGE | TRIGGER_ENABLE_INTERR_REQ);
    port_system_gpio_exti_enable(pin,1,0);
}

/// @brief Return the status of the button (pressed or not)
/// @param button_id Button ID. This index is used to select the element of the buttons_arr[] array.
/// @return Return the value of the field flag_pressed , boolean
bool port_button_is_pressed (uint32_t button_id){
    return buttons_arr[button_id].flag_pressed;
};

/// @brief Return the count of the System tick in milliseconds.
/// @return Return the count of the System tick in milliseconds, uint32_t
uint32_t port_button_get_tick (){
    return port_system_get_millis();
};