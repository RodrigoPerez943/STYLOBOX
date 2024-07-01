/**
 * @file fsm_button.c
 * @brief Button FSM main file.
 * @author Rodrigo Pérez del Pulgar Almodóvar
 * @author David Hernández Sola
 * @date 22/03/2023
 */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "fsm_button.h"
#include "port_button.h"


/* State machine input or transition functions */

/// @brief Check if the button has been pressed.
/// Call function port_button_is_pressed() and retrieve the status.
/// @param p_this 	Pointer to an fsm_t struct than contains an fsm_button_t
/// @return Return the status, boolean
static bool check_button_pressed(fsm_t *p_this){
    fsm_button_t *p_boton = (fsm_button_t *)(p_this);
    return port_button_is_pressed(p_boton->button_id);
};

/// @brief Call function port_button_get_tick() and retrieve the current system tick
/// Check if the current system tick is higher than the field next_timeout
/// @param p_this 	Pointer to an fsm_t struct than contains an fsm_button_t
/// @return Return true if it is higher, otherwise return false
static bool check_timeout(fsm_t *p_this	){
    uint32_t tiempo = port_button_get_tick();
    fsm_button_t *p_boton = (fsm_button_t *)(p_this);
    if (tiempo > (p_boton->next_timeout)){
        return true;
    }
    return false;
};

/// @brief Check if the button has been released.
/// Call function port_button_is_pressed() and retrieve the status
/// @param p_this p_this Pointer to an fsm_t struct than contains an fsm_button_t.
/// @return Return the inverse of the status, boolean
static bool check_button_released(fsm_t *p_this){
    fsm_button_t *p_boton = (fsm_button_t *)(p_this);
    return !port_button_is_pressed(p_boton->button_id);
};

/* State machine output or action functions */

/// @brief Store the system tick when the button was pressed.
/// Call function port_button_get_tick() and retrieve the current system tick.
/// Store it in the field tick_pressed
/// Update the field next_timeout considering the current tick and the field debounce_time
/// @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
static void do_store_tick_pressed(fsm_t *p_this){
    uint32_t tick = port_button_get_tick();
    fsm_button_t *p_boton = (fsm_button_t *)(p_this);
    p_boton->tick_pressed = tick;
    p_boton -> next_timeout = tick + p_boton-> debounce_time;
};

/// @brief Store the duration of the button press.
///Call function port_button_get_tick() and retrieve the current system tick
/// Update the field duration considering it and the field tick_pressed
/// Update the field next_timeout considering the current tick and the field debounce_time
/// @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
static void do_set_duration	(fsm_t *p_this){
    fsm_button_t *p_boton = (fsm_button_t *)(p_this);
    uint32_t tick = port_button_get_tick();
    uint32_t tickp = p_boton->tick_pressed;
    p_boton->duration = tick - tickp;
    p_boton -> next_timeout = tick + p_boton->debounce_time;
};

/* Other auxiliary functions */

/// @brief Array representing the transitions table of the FSM button.
static fsm_trans_t fsm_trans_button [] = {
    { BUTTON_RELEASED , check_button_pressed , BUTTON_PRESSED_WAIT ,do_store_tick_pressed },
    { BUTTON_PRESSED_WAIT , check_timeout , BUTTON_PRESSED , NULL },
    { BUTTON_PRESSED , check_button_released , BUTTON_RELEASED_WAIT , do_set_duration } ,
    { BUTTON_RELEASED_WAIT , check_timeout , BUTTON_RELEASED , NULL },
    { -1 , NULL , -1, NULL }
};


/// @brief Create a new button FSM.
/// @param debounce_time Anti-debounce time in milliseconds
/// @param button_id Unique button identifier number
/// @return A pointer to the button FSM
fsm_t *fsm_button_new(uint32_t debounce_time, uint32_t button_id)
{
    fsm_t *p_fsm = malloc(sizeof(fsm_button_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_button_init(p_fsm, debounce_time, button_id);
    return p_fsm;
};

/// @brief Return the duration of the last button press.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
/// @return return the field duration of the last button press, uint32_t
uint32_t fsm_button_get_duration(fsm_t *p_this) {
    fsm_button_t *p_boton = (fsm_button_t *)(p_this);
    return p_boton->duration;
};

/// @brief Reset the duration of the last button press.
///  Set to 0 the field duration
/// @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
void fsm_button_reset_duration(fsm_t *p_this) {
    fsm_button_t *p_boton = (fsm_button_t *)(p_this);
    p_boton->duration = 0;
};


/// @brief Initialize a button FSM.
/// This function initializes the default values of the FSM struct and calls to the port to initialize the HW of associated to the given ID.
/// Initialize the fields debounce_time, button_id of p_fsm with the received values
/// Initialize the fields tick_pressed, duration of p_fsm with 0
/// Call function port_button_init()
/// @param p_this Pointer to an fsm_t struct than contains an fsm_button_t.
/// @param debounce_time Anti-debounce time in milliseconds
/// @param button_id Unique button identifier number
void fsm_button_init(fsm_t *p_this, uint32_t debounce_time, uint32_t button_id)
{
    fsm_button_t *p_boton = (fsm_button_t *)(p_this);
    fsm_init(p_this , fsm_trans_button);
    p_boton -> debounce_time = debounce_time;
    p_boton -> button_id = button_id;
    p_boton -> tick_pressed = 0;
    p_boton -> duration = 0;
    port_button_init(button_id);
};

/// @brief Check if the button is pressed.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_button_t struct
/// @return True if the button is pressed. False if the button is not pressed.
bool fsm_button_check_activity(fsm_t *p_this){
    return((p_this->current_state != BUTTON_RELEASED));
};




