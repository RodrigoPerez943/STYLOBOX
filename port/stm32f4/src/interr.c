/**
 * @file interr.c
 * @brief Interrupt service routines for the STM32F4 platform.
 * @author David Hernández Sola
 * @author Rodrigo Pérez del Pulgar Almodovar
 * @date 2024-01-01
 */
// Include HW dependencies:
#include "port_system.h"
#include "port_button.h"
#include "port_usart.h"
#include "port_buzzer.h"

// Include headers of different port elements:

//------------------------------------------------------
// INTERRUPT SERVICE ROUTINES Rod
//------------------------------------------------------
/**
 * @brief Interrupt service routine for the System tick timer (SysTick).
 *
 * @note This ISR is called when the SysTick timer generates an interrupt.
 * The program flow jumps to this ISR and increments the tick counter by one millisecond.
 *
 * > **TO-DO alumnos:**
 * >
 * > ✅ 1. **Increment the System tick counter `msTicks` in 1 count.** To do so, use the function `port_system_get_millis()` and `port_system_get_millis()`.
 *
 * @warning **The variable `msTicks` must be declared volatile!** Just because it is modified by a call of an ISR, in order to avoid [*race conditions*](https://en.wikipedia.org/wiki/Race_condition). **Added to the definition** after *static*.
 *
 */
void SysTick_Handler(void)
{   
    uint32_t millis = port_system_get_millis();
    port_system_set_millis(millis + 1);
};

/// @brief This function handles Px10-Px15 global interrupts.
/// First, this function identifies the line/ pin which has raised the interruption. Then, perform the desired action. Before leaving it cleans the interrupt pending register.
void EXTI15_10_IRQHandler ( void )
{
    port_system_systick_resume();
    if (EXTI->PR & BIT_POS_TO_MASK(buttons_arr[BUTTON_0_ID].pin )){
        if ( port_system_gpio_read(buttons_arr[BUTTON_0_ID].p_port , buttons_arr[BUTTON_0_ID].pin) ==LOW  ){
            buttons_arr[BUTTON_0_ID].flag_pressed = true;
        }
        else {
            buttons_arr[BUTTON_0_ID].flag_pressed = false;
        }
        EXTI->PR |= BIT_POS_TO_MASK(buttons_arr[BUTTON_0_ID].pin);
}

};


/// @brief This function handles USART3 global interrupt.
/// First, this function identifies the line/ pin which has raised the interruption. Then, perform the desired action. Before leaving it cleans the interrupt pending register.
void USART3_IRQHandler(void) {
    port_system_systick_resume();
    // Check if RXNE flag is set and RXNEIE bit is enabled
    if ((usart_arr[USART_0_ID].p_usart->SR & USART_SR_RXNE) && (usart_arr[USART_0_ID].p_usart->CR1 & USART_CR1_RXNEIE)) {
        // Received new data, handle it
        port_usart_store_data(USART_0_ID); 
    }

    // Check if TXE flag is set and TXEIE bit is enabled
    if ((usart_arr[USART_0_ID].p_usart->SR& USART_SR_TXE) && (usart_arr[USART_0_ID].p_usart->CR1 & USART_CR1_TXEIE)) {
        // Ready to transmit new data, handle it
        port_usart_write_data(USART_0_ID); 
    }
};

/// @brief This function handles TIM2 global interrupt.
void TIM2_IRQHandler(void){
    TIM2->SR = ~TIM_SR_UIF;
    buzzers_arr[BUZZER_0_ID].note_end = true;
};  

