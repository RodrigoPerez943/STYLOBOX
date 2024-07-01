/**
 * @file port_buzzer.c
 * @brief Portable functions to interact with the Buzzer melody player FSM library.
 * @author David Hernández Sola
 * @author Rodrigo Pérez del Pulgar Almodovar
 * @date fecha
 */
/* Includes ------------------------------------------------------------------*/
/* Standard C libraries */

#include <math.h>
#include "port_buzzer.h"

/* HW dependent libraries */

/* Global variables */
#define ALT_FUNC2_TIM3 2

/// @brief Array of elements that represents the HW characteristics of the buzzers.
port_buzzer_hw_t buzzers_arr [] = {
  [BUZZER_0_ID] = {
    .p_port = BUZZER_0_GPIO,
    .pin = BUZZER_0_PIN,
    .note_end = false,
    .alt_func = ALT_FUNC2_TIM3
  }
};

/* Private functions */

/// @brief Configure the timer that controls the duration of the note.
/// @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
static void _timer_duration_setup(uint32_t buzzer_id)
{
  if (buzzer_id == BUZZER_0_ID)
  {

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->CR1 &= ~TIM_CR1_CEN; // Deshabilitar el contador
    TIM2->CR1 |= TIM_CR1_ARPE; // Habilitar la precarga de autorecarga
    TIM2->SR &= ~TIM_SR_UIF; // Limpiar la bandera escribiendo 0 ???
    TIM2->DIER |= TIM_DIER_UIE; // Habilitar la interrupción de actualización

    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 3, 0)); 
    NVIC_EnableIRQ(TIM2_IRQn);                                                          
  }
};
 
/// @brief Configure the timer that controls the PWM of the buzzer.
/// @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
static void _timer_pwm_setup (uint32_t buzzer_id){

 if (buzzer_id == BUZZER_0_ID)
  {
    RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN ;     
    TIM3 -> CR1 &= ~TIM_CR1_CEN ;
    TIM3->CR1 |= TIM_CR1_ARPE; // Habilitar la precarga de autorecarga
    TIM3 -> CNT &= 0; // Contador a 0
    TIM3 -> ARR &= 0;
    TIM3 -> PSC &= 0;
    TIM3 -> EGR |= TIM_EGR_UG ;
    TIM3 -> CCER &= ~TIM_CCER_CC1E ; 
    TIM3 -> CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);
    TIM3 -> CCMR1 |= TIM_CCMR1_OC1PE ; /* Habilita el preload */
  }
};

/* Public functions -----------------------------------------------------------*/

/// @brief Set the duration of the timer that controls the duration of the note.
/// @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
/// @param duration_ms Duration of the note in ms
void port_buzzer_set_note_duration(uint32_t buzzer_id, uint32_t duration_ms) {

    if (buzzer_id == BUZZER_0_ID){
      
      TIM2 -> CR1 &= ~TIM_CR1_CEN ;
      TIM2 -> CNT &= 0;

      double clk = (double)SystemCoreClock;
      double ms = (double)duration_ms;

      double psc = (round)((clk*(ms/1000.0)/(65535.0 + 1.0)) - 1.0) ;
      double arr = (round)((clk*(ms/1000.0)/(psc + 1.0)) - 1.0) ;

      if (arr > 65535.0) {
          psc = psc + 1.0;
          arr = (round)((clk*(ms/1000.0) / (psc + 1.0)) - 1.0);
      }
      TIM2 -> ARR = arr;
      TIM2 -> PSC = psc;
      TIM2 -> EGR = TIM_EGR_UG ;
      buzzers_arr[buzzer_id].note_end = false;
      TIM2 -> CR1 |= TIM_CR1_CEN ;
    
}
};

/// @brief Set the PWM frequency of the timer that controls the frequency of the note.
/// @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
/// @param frequency_hz Frequency of the note in Hz
void port_buzzer_set_note_frequency(uint32_t buzzer_id, double frequency_hz) {
    if (buzzer_id == BUZZER_0_ID) {
        if (frequency_hz == 0) {
            TIM3->CR1 &= ~TIM_CR1_CEN;
            return;
        }

        TIM3->CR1 &= ~TIM_CR1_CEN;
        TIM3->CNT = 0;

        double clk = (double)SystemCoreClock;
        double ms = 1 / frequency_hz;

        double psc = (round)((clk * ms) / (65536.0) - 1.0);
        double arr = (round)((clk * ms) / (psc + 1.0) - 1.0);

        if (arr > 65535.0) {
            psc += 1.0;
            arr = (round)((clk * ms) / (psc + 1.0) - 1.0);
        }

        TIM3->ARR = arr;
        TIM3->PSC = psc;
        TIM3->CCR1 = (BUZZER_PWM_DC * (arr + 1.0));

        TIM3->EGR |= TIM_EGR_UG;
        TIM3->CCER |= TIM_CCER_CC1E;
        TIM3->CR1 |= TIM_CR1_CEN;
    }
};

 
/// @brief Retrieve the status of the note end flag.
/// @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
/// @return true;false
bool 	port_buzzer_get_note_timeout (uint32_t buzzer_id){
  return buzzers_arr[buzzer_id].note_end;
};
 
/// @brief Disable the PWM output of the timer that controls the frequency of the note and the timer that controls the duration of the note.
/// @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
void 	port_buzzer_stop (uint32_t buzzer_id){
  if (buzzer_id == BUZZER_0_ID) {
    TIM2 -> CR1 &= ~TIM_CR1_CEN;
    TIM3 -> CR1 &= ~TIM_CR1_CEN;
  };
};


/// @brief Configure the HW specifications of a given buzzer melody player.
/// @param buzzer_id Buzzer melody player ID. This index is used to select the element of the buzzers_arr[] array
void port_buzzer_init(uint32_t buzzer_id)
{
  port_buzzer_hw_t buzzzer = buzzers_arr[buzzer_id];
  port_system_gpio_config(buzzzer.p_port,buzzzer.pin,GPIO_MODE_ALTERNATE,GPIO_PUPDR_NOPULL);
  port_system_gpio_config_alternate(buzzzer.p_port,buzzzer.pin,buzzzer.alt_func);
  _timer_duration_setup(buzzer_id);
  _timer_pwm_setup(buzzer_id);
};




