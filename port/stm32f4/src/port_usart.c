/**
 * @file port_usart.c
 * @brief Portable functions to interact with the USART FSM library.
 * @author Rodrigo Pérez del Pulgar Almodóvar
 * @author David Hernández Sola
 * @date 22/03/2023
 */
/* Includes ------------------------------------------------------------------*/
/* Standard C libraries */

#include <string.h>
#include <stdlib.h>
#include "port_system.h"
#include "port_usart.h"

/* HW dependent libraries */

/* Global variables */

/// @brief Structure to define the HW dependencies of a USART.
port_usart_hw_t usart_arr[] = {

    [USART_0_ID] = {
        .p_usart = USART_0, /*!< USART peripheral */
        .p_port_tx = USART_0_GPIO_TX, /*!< GPIO where the USART TX is connected*/
        .p_port_rx = USART_0_GPIO_RX, /*!< GPIO where the USART RX is connected*/
        .pin_tx = USART_0_PIN_TX, /*!< Pin/line where the USART TX is connected*/
        .pin_rx = USART_0_PIN_RX, /*!< Pin/line where the USART RX is connected*/
        .alt_func_tx = USART_0_AF_TX, /*!< Alternate function for the TX pin */
        .alt_func_rx = USART_0_AF_RX, /*!< Alternate function for the RX pin */
        .i_idx = 0, /*!< Index to the input buffer */
        .read_complete = false, /*!< Flag to indicate that the data has been read.*/
        .o_idx = 0, /*!< Index to the output buffer */
        .write_complete = false, /*!< Flag to indicate that the data has been sent.*/
    }};

/* Private functions */

/* Public functions */

/// @brief Reset a buffer to a default value.
/// @param buffer Pointer to the buffer to be reset
/// @param length Length of the buffer 
void _reset_buffer(char *buffer , uint32_t length){ 
    memset(buffer, EMPTY_BUFFER_CONSTANT , length);
};

/// @brief Get the message received through the USART and store it in the buffer passed as argument.
// Copy the content of the input_buffer field of the usart_arr[] element to the buffer passed as argument.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
/// @param p_buffer Pointer to the buffer where the message will be stored.
void port_usart_get_from_input_buffer(uint32_t usart_id, char *p_buffer){
  memcpy(p_buffer, usart_arr[usart_id].input_buffer , USART_INPUT_BUFFER_LENGTH);
};

/// @brief Check if the USART is ready to receive a new message.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
/// @return Return the status of the TXE flag, boolean
bool port_usart_get_txr_status(uint32_t usart_id){
    return (usart_arr[usart_id].p_usart ->SR) & USART_SR_TXE; 
};

/// @brief Copy the message passed as argument to the output buffer of the USART.
// Copy the content of the buffer passed as argument to the output_buffer field of the usart_arr[] element
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
/// @param p_data Pointer to the message to send.
/// @param length Length of the buffer 
void port_usart_copy_to_output_buffer(uint32_t usart_id, char *p_data, uint32_t length){
    memcpy(usart_arr[usart_id].output_buffer, p_data , length);
};

/// @brief Reset the input buffer of the USART.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_reset_input_buffer(uint32_t usart_id){
    _reset_buffer(usart_arr[usart_id].input_buffer, USART_INPUT_BUFFER_LENGTH);
    usart_arr[usart_id].read_complete = false;
};

/// @brief Reset the output buffer of the USART.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_reset_output_buffer(uint32_t usart_id){
    _reset_buffer(usart_arr[usart_id].output_buffer, USART_OUTPUT_BUFFER_LENGTH);
    usart_arr[usart_id].write_complete = false;
};

/// @brief Check if a reception is complete.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
/// @return  Return the value of the read_complete field of the usart_arr[] element, boolean.
bool port_usart_rx_done(uint32_t usart_id){
    return usart_arr[usart_id].read_complete;
};
/// @brief Check if a transmission is complete.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
/// @return Return the value of the write_complete field of the usart_arr[] element.
bool port_usart_tx_done(uint32_t usart_id){
    return usart_arr[usart_id].write_complete;
};

/// @brief Function to read the data from the USART Data Register and store it in the input buffer.
/// This function is called from the ISR USART3_IRQHandler() when the RXNE flag is set.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_store_data(uint32_t usart_id) {
    char data = usart_arr[usart_id].p_usart -> DR;

    if (data != END_CHAR_CONSTANT) {
        uint8_t idx = usart_arr[usart_id].i_idx;

        if (idx >= USART_INPUT_BUFFER_LENGTH) {
            usart_arr[usart_id].i_idx = 0;
        };
        usart_arr[usart_id].input_buffer[idx] = data;
        usart_arr[usart_id].i_idx++;
    } else {
        usart_arr[usart_id].read_complete = true;
        usart_arr[usart_id].i_idx = 0;
    };
};

