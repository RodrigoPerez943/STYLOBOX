/**
 * @file main.c
 * @brief Main program body.
 * 
 * This file includes the main set of functions to initialize and run a control system
 * using various components like buttons, LEDs, USART, and buzzer. It includes initialization
 * of GPIOs, a main loop with a finite state machine (FSM) handling, and handlers for button
 * debouncing and state changes based on user inputs.
 */

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "port_buzzer.h" 
#include "port_system.h"
#include <stdio.h>
#include "fsm_button.h"
#include "port_button.h"
#include "fsm_usart.h"
#include "port_usart.h"
#include "fsm_buzzer.h"
#include "melodies.h"
#include <string.h>
#include "fsm_jukebox.h"
#include "port_led.h"

/**
 * @brief Initialize GPIOs for input buttons and output indicators.
 */
void GPIO_Init(void);

/**
 * @brief Plays a melody when the system state changes.
 */
void PlayStateChangeMelody(void);

#define DEBOUNCE_DELAY 50 ///< Debounce delay for buttons in milliseconds
#define ON_OFF_PRESS_TIME_MS 1001 ///< Duration to hold button to toggle system ON/OFF
#define NEXT_SONG_BUTTON_TIME_MS 501 ///< Duration to press button to change song
#define MELODY_LENGTH 2 ///< Number of notes in the state change melody


// (frecuencias en Hz)
double noteFrequencies[12] = {
    493.88,
    466.16,
    440.00,
    415.30,
    392.00,
    369.99,
    349.23,
    329.63,
    311.13,
    293.66,
    277.18,
    261.63
};

/**
 * @struct Note
 * @brief Structure to represent a musical note.
 *
 * This structure holds information about the frequency and duration of a musical note.
 */
typedef struct {
    double frequency;
    uint32_t duration;
} Note;

Note stateChangeMelody[] = {
    {523.25, 200}, // C5
    {200, 200}, // E5
};

uint32_t tiempoPulsado = 0; ///< Time stamp when the button was pressed

/**
 * @brief Main program.
 * @return int Program exit status.
 */
