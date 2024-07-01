/**
 * @file port_led.h
 * @brief Header file for port_led.c.
 * @author Román Cárdenas, Alberto Boscá, Josué Pagán (r.cardenas@upm.es, j.pagan@upm.es, alberto.bosca@upm.es)
 * @date 2024-01-01
 */

#ifndef PORT_LED_H_
#define PORT_LED_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdbool.h>

/* Function prototypes and explanation -------------------------------------------------*/
/**
 * @brief configures the GPIO pin of the LED.
 */
void port_led_gpio_setup(void);

/**
 * @brief toggles the LED ON
 */
void port_led_toggle_on(void);


/**
 * @brief toggles the LED OFF
 */
void port_led_toggle_off(void);

#endif // PORT_LED_H_
