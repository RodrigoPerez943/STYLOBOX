/**
 * @file fsm_usart.c
 * @brief USART FSM main file.
 * @author Rodrigo Pérez del Pulgar Almodóvar
 * @author David Hernández Sola
 * @date 22/03/2023
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C libraries */
#include <string.h>
#include <stdlib.h>
#include "port_usart.h"
#include "fsm_usart.h"
/* Other libraries */

/* State machine input or transition functions */

/// @brief Check if data have been received.
// Call function port_usart_rx_done() and return the result
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
/// @return true or false, boolean
static bool check_data_rx (fsm_t *p_this) 
{
    fsm_usart_t *p_usart = (fsm_usart_t *)(p_this);
    return port_usart_rx_done(p_usart->usart_id);
};

/// @brief Check if there are data to be sent.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
/// @return Return true if the first element of the out_data array is different from EMPTY_BUFFER_CONSTANT, boolean
static bool check_data_tx(fsm_t *p_this)
{
    fsm_usart_t *p_usart = (fsm_usart_t *)(p_this);
    return (EMPTY_BUFFER_CONSTANT != p_usart->out_data[0]);
};

/// @brief Check if the data have been sent.
///  Call function port_usart_tx_done() and return the result
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
/// @return return value of the function port_usart_tx_done()
static bool check_tx_end(fsm_t *p_this)	
{
    fsm_usart_t *p_usart = (fsm_usart_t *)(p_this);
    return port_usart_tx_done(p_usart->usart_id);
};

/* State machine output or action functions */

/// @brief Get the data received.
/// This function gets the data received by the USART that still remains in the internal buffer of the PORT layer.
/// Call function port_usart_get_from_input_buffer() to store the data in the array in_data.
/// Call function port_usart_reset_input_buffer() to reset the USART RX buffer in the PORT layer.
/// Update the flag data_received to indicate that data have been received.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
static void do_get_data_rx(fsm_t *p_this)
{
    fsm_usart_t *p_usart = (fsm_usart_t *)(p_this);
    port_usart_get_from_input_buffer(p_usart->usart_id,p_usart->in_data);
    port_usart_reset_input_buffer(p_usart->usart_id);
    p_usart->data_received = true;

};

/// @brief Set the data to be sent.
/// This function sets the data to be sent by the USART to the internal buffer of the PORT layer.
/// Reset the output data in the PORT layer before setting a new one
/// Call function port_usart_copy_to_output_buffer() to set the data stored in the array out_data.
/// Wait until the TX register is empty
/// Call function port_usart_write_data() to write the first character to the USART TX register
/// Call function port_usart_enable_tx_interrupt() to enable the USART TX interrupt to start the transmission
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
static void do_set_data_tx(fsm_t *p_this) 
{
    fsm_usart_t *p_usart = (fsm_usart_t *)(p_this);
    port_usart_reset_output_buffer(p_usart->usart_id);
    port_usart_copy_to_output_buffer(p_usart->usart_id,p_usart->out_data,USART_OUTPUT_BUFFER_LENGTH);
    while (!port_usart_get_txr_status(p_usart->usart_id)){};
    port_usart_write_data(p_usart->usart_id);
    port_usart_enable_tx_interrupt(p_usart->usart_id);
};


/// @brief Finish the data transmission.
/// This function finishes the data transmission by resetting the output data in the PORT layer.
/// Reset the output data in the PORT layer
/// Reset the output data out_data with EMPTY_BUFFER_CONSTANT.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
static void do_tx_end(fsm_t *p_this)
{
    fsm_usart_t *p_usart = (fsm_usart_t *)(p_this);
    port_usart_reset_output_buffer(p_usart->usart_id);
    memset(p_usart->out_data,EMPTY_BUFFER_CONSTANT,USART_OUTPUT_BUFFER_LENGTH);
};

/* Public functions */

/// @brief Array representing the transitions table of the fsm_usart.
static fsm_trans_t fms_trans_usart[] = {
    { WAIT_DATA , check_data_tx , SEND_DATA ,do_set_data_tx },
    { WAIT_DATA , check_data_rx , WAIT_DATA ,do_get_data_rx },
    { SEND_DATA , check_tx_end , WAIT_DATA , do_tx_end },
    { -1 , NULL , -1, NULL }
};


/// @brief Check if data has been received.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
/// @return Return the value of the field data_received, boolean
bool fsm_usart_check_data_received(	fsm_t *p_this){
    fsm_usart_t *p_usart = (fsm_usart_t *)(p_this);
    return p_usart->data_received;
};

