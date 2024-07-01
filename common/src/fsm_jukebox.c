/**
 * @file fsm_jukebox.c
 * @brief Jukebox FSM main file.
 * @author David Hernández Sola
 * @author Rodrigo Pérez del Pulgar Almodovar
 * @date fecha
 */

/* Includes ------------------------------------------------------------------*/
// Standard C includes

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fsm.h>
#include "fsm_jukebox.h"
#include "fsm_button.h"
#include "fsm_usart.h"
#include "fsm_buzzer.h"
#include "port_system.h"
#include "port_usart.h"

// Other includes
#include "fsm.h"

/* Defines ------------------------------------------------------------------*/
#define MAX(a, b) ((a) > (b) ? (a) : (b)) /*!< Macro to get the maximum of two values. */

/* Private functions */
/**
 * @brief Parse the message received by the USART.
 * 
 * Given data received by the USART, this function parses the message and extracts the command and the parameter (if available).
 * 
 * > 1. Split the message by space using function `strtok()` \n
 * > 2. If there's a token (command), copy it to the `p_command` variable. Otherwise, return `false` \n
 * > 3. Extract the parameter (if available). To do so, get the next token using function `strtok()`. If there's a token, copy it to the `p_param` variable. Otherwise, copy an empty string to the `p_param` variable \n
 * > 4. Return `true` indicating that the message has been parsed correctly \n
 * 
 * @param p_message Pointer to the message received by the USART.
 * @param p_command Pointer to store the command extracted from the message. 
 * @param p_param Pointer to store the parameter extracted from the message. 
 * @return true if the message has been parsed correctly 
 * @return false if the message has not been parsed correctly 
 */
bool _parse_message(char *p_message, char *p_command, char *p_param)
{
    char *p_token = strtok(p_message, " "); // Split the message by space

    // If there's a token (command), copy it to the command variable
    if (p_token != NULL)
    {
        strcpy(p_command, p_token);
    }
    else
    {
        // No command found, you might return an error or handle it as needed
        // The USART driver of the computer sends an empty at initialization, so we will ignore it
        return false;
    }

    // Extract the parameter (if available)
    p_token = strtok(NULL, " "); // Get the next token

    if (p_token != NULL)
    {
        strcpy(p_param, p_token);
    }
    else
    {
        strcpy(p_param, " "); // NO param found
    }
    return true;
}

/// @brief Set the next song to be played.
/// @param p_fsm_jukebox Pointer to the Jukebox FSM.
void _set_next_song	(fsm_jukebox_t *p_fsm_jukebox){

fsm_buzzer_set_action(p_fsm_jukebox->p_fsm_buzzer ,STOP);

    p_fsm_jukebox-> melody_idx = (p_fsm_jukebox->melody_idx + 1) % MELODIES_MEMORY_SIZE;

    if (p_fsm_jukebox -> melodies[p_fsm_jukebox -> melody_idx].melody_length <= 0) {
        p_fsm_jukebox->melody_idx = 0;
    };
    
    p_fsm_jukebox->p_melody = p_fsm_jukebox->melodies[p_fsm_jukebox->melody_idx].p_name;

    printf("Playing: %s\n", p_fsm_jukebox->p_melody);
    
    fsm_buzzer_set_melody(p_fsm_jukebox->p_fsm_buzzer, &p_fsm_jukebox->melodies[p_fsm_jukebox->melody_idx]); // ?

    fsm_buzzer_set_action(p_fsm_jukebox->p_fsm_buzzer, PLAY);
};

