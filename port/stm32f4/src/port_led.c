/**
 * @file port_led.h
 * @brief Portable functions to interact with the LED of the STM32F4 platform.
 * @author Román Cárdenas, Alberto Boscá, Josué Pagán (r.cardenas@upm.es, j.pagan@upm.es, alberto.bosca@upm.es)
 * @date 2024-01-01
 */
/* Includes ------------------------------------------------------------------*/
/* HW dependent includes */
#include "port_led.h"
#include "port_system.h"
#include "stm32f4xx.h"

/* Defines -------------------------------------------------------------------*/
#define LD2_PIN 5           /*!< LD2 pin number */
#define LD2_GPIO_PORT GPIOA /*!< LD2 GPIO port */

#define MODER5_MASK (GPIO_MODER_MODE0 << LD2_PIN * 2) /*!< Mask for MODER register using LD2_PIN */
#define PUPDR5_MASK (GPIO_PUPDR_PUPD0 << LD2_PIN * 2) /*!< Mask for PUPDR register using LD2_PIN */

#define MODER5_AS_OUTPUT (GPIO_MODE_OUT << LD2_PIN * 2)     /*!< LD2 pin as output */
#define PUPDR5_AS_NOPUPD (GPIO_PUPDR_NOPULL << LD2_PIN * 2) /*!< LD2 pin as no pull-up nor pull-down */

#define IDR5_MASK (GPIO_IDR_ID0 << LD2_PIN) /*!< Mask for IDR register using LD2_PIN */
#define ODR5_MASK (GPIO_ODR_OD0 << LD2_PIN) /*!< Mask for ODR register using LD2_PIN */

 /// @brief configures the GPIO pin of the LED.
 void port_led_gpio_setup ( void )
 {
    RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN ;
    LD2_GPIO_PORT -> MODER &= ~ MODER5_MASK ;
    LD2_GPIO_PORT -> PUPDR &= ~ PUPDR5_MASK ;
    LD2_GPIO_PORT -> MODER |= MODER5_AS_OUTPUT ;
    LD2_GPIO_PORT -> PUPDR |= PUPDR5_AS_NOPUPD ;
 }

/**
 * @brief toggles the LED ON
 */
 void port_led_toggle_on (){
    LD2_GPIO_PORT -> ODR |= ODR5_MASK ; /* Encender : escribimos un 1 logico */
}

/**
 * @brief toggles the LED OFF
 */
 void port_led_toggle_off (){
    LD2_GPIO_PORT -> ODR &= ~ ODR5_MASK ; /* Apagar : escribimos un 0 logico */
}
 