/// @brief Reset the input data buffer.
/// Initialize the field in_data of size USART_INPUT_BUFFER_LENGTH to EMPTY_BUFFER_CONSTANT
/// Reset the field data_received
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
void fsm_usart_reset_input_data	(fsm_t *p_this	)
{
    fsm_usart_t *p_usart = (fsm_usart_t *)(p_this);
    memset(p_usart->in_data,EMPTY_BUFFER_CONSTANT,USART_INPUT_BUFFER_LENGTH);
    p_usart->data_received = false;
};

/// @brief Disable the USART RX interrupt.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
void fsm_usart_disable_rx_interrupt(fsm_t *p_this){
    fsm_usart_t *p_usart = (fsm_usart_t *)(p_this);
    port_usart_disable_rx_interrupt(p_usart->usart_id);
};

/// @brief Disable the USART TX interrupt.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
void fsm_usart_disable_tx_interrupt(fsm_t *p_this){
    fsm_usart_t *p_usart = (fsm_usart_t *)(p_this);
    port_usart_disable_tx_interrupt(p_usart->usart_id);
};


/// @brief Enable the USART RX interrupt.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
void fsm_usart_enable_rx_interrupt(	fsm_t *p_this){
    fsm_usart_t *p_usart = (fsm_usart_t *)(p_this);
    port_usart_enable_rx_interrupt(p_usart->usart_id);
};

/// @brief Enable the USART TX interrupt.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
void fsm_usart_enable_tx_interrupt(	fsm_t *p_this){
    fsm_usart_t *p_usart = (fsm_usart_t *)(p_this);
    port_usart_enable_tx_interrupt(p_usart->usart_id);
};


/// @brief This function gets the data received by the USART.
/// Copies the content from source in_data of size USART_INPUT_BUFFER_LENGTH to the destination array p_data.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
/// @param p_data Pointer to the array where the data will be copied from the in_data array. 
void fsm_usart_get_in_data(fsm_t *p_this, char *p_data)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    memcpy(p_data, p_fsm->in_data, USART_INPUT_BUFFER_LENGTH);
};

/// @brief This function sets the data to send by the USART.
/// Resets the array out_data of size USART_OUTPUT_BUFFER_LENGTH to EMPTY_BUFFER_CONSTANT.
/// Copies the content from source array p_data to the destination out_data of size USART_OUTPUT_BUFFER_LENGTH
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
/// @param p_data Pointer to the array where the data will be copied from the in_data array. 
void fsm_usart_set_out_data(fsm_t *p_this, char *p_data)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    memset(p_fsm->out_data, EMPTY_BUFFER_CONSTANT, USART_OUTPUT_BUFFER_LENGTH);
    memcpy(p_fsm->out_data, p_data, USART_OUTPUT_BUFFER_LENGTH);
};


/// @brief Create a new USART FSM. 
/// This FSM implements a USART communication protocol. It is a state machine that sends and receives data.
/// The FSM stores the received data in the in_data array. The user should ask for it using the function fsm_usart_get_in_data().
/// At start and reset, the in_data array must be empty. An empty array means that there has not been new data.
/// @param usart_id Unique USART identifier number
/// @return A pointer to the USART FSM, fsm_t
fsm_t *fsm_usart_new(uint32_t usart_id)
{
    fsm_t *p_fsm = malloc(sizeof(fsm_usart_t));
    fsm_usart_init(p_fsm, usart_id);
    return p_fsm;
};

/// @brief This function initializes the default values of the FSM struct and calls to the port to initialize the HW of associated to the given ID.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_usart_t. 
/// @param usart_id Unique USART identifier number
void fsm_usart_init(fsm_t *p_this, uint32_t usart_id)
{
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    fsm_init(p_this, fms_trans_usart);
    p_fsm->usart_id = usart_id;
    p_fsm->data_received = false;
    memset(p_fsm->in_data,EMPTY_BUFFER_CONSTANT,USART_INPUT_BUFFER_LENGTH);
    memset(p_fsm->out_data,EMPTY_BUFFER_CONSTANT,USART_OUTPUT_BUFFER_LENGTH);
    port_usart_init(usart_id);

};

/// @brief Check if the USART FSM is active, or not.
/// @param p_this 	Pointer to an fsm_t struct than contains an fsm_usart_t struct
/// @return True if the current state is SEND_DATA or data_received is true. Otherwise, return false.
bool fsm_usart_check_activity(fsm_t *p_this){
    fsm_usart_t *p_fsm = (fsm_usart_t *)(p_this);
    bool uno = p_this ->current_state == SEND_DATA;
    bool dos = p_fsm->data_received;
    return((uno || dos));
};