/// @brief Execute the command received by the USART.
/// @param p_fsm_jukebox Pointer to the Jukebox FSM.
/// @param p_command Pointer to the command to be executed.
/// @param p_param Pointer to the parameter of the command to be executed.
void _execute_command(fsm_jukebox_t *p_fsm_jukebox, char *p_command, char *p_param){

    // Play command
    if (strcmp(p_command, "play") == 0) {
        fsm_buzzer_set_action(p_fsm_jukebox->p_fsm_buzzer, PLAY);
    }
    // Stop command
    else if (strcmp(p_command, "stop") == 0) {
        fsm_buzzer_set_action(p_fsm_jukebox->p_fsm_buzzer, STOP);
    }
    // Pause command
    else if (strcmp(p_command, "pause") == 0) {
        fsm_buzzer_set_action(p_fsm_jukebox->p_fsm_buzzer, PAUSE);
    }
    // Speed command
    else if (strcmp(p_command, "speed") == 0) {
        double param = atof(p_param);
        fsm_buzzer_set_speed(p_fsm_jukebox->p_fsm_buzzer,MAX( param , 0.1));
    }
    // Next song command
    else if (strcmp(p_command, "next") == 0) {
        _set_next_song(p_fsm_jukebox);
    }
    // Select command
    else if (strcmp(p_command, "select")== 0) {
        uint32_t melody_selected = atoi(p_param);
        if (p_fsm_jukebox->melodies[melody_selected].melody_length != 0){
            
            fsm_buzzer_set_action(p_fsm_jukebox->p_fsm_buzzer,STOP);
            p_fsm_jukebox->melody_idx = melody_selected;
            fsm_buzzer_set_melody(p_fsm_jukebox->p_fsm_buzzer, &p_fsm_jukebox->melodies[melody_selected]);
            p_fsm_jukebox->p_melody = p_fsm_jukebox->melodies[melody_selected].p_name;
            fsm_buzzer_set_action(p_fsm_jukebox->p_fsm_buzzer, PLAY);        
            }
        else {
            char msg[USART_OUTPUT_BUFFER_LENGTH];
            sprintf(msg, "Error: Melody not found\n");
            fsm_usart_set_out_data(p_fsm_jukebox->p_fsm_usart,msg);
        }
    }
    // Info command
    else if (strcmp(p_command, "info") == 0) {
        char msg[USART_OUTPUT_BUFFER_LENGTH];
        sprintf(msg, "Playing: %s\n", p_fsm_jukebox->p_melody);
        fsm_usart_set_out_data(p_fsm_jukebox->p_fsm_usart,msg);
    }
    // If command not recognized
    else {
        char msg[USART_OUTPUT_BUFFER_LENGTH];
        sprintf(msg, "Error: Command not found\n");
        fsm_usart_set_out_data(p_fsm_jukebox->p_fsm_usart,msg);
    }

};

/* State machine input or transition functions */

/// @brief Check if the button has been pressed for the required time to turn ON the Jukebox.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
/// @return true;false
static bool check_on(fsm_t *p_this){

    fsm_jukebox_t *p_fsm = (fsm_jukebox_t *)(p_this);

    return (fsm_button_get_duration(p_fsm->p_fsm_button) > 0 && fsm_button_get_duration(p_fsm->p_fsm_button) > (p_fsm -> on_off_press_time_ms));
};

/// @brief Check if the button has been pressed for the required time to turn OFF the Jukebox.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
/// @return true;false
static bool check_off (fsm_t *p_this){
    return check_on(p_this);
};	

/// @brief Check if the buzzer has finished playing the melody.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
/// @return true;false
static bool check_melody_finished(fsm_t * p_this){
    fsm_jukebox_t *p_fsm = (fsm_jukebox_t *)(p_this);
    return (STOP == fsm_buzzer_get_action(p_fsm->p_fsm_buzzer));
};

/// @brief Check if the USART has received data.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
/// @return true;false
static bool check_command_received	(fsm_t *p_this){
    fsm_jukebox_t *p_fsm = (fsm_jukebox_t *)(p_this);
    return fsm_usart_check_data_received(p_fsm->p_fsm_usart);
};

/// @brief Check if the button has been pressed for the required time to load the next song.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
/// @return true;false
static bool check_next_song_button	(fsm_t *p_this){

    fsm_jukebox_t *p_fsm = (fsm_jukebox_t *)(p_this);
    uint32_t valor = fsm_button_get_duration(p_fsm->p_fsm_button);
    return (valor > 0 && valor > p_fsm -> next_song_press_time_ms && valor < p_fsm -> on_off_press_time_ms );
};