int main(void) {

    int primeraIteracion = 0;
    int noteIndex = -1;
    int noteindexBefore = -1;
    bool change = 0;
    GPIO_PinState pinStates[12];
    GPIO_PinState debouncedStates[12] = {GPIO_PIN_RESET};

    uint32_t debounceTimers[12] = {0};
    uint32_t buttonDebounceTimer1 = 0;
    uint32_t buttonDebounceTimer2 = 0;
    uint32_t buttonDebounceTimer3 = 0;

    GPIO_PinState lastButtonState1 = GPIO_PIN_RESET;
    GPIO_PinState lastButtonState2 = GPIO_PIN_RESET;
    GPIO_PinState lastButtonState3 = GPIO_PIN_RESET;
    GPIO_PinState buttonState1 = GPIO_PIN_RESET;  // Botón +
    GPIO_PinState buttonState2 = GPIO_PIN_RESET;  // Botón -
    GPIO_PinState buttonState3 = GPIO_PIN_RESET;

    int salir = 0;
    bool estadoAnterior = 0;    
    port_led_gpio_setup();

    while(1){

    if (estadoAnterior == 0) {
        port_led_toggle_on();
        port_system_init();

        fsm_t* p_fsm_button = fsm_button_new(BUTTON_0_DEBOUNCE_TIME_MS, BUTTON_0_ID);
        fsm_t* p_fsm_usart = fsm_usart_new(USART_0_ID);
        fsm_t* p_fsm_buzzer = fsm_buzzer_new(BUZZER_0_ID);
        fsm_t* p_fsm_jukebox = fsm_jukebox_new(p_fsm_button, ON_OFF_PRESS_TIME_MS, p_fsm_usart, p_fsm_buzzer, NEXT_SONG_BUTTON_TIME_MS);

        while(1){
            salir = 1;
            buttonState3 = port_button_is_pressed(BUTTON_0_ID);  // Botón Modo

            if (buttonState3 != lastButtonState3) {
                    if ((port_button_get_tick() - buttonDebounceTimer3) > DEBOUNCE_DELAY) {
                        if (buttonState3 != lastButtonState3) {
                            lastButtonState3 = buttonState3;
                        if (buttonState3 == 1) {
                            tiempoPulsado = port_button_get_tick();
                        }
                        }
                    }
                    buttonDebounceTimer3 = port_button_get_tick();
                }
            
            if (lastButtonState3 == GPIO_PIN_SET &&  (((port_button_get_tick() - tiempoPulsado) > 7000))) { 
                estadoAnterior = 1;
                PlayStateChangeMelody();
                break;
            }
            
            fsm_fire(p_fsm_button);
            fsm_fire(p_fsm_usart);
            fsm_fire(p_fsm_buzzer);
            fsm_fire(p_fsm_jukebox);


        }
        
        if (salir == 1){
            fsm_destroy(p_fsm_button);
            fsm_destroy(p_fsm_usart);
            fsm_destroy(p_fsm_buzzer);
            fsm_destroy(p_fsm_jukebox);
        }


    } else if (estadoAnterior == 1) {
        port_led_toggle_off();
        GPIO_Init();
        HAL_Init();
        port_buzzer_init(BUZZER_0_ID);

        tiempoPulsado = 0;

        while (1) {

            port_system_get_millis();

            pinStates[0] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8);
            pinStates[1] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);
            pinStates[2] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5); 

            pinStates[3] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12);  
            pinStates[4] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11);  
            pinStates[5] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);  

            pinStates[6] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);  
            pinStates[7] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);  
            pinStates[8] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);  
            pinStates[9] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);  
            pinStates[10] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13); 

            pinStates[11] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4); 

            buttonState1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9);  // Botón +
            buttonState2 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);  // Botón -
            buttonState3 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);  // Botón Modo
            
            if (buttonState1 != lastButtonState1) {
                if ((port_system_get_millis() - buttonDebounceTimer1) > DEBOUNCE_DELAY) {
                    if (buttonState1 != lastButtonState1) {
                        lastButtonState1 = buttonState1;
                        if (buttonState1 == GPIO_PIN_SET) {
                            for (int i = 0; i < 12; i++) {
                                noteFrequencies[i] *= 2;
                            }
                        }
                    }
                }
                buttonDebounceTimer1 = port_system_get_millis();
            }

            if (buttonState2 != lastButtonState2) {
                if ((port_system_get_millis() - buttonDebounceTimer2) > DEBOUNCE_DELAY) {
                    if (buttonState2 != lastButtonState2) {
                        lastButtonState2 = buttonState2;
                        if (buttonState2 == GPIO_PIN_SET) {
                            for (int i = 0; i < 12; i++) {
                                noteFrequencies[i] /= 2;
                            }
                        }
                    }
                }
                buttonDebounceTimer2 = port_system_get_millis();
            }

            buttonState3 = port_button_is_pressed(BUTTON_0_ID);  // Botón Modo
            if (buttonState3 != lastButtonState3) {
                    if ((port_system_get_millis() - buttonDebounceTimer3) > DEBOUNCE_DELAY) {
                        if (buttonState3 != lastButtonState3) {
                            lastButtonState3 = buttonState3;
                            if (buttonState3 == GPIO_PIN_SET) {
                                tiempoPulsado = port_system_get_millis();
                            }
                        }
                    }
                    buttonDebounceTimer3 = port_system_get_millis();
            }

            if ( lastButtonState3 == GPIO_PIN_SET && ((port_button_get_tick() - tiempoPulsado) > 7000)) {
                PlayStateChangeMelody();
                NVIC_SystemReset();
            }

                for (int i = 0; i < 12; i++) {
                    if (pinStates[i] != debouncedStates[i]) {
                        if ((port_system_get_millis() - debounceTimers[i]) > DEBOUNCE_DELAY) {
                            debouncedStates[i] = pinStates[i];
                            change = 1;
                            debounceTimers[i] = port_system_get_millis();
                            continue;
                        }
                        debounceTimers[i] = port_system_get_millis();
                    }
                }

                for (int i = 0; i < 12; i++) {
                    if (debouncedStates[i] == GPIO_PIN_SET) {
                        if (primeraIteracion == 0) {
                            noteIndex = i;
                            noteindexBefore = noteIndex;
                            primeraIteracion += 1;
                            break;
                        }
                        noteindexBefore = noteIndex;
                        noteIndex = i;
                        break;
                    }
                }

                if (noteIndex != -1 && noteindexBefore == noteIndex && debouncedStates[noteIndex] == GPIO_PIN_SET && change == 0) {
                    continue;
                } else if (noteIndex != -1 && debouncedStates[noteIndex] == GPIO_PIN_SET) {
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); 
                    port_buzzer_set_note_duration(BUZZER_0_ID, 1000);
                    port_buzzer_set_note_frequency(BUZZER_0_ID, noteFrequencies[noteIndex]);
                    change = 0;
                    port_system_delay_ms(90);
                } else if (noteIndex != -1) {
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
                    port_buzzer_stop(BUZZER_0_ID);
                }
        }
    }
    }
}

/**
 * @brief Plays a predefined melody to indicate a state change.
 */
void PlayStateChangeMelody(void) {
    for (int i = 0; i < MELODY_LENGTH; i++) {
        port_buzzer_set_note_duration(BUZZER_0_ID, stateChangeMelody[i].duration);
        port_buzzer_set_note_frequency(BUZZER_0_ID, stateChangeMelody[i].frequency);
        port_system_delay_ms(stateChangeMelody[i].duration);
    }
    port_buzzer_stop(BUZZER_0_ID);
}

/**
 * @brief Initializes GPIO pins used in the application.
 */
void GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;

    // Configuración de pines
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_14;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}