/// @brief Function to write the data from the output buffer to the USART Data Register.
/// This function is called from the ISR USART3_IRQHandler() when the TXE flag is set.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_write_data(uint32_t usart_id){
    uint8_t idx = usart_arr[usart_id].o_idx;
    uint32_t data = usart_arr[usart_id].output_buffer[idx];

    if (idx == USART_OUTPUT_BUFFER_LENGTH-1 || data == END_CHAR_CONSTANT) {
        usart_arr[usart_id].p_usart -> DR = data;
        port_usart_disable_tx_interrupt(usart_id);
        usart_arr[usart_id].o_idx = 0;
        usart_arr[usart_id].write_complete = true;

    } else if (data != EMPTY_BUFFER_CONSTANT) {
        usart_arr[usart_id].p_usart -> DR = data;
        usart_arr[usart_id].o_idx++;
    }
};

/// @brief Disable USART RX interrupt.
/// Disable the reception interrupt (RXNEIE) bit of the CR1 register of the USART peripheral
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_disable_rx_interrupt(uint32_t usart_id){
    USART_TypeDef *p_usart = usart_arr[usart_id].p_usart;
    p_usart->CR1 &= ~USART_CR1_RXNEIE;
};

/// @brief Disable USART TX interrupts.
/// Disable the transmission buffer empty (TXEIE) interrupt bit of the CR1 register of the USART peripheral
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_disable_tx_interrupt(uint32_t usart_id){
    USART_TypeDef *p_usart = usart_arr[usart_id].p_usart;
    p_usart->CR1 &= ~USART_CR1_TXEIE;
};

/// @brief Enable USART RX interrupt.
/// Enable the reception interrupt (RXNEIE) bit of the CR1 register of the USART peripheral
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_enable_rx_interrupt(uint32_t usart_id){
    USART_TypeDef *p_usart = usart_arr[usart_id].p_usart;
    p_usart->CR1 |= USART_CR1_RXNEIE;
};


/// @brief Enable USART TX interrupts.
/// Enable the transmission buffer empty (TXEIE) interrupt bit of the CR1 register of the USART peripheral.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_enable_tx_interrupt(uint32_t usart_id){
    USART_TypeDef *p_usart = usart_arr[usart_id].p_usart;
    p_usart->CR1 |= USART_CR1_TXEIE;
};

/// @brief Configure the HW specifications of a given USART.
/// @param usart_id USART ID. This index is used to select the element of the usart_arr[] array. 
void port_usart_init(uint32_t usart_id){

    USART_TypeDef *p_usart = usart_arr[usart_id].p_usart;
    GPIO_TypeDef *p_port_tx = usart_arr[usart_id].p_port_tx;
    GPIO_TypeDef *p_port_rx = usart_arr[usart_id].p_port_rx;
    uint8_t pin_tx = usart_arr[usart_id].pin_tx;
    uint8_t pin_rx = usart_arr[usart_id].pin_rx;
    uint8_t alt_func_tx = usart_arr[usart_id].alt_func_tx;
    uint8_t alt_func_rx = usart_arr[usart_id].alt_func_rx;

    /* TO-DO alumnos: */
    port_system_gpio_config(p_port_tx, pin_tx, GPIO_MODE_ALTERNATE, GPIO_PUPDR_PUP);
    port_system_gpio_config(p_port_rx, pin_rx, GPIO_MODE_ALTERNATE, GPIO_PUPDR_PUP);
    port_system_gpio_config_alternate(p_port_tx, pin_tx, alt_func_tx);
    port_system_gpio_config_alternate(p_port_rx, pin_rx, alt_func_rx);

    // Enable clock for USART peripheral.
    RCC -> APB1ENR |= RCC_APB1ENR_USART3EN;

    // Disable the USART.
    p_usart -> CR1 &= ~USART_CR1_UE;

    // Configure USART: 9600 baud rate, 8 data bits, no parity, 1 stop bit, oversampling by 16.
    p_usart -> BRR = 0b0000011010000011; 
    p_usart -> CR1 &= ~USART_CR1_M;
    p_usart-> CR1 &= ~(USART_CR1_PCE | USART_CR1_PS);
    p_usart -> CR2 &= ~USART_CR2_STOP;
    p_usart-> CR1 &= ~(USART_CR1_TE | USART_CR1_RE);
    p_usart-> CR1 |=  (USART_CR1_TE | USART_CR1_RE);
    p_usart -> SR &= ~USART_SR_RXNE;
    port_usart_disable_rx_interrupt(usart_id);
    port_usart_disable_tx_interrupt(usart_id);

    if (p_usart == USART3)
    {
        NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
        NVIC_EnableIRQ(USART3_IRQn);
        
    };
    
    p_usart -> CR1 |= USART_CR1_UE;

    // Reset input and output buffers.
    _reset_buffer(usart_arr[usart_id].input_buffer, USART_INPUT_BUFFER_LENGTH);
    _reset_buffer(usart_arr[usart_id].output_buffer, USART_OUTPUT_BUFFER_LENGTH);

};