/// @brief Check if any of the elements of the system is active.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
/// @return true;false
static bool check_activity (fsm_t *p_this){
    fsm_jukebox_t *p_fsm = (fsm_jukebox_t *)(p_this);
    return ( fsm_button_check_activity(p_fsm->p_fsm_button) || fsm_buzzer_check_activity(p_fsm->p_fsm_buzzer) || fsm_usart_check_activity(p_fsm->p_fsm_usart));
};

/// @brief Check if all the is system active.
/// @param p_this Pointer to an fsm_t struct than contains an fsm_jukebox_t.
/// @return true;false
static bool check_no_activity	(fsm_t *p_this){
    return (!check_activity(p_this));
};

/* State machine output or action functions */

/// @brief Initialize the Jukebox by playing the intro melody, at the beginning of the program.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
static void do_start_up	(fsm_t *p_this)	{
    fsm_jukebox_t *p_fsm = (fsm_jukebox_t *)p_this;

    fsm_button_reset_duration(p_fsm->p_fsm_button);

    // 2. Enable RX USART interrupts
    fsm_usart_enable_rx_interrupt(p_fsm->p_fsm_usart);

    // 3. Debugging output to ITM terminal
    printf("Jukebox ON\n");

    // 4. Set the speed of the buzzer to 1.0
    fsm_buzzer_set_speed(p_fsm->p_fsm_buzzer, 1.0);

    melody_t *melodia = p_fsm -> melodies;

    // 5. Set the melody to be played
    fsm_buzzer_set_melody(p_fsm->p_fsm_buzzer, melodia);

    // 6. Start playing the melody
    fsm_buzzer_set_action(p_fsm->p_fsm_buzzer, PLAY);
};

/// @brief After playing the intro melody, start the Jukebox.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
static void do_start_jukebox(fsm_t *p_this	){
    fsm_jukebox_t *p_fsm = (fsm_jukebox_t *)p_this;
    p_fsm -> melody_idx = 0;
    p_fsm -> p_melody = p_fsm->melodies[p_fsm -> melody_idx].p_name;
};

/// @brief Turn the Jukebox OFF.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
static void do_stop_jukebox	(fsm_t *p_this){

    fsm_jukebox_t *p_fsm = (fsm_jukebox_t *)p_this;
    fsm_button_reset_duration(p_fsm->p_fsm_button);
    fsm_usart_disable_rx_interrupt(p_fsm->p_fsm_usart);
    fsm_usart_disable_tx_interrupt(p_fsm->p_fsm_usart);
    printf("Jukebox OFF\n");
    fsm_buzzer_set_action(p_fsm->p_fsm_buzzer,STOP);
};

/// @brief Load the next song.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
static void do_load_next_song (fsm_t *p_this){
    fsm_jukebox_t *p_fsm = (fsm_jukebox_t *)p_this;
    _set_next_song(p_fsm);
    fsm_button_reset_duration(p_fsm->p_fsm_button);
};

/// @brief Read the command received by the USART.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
static void do_read_command	(fsm_t *p_this){
    fsm_jukebox_t *p_fsm = (fsm_jukebox_t *)p_this;
    char p_message[USART_INPUT_BUFFER_LENGTH];
    char p_command[USART_INPUT_BUFFER_LENGTH];
    char p_param[USART_INPUT_BUFFER_LENGTH];
    fsm_usart_get_in_data(p_fsm->p_fsm_usart,p_message);
    if (_parse_message(p_message,p_command,p_param)){
        _execute_command(p_fsm,p_command,p_param);
    };
    fsm_usart_reset_input_data(p_fsm->p_fsm_usart);
    memset(p_message,0,USART_INPUT_BUFFER_LENGTH);
};

/// @brief Start the low power mode while the Jukebox is OFF.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
static void do_sleep_off(fsm_t *p_this){
    port_system_sleep();
};


/// @brief Start the low power mode while the Jukebox is waiting for a command.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
static void do_sleep_wait_command(fsm_t *p_this){
    port_system_sleep();
};

/// @brief Start the low power mode while the Jukebox is OFF.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
static void do_sleep_while_off(fsm_t *p_this){
    port_system_sleep();
};

