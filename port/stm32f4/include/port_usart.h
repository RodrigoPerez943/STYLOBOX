/**
 * @file port_usart.h
 * @brief Header for port_usart.c file.
 * @author Rodrigo Pérez del Pulgar Almodóvar
 * @author David Hernández Sola
 * @date 22/03/2023
 */
#ifndef PORT_USART_H_
#define PORT_USART_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

/* HW dependent includes */

#define USART_0_ID 0 /*!< USARTidentifier */
#define USART_0 USART3 /*!< USART used connected to the GPIO */ 
#define USART_0_GPIO_TX GPIOB /*!< USART GPIO port for TX pin */
#define USART_0_GPIO_RX GPIOC /*!< USART GPIO port for RX pin */
#define USART_0_PIN_TX 10 /*!< USART GPIO pin for TX */
#define USART_0_PIN_RX 11 /*!< USART GPIO pin for RX */
#define USART_0_AF_TX 7 /*!< USART alternate function for TX */
#define USART_0_AF_RX 7 /*!< USART alternate function for RX */
#define USART_INPUT_BUFFER_LENGTH 10 /*!< USART input message length */
#define USART_OUTPUT_BUFFER_LENGTH 100 /*!< USART output message length */
#define EMPTY_BUFFER_CONSTANT 0x0 /*!< Empty char constant */
#define END_CHAR_CONSTANT 0xA /*!< End char constant */

/* Defines and enums ----------------------------------------------------------*/
/* Defines */

/* Typedefs --------------------------------------------------------------------*/

/// @brief Structure to define the HW dependencies of a USART.
typedef struct
{
    USART_TypeDef *p_usart; /*!< USART peripheral*/
    GPIO_TypeDef *p_port_tx; /*!< GPIO where the USART TX is connected*/
    GPIO_TypeDef *p_port_rx; /*!< GPIO where the USART RX is connected */
    uint8_t pin_tx; /*!< Pin/line where the USART TX is connected*/
    uint8_t pin_rx; /*!< Pin/line where the USART RX is connected*/
    uint8_t alt_func_tx; /*!< Alternate function for the TX pin*/
    uint8_t alt_func_rx; /*!< Alternate function for the RX pin */
    char input_buffer[USART_INPUT_BUFFER_LENGTH]; /*!< Input buffer */
    uint8_t i_idx; /*!< Index to the input buffer */
    bool read_complete; /*!< Flag to indicate that the data has been read. */
    char output_buffer[USART_OUTPUT_BUFFER_LENGTH]; /*!< Output buffer*/
    uint8_t o_idx; /*!< Index to the output buffer */
    bool write_complete; /*!< Flag to indicate that the data has been sent. */
} port_usart_hw_t;

/* Global variables */

/// @brief Array of elements that represents the HW characteristics of the USARTs.
extern port_usart_hw_t usart_arr[];

/* Function prototypes and explanation -------------------------------------------------*/

/// @brief Configure the HW specifications of a given USART.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_init(uint32_t usart_id);

/// @brief Check if a transmission is complete.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
/// @return Return the value of the write_complete field of the usart_arr[] element, boolean
bool port_usart_tx_done(uint32_t usart_id);

/// @brief Check if a reception is complete.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
/// @return Return the value of the read_complete field of the usart_arr[] element, boolean
bool port_usart_rx_done(uint32_t usart_id);

/// @brief Get the message received through the USART and store it in the buffer passed as argument.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
/// @param p_buffer Pointer to the buffer where the message will be stored.
void port_usart_get_from_input_buffer(uint32_t usart_id, char *p_buffer);

/// @brief Check if the USART is ready to receive a new message.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
/// @return  Return the status of the TXE flag, boolean
bool port_usart_get_txr_status(uint32_t usart_id);

/// @brief Copy the message passed as argument to the output buffer of the USART.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
/// @param p_data Pointer to the message to send. 
/// @param length Length of the message to send.
void port_usart_copy_to_output_buffer(uint32_t usart_id, char *p_data, uint32_t length);

/// @brief 
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_reset_input_buffer(uint32_t usart_id);

/// @brief 
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_reset_output_buffer(uint32_t usart_id);

/// @brief Reset the input buffer of the USART.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_store_data(uint32_t usart_id);

/// @brief Function to write the data from the output buffer to the USART Data Register.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_write_data(uint32_t usart_id);

/// @brief Disable USART RX interrupt.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_disable_rx_interrupt(uint32_t usart_id);

/// @brief Disable USART TX interrupt.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_disable_tx_interrupt(uint32_t usart_id);

/// @brief Enable USART RX interrupt.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_enable_rx_interrupt(uint32_t usart_id);

/// @brief Enable USART TX interrupt.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_enable_tx_interrupt(uint32_t usart_id);

#endif