/// @brief Start the low power mode while the Jukebox is ON.
/// @param p_this Pointer to an fsm_t struct that contains an fsm_jukebox_t.
static void do_sleep_while_on(fsm_t *p_this){
    port_system_sleep();
};

/* Public functions */

/// @brief Create a new jukebox FSM.
/// @param p_fsm_button Pointer to the button FSM
/// @param on_off_press_time_ms Button press time in milliseconds to turn the system ON or OFF
/// @param p_fsm_usart Pointer to the USART FSM
/// @param p_fsm_buzzer Pointer to the buzzer FSM.
/// @param next_song_press_time_ms Button press time in milliseconds to change to the next song.
/// @return A pointer to the button FSM
fsm_t *fsm_jukebox_new(fsm_t *p_fsm_button, uint32_t on_off_press_time_ms, fsm_t *p_fsm_usart, fsm_t *p_fsm_buzzer, uint32_t next_song_press_time_ms)
{
    fsm_t *p_fsm = malloc(sizeof(fsm_jukebox_t));

    fsm_jukebox_init(p_fsm, p_fsm_button, on_off_press_time_ms, p_fsm_usart, p_fsm_buzzer, next_song_press_time_ms);
    
    return p_fsm;
}

/// @brief Array representing the transitions table of the FSM Jukebox.
static fsm_trans_t fms_trans_jukebox[] = {
    { OFF , check_on , START_UP ,do_start_up },
    { START_UP , check_melody_finished , WAIT_COMMAND ,do_start_jukebox },
    { WAIT_COMMAND , check_off , OFF , do_stop_jukebox },
    { WAIT_COMMAND , check_next_song_button , WAIT_COMMAND , do_load_next_song },
    { WAIT_COMMAND , check_command_received , WAIT_COMMAND , do_read_command },
    { WAIT_COMMAND , check_no_activity , SLEEP_WHILE_ON , do_sleep_wait_command },
    { SLEEP_WHILE_ON , check_no_activity , SLEEP_WHILE_ON , do_sleep_while_on },
    { SLEEP_WHILE_ON , check_activity , WAIT_COMMAND , NULL },
    { OFF , check_no_activity , SLEEP_WHILE_OFF , do_sleep_off },
    { SLEEP_WHILE_OFF , check_no_activity , SLEEP_WHILE_OFF , do_sleep_while_off },
    { SLEEP_WHILE_OFF , check_activity , OFF , NULL },
    { -1 , NULL , -1, NULL }
};

/// @brief Initialize a jukebox FSM.
/// @param p_this Pointer to the jukebox FSM structure.
/// @param p_fsm_button Pointer to the button FSM.
/// @param on_off_press_time_ms Time in milliseconds to press the on/off button.
/// @param p_fsm_usart Pointer to the USART FSM.
/// @param p_fsm_buzzer Pointer to the buzzer FSM.
/// @param next_song_press_time_ms Time in milliseconds to press the next song button.
void fsm_jukebox_init(fsm_t *p_this, fsm_t *p_fsm_button, uint32_t on_off_press_time_ms, fsm_t *p_fsm_usart, fsm_t *p_fsm_buzzer, uint32_t next_song_press_time_ms ){
    
    // 1. Initialize the FSM with the given transition table
    fsm_init(p_this, fms_trans_jukebox); 
    fsm_jukebox_t *p_fsm = (fsm_jukebox_t *)(p_this);

    // 2. Initialize the other fields with received values
    p_fsm ->p_fsm_button = p_fsm_button;
    p_fsm ->on_off_press_time_ms = on_off_press_time_ms;
    p_fsm ->p_fsm_usart = p_fsm_usart;
    p_fsm ->p_fsm_buzzer = p_fsm_buzzer;
    p_fsm ->next_song_press_time_ms = next_song_press_time_ms;

    // 3. Initialize the melody index to 0
    p_fsm ->  melody_idx = 0;

    // 4. Reset the array of melodies
    memset(p_fsm ->melodies, EMPTY_BUFFER_CONSTANT, sizeof(p_fsm->melodies));

    p_fsm ->melodies[0] = scale_melody; 
    p_fsm ->melodies[1] = happy_birthday_melody;
    p_fsm ->melodies[2] = tetris_melody;
};


