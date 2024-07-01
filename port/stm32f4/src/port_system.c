/**
 * @file port_system.c
 * @brief File that defines the functions that are related to the access to the specific HW of the microcontroller.
 * @author David Hernández Sola
 * @author Rodrigo Pérez del Pulgar Almodovar
 * @date 2024-01-01
 */

/* Includes ------------------------------------------------------------------*/
#include "port_system.h"


/* Defines -------------------------------------------------------------------*/
#define HSI_VALUE ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz */

/* GLOBAL VARIABLES */
static volatile uint32_t msTicks = 0; /*!< Variable to store millisecond ticks. @warning **It must be declared volatile!** Just because it is modified in an ISR. **Add it to the definition** after *static*. */

/* These variables are declared extern in CMSIS (system_stm32f4xx.h) */
uint32_t SystemCoreClock = HSI_VALUE;                                               /*!< Frequency of the System clock */
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9}; /*!< Prescaler values for AHB bus */
const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};                          /*!< Prescaler values for APB bus */

//------------------------------------------------------
// SYSTEM CONFIGURATION
//------------------------------------------------------
/**
 * @brief  Setup the microcontroller system
 *         Initialize the FPU setting, vector table location and External memory
 *         configuration.
 *
 * @note   This function is called at startup by CMSIS in startup_stm32f446xx.s.
 */
void SystemInit(void)
{
/* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
  SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */
#endif

#if defined(DATA_IN_ExtSRAM) || defined(DATA_IN_ExtSDRAM)
  SystemInit_ExtMemCtl();
#endif /* DATA_IN_ExtSRAM || DATA_IN_ExtSDRAM */

  /* Configure the Vector Table location -------------------------------------*/
#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#endif                                                 /* USER_VECT_TAB_ADDRESS */
}

/**
 * @brief System Clock Configuration
 *
 * @attention This function should NOT be accesible from the outside to avoid configuration problems.
 * @note This function starts a system timer that generates a SysTick every 1 ms.
 */
void system_clock_config(void)
{
  /** Configure the main internal regulator output voltage */
  /* Power controller (PWR) */
  /* Control the main internal voltage regulator output voltage to achieve a trade-off between performance and power consumption when the device does not operate at the maximum frequency */
  PWR->CR &= ~PWR_CR_VOS; // Clean and set value
  PWR->CR |= (PWR_CR_VOS & (POWER_REGULATOR_VOLTAGE_SCALE3 << PWR_CR_VOS_Pos));

  /* Initializes the RCC Oscillators. */
  /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
  RCC->CR &= ~RCC_CR_HSITRIM; // Clean and set value
  RCC->CR |= (RCC_CR_HSITRIM & (RCC_HSI_CALIBRATION_DEFAULT << RCC_CR_HSITRIM_Pos));

  /* RCC Clock Config */
  /* Initializes the CPU, AHB and APB buses clocks */
  /* To correctly read data from FLASH memory, the number of wait states (LATENCY)
      must be correctly programmed according to the frequency of the CPU clock
      (HCLK) and the supply voltage of the device. */

  /* Increasing the number of wait states because of higher CPU frequency */
  FLASH->ACR = FLASH_ACR_LATENCY_2WS; /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */

  /* Change in clock source is performed in 16 clock cycles after writing to CFGR */
  RCC->CFGR &= ~RCC_CFGR_SW; // Clean and set value
  RCC->CFGR |= (RCC_CFGR_SW & (RCC_CFGR_SW_HSI << RCC_CFGR_SW_Pos));

  /* Update the SystemCoreClock global variable */
  SystemCoreClock = HSI_VALUE >> AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos];

  /* Configure the source of time base considering new system clocks settings */
  SysTick_Config(SystemCoreClock / (1000U / TICK_FREQ_1KHZ)); /* Set Systick to 1 ms */
}

size_t port_system_init()
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  /* Configure Flash prefetch, Instruction cache, Data cache */
  /* Instruction cache enable */
  FLASH->ACR |= FLASH_ACR_ICEN;

  /* Data cache enable */
  FLASH->ACR |= FLASH_ACR_DCEN;

  /* Prefetch cache enable */
  FLASH->ACR |= FLASH_ACR_PRFTEN;

  /* Set Interrupt Group Priority */
  NVIC_SetPriorityGrouping(NVIC_PRIORITY_GROUP_4);

  /* Use systick as time base source and configure 1ms tick (default clock after Reset is HSI) */
  /* Configure the SysTick IRQ priority. It must be the highest (lower number: 0)*/
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0U, 0U)); /* Tick interrupt priority */

  /* Init the low level hardware */
  /* Reset and clock control (RCC) */
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; /* Syscfg clock enabling */

  /* Peripheral clock enable register */
  RCC->APB1ENR |= RCC_APB1ENR_PWREN; /* PWREN: Power interface clock enable */

  /* Configure the system clock */
  system_clock_config();

  return 0;
}

//------------------------------------------------------
// TIMER RELATED FUNCTIONS
//------------------------------------------------------

/// @brief Get the count of the System tick in milliseconds.
/// @return uint32_t
uint32_t port_system_get_millis()
{
  return msTicks; /* ms */
}

/// @brief Sets the number of milliseconds since the system started.
/// @param ms New number of milliseconds since the system started.
void port_system_set_millis(uint32_t ms)
{
  msTicks = ms;
}

/// @brief Wait for some milliseconds.
/// @param ms Number of milliseconds to wait
void port_system_delay_ms(uint32_t ms)
{
  uint32_t tickstart = port_system_get_millis();

  while ((port_system_get_millis() - tickstart) < ms)
  {
  }
}
/// @brief Wait for some milliseconds from a time reference.
/// @param p_t Pointer to the time reference
/// @param ms Number of milliseconds to wait
void port_system_delay_until_ms(uint32_t *p_t, uint32_t ms)
{
  uint32_t until = *p_t + ms;
  uint32_t now = port_system_get_millis();
  if (until > now)
  {
    port_system_delay_ms(until - now);
  }
  *p_t = port_system_get_millis();
}
/// @brief Suspend Tick increment.
void port_system_systick_suspend()
{
 SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}
/// @brief Resume Tick increment.
void port_system_systick_resume()
{
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}



//------------------------------------------------------
// GPIO RELATED FUNCTIONS
//------------------------------------------------------

/// @brief Configure the mode and pull of a GPIO.
/// @param p_port Port of the GPIO (CMSIS struct like)
/// @param pin Pin/line of the GPIO (index from 0 to 15)
/// @param mode Input, output, alternate, or analog
/// @param pupd Pull-up, pull-down, or no-pull
void port_system_gpio_config(GPIO_TypeDef *p_port, uint8_t pin, uint8_t mode, uint8_t pupd)
{
  if (p_port == GPIOA)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; /* GPIOA_CLK_ENABLE */
  }
  else if (p_port == GPIOB)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; /* GPIOB_CLK_ENABLE */
  }
  else if (p_port == GPIOC)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; /* GPIOC_CLK_ENABLE */
  }

  /* Clean ( &=~ ) by displacing the base register and set the configuration ( |= ) */
  p_port->MODER &= ~(GPIO_MODER_MODER0 << (pin * 2U));
  p_port->MODER |= (mode << (pin * 2U));

  p_port->PUPDR &= ~(GPIO_PUPDR_PUPD0 << (pin * 2U));
  p_port->PUPDR |= (pupd << (pin * 2U));
}
/// @brief Configure the external interruption or event of a GPIO.
/// @param p_port Port of the GPIO (CMSIS struct like)
/// @param pin Pin/line of the GPIO (index from 0 to 15)
/// @param mode Trigger mode can be a combination (OR) of: (i) direction: rising edge (0x01), falling edge (0x02), (ii) event request (0x04), or (iii) interrupt request (0x08).
void port_system_gpio_config_exti(GPIO_TypeDef *p_port, uint8_t pin, uint32_t mode)
{
  uint32_t port_selector = 0;

  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  /* SYSCFG external interrupt configuration register */
  if (p_port == GPIOA)
  {
    port_selector = 0;
  }
  else if (p_port == GPIOB)
  {
    port_selector = 1;
  }
  else if (p_port == GPIOC)
  {
    port_selector = 2;
  }

  uint32_t base_mask = 0x0FU;
  uint32_t displacement = (pin % 4) * 4;

  SYSCFG->EXTICR[pin / 4] &= ~(base_mask << displacement);
  SYSCFG->EXTICR[pin / 4] |= (port_selector << displacement);

  /* Rising trigger selection register (EXTI_RTSR) */
  EXTI->RTSR &= ~BIT_POS_TO_MASK(pin);
  if (mode & TRIGGER_RISING_EDGE)
  {
    EXTI->RTSR |= BIT_POS_TO_MASK(pin);
  }

  /* Falling trigger selection register (EXTI_FTSR) */
  EXTI->FTSR &= ~BIT_POS_TO_MASK(pin);
  if (mode & TRIGGER_FALLING_EDGE)
  {
    EXTI->FTSR |= BIT_POS_TO_MASK(pin);
  }

  /* Event mask register (EXTI_EMR) */
  EXTI->EMR &= ~BIT_POS_TO_MASK(pin);
  if (mode & TRIGGER_ENABLE_EVENT_REQ)
  {
    EXTI->EMR |= BIT_POS_TO_MASK(pin);
  }

  /* Clear EXTI line configuration */
  EXTI->IMR &= ~BIT_POS_TO_MASK(pin);

  /* Interrupt mask register (EXTI_IMR) */
  if (mode & TRIGGER_ENABLE_INTERR_REQ)
  {
    EXTI->IMR |= BIT_POS_TO_MASK(pin);
  }
}

/// @brief Enable interrupts of a GPIO line (pin)
/// @param pin Pin/line of the GPIO (index from 0 to 15)
/// @param priority Priority level (from highest priority: 0, to lowest priority: 15)
/// @param subpriority Subpriority level (from highest priority: 0, to lowest priority: 15)
void port_system_gpio_exti_enable(uint8_t pin, uint8_t priority, uint8_t subpriority)
{
  NVIC_SetPriority(GET_PIN_IRQN(pin), NVIC_EncodePriority(NVIC_GetPriorityGrouping(), priority, subpriority));
  NVIC_EnableIRQ(GET_PIN_IRQN(pin));
}

/// @brief Disable interrupts of a GPIO line (pin)
/// @param pin Pin/line of the GPIO (index from 0 to 15)
void port_system_gpio_exti_disable(uint8_t pin)
{
  NVIC_DisableIRQ(GET_PIN_IRQN(pin));
}

/// @brief Configure the alternate function of a GPIO.
/// @param p_port Port of the GPIO (CMSIS struct like)
/// @param pin Pin/line of the GPIO (index from 0 to 15)
/// @param alternate Alternate function number (values from 0 to 15) according to table of the datasheet: "Table 11. Alternate function".
void port_system_gpio_config_alternate(GPIO_TypeDef *p_port, uint8_t pin, uint8_t alternate)
{
  uint32_t base_mask = 0x0FU;
  uint32_t displacement = (pin % 8) * 4;

  p_port->AFR[(uint8_t)(pin / 8)] &= ~(base_mask << displacement);
  p_port->AFR[(uint8_t)(pin / 8)] |= (alternate << displacement);
}

// ------------------------------------------------------
// POWER RELATED FUNCTIONS
// ------------------------------------------------------

/// @brief Read the digital value of a GPIO.
/// @param p_port Port of the GPIO (CMSIS struct like)
/// @param pin Pin/line of the GPIO (index from 0 to 15)
/// @return true if the GPIO was HIGH;false if the GPIO was LOW
bool port_system_gpio_read	(	GPIO_TypeDef * 	p_port, uint8_t 	pin ){
  bool value = (bool)(p_port-> IDR & BIT_POS_TO_MASK(pin));
  return value;
}	

/// @brief Write a digital value in a GPIO atomically.
/// @param p_port Port of the GPIO (CMSIS struct like)
/// @param pin Pin/line of the GPIO (index from 0 to 15)
/// @param value Boolean value to set the GPIO to HIGH (1, true) or LOW (0, false)
void port_system_gpio_write	(	GPIO_TypeDef * 	p_port, uint8_t pin, bool value){
  
  uint32_t mask = BIT_POS_TO_MASK(pin);
  if(value){
    p_port -> BSRR |= mask;
  }
  else{
    p_port -> BSRR |= mask << 16; //El registro BSSR se resetea de esta manera
  }

}	

/// @brief Toggle the value of a GPIO.
/// @param p_port Port of the GPIO (CMSIS struct like)
/// @param pin Pin/line of the GPIO (index from 0 to 15)
void port_system_gpio_toggle	(	GPIO_TypeDef * 	p_port, uint8_t pin){
  bool value = port_system_gpio_read(p_port,pin);
  if(value == HIGH){
    port_system_gpio_write(p_port,pin,LOW);
  }
  else{
    port_system_gpio_write(p_port,pin,HIGH);
  }
}	

/// @brief Set the system in stop mode for low power consumption.
void port_system_power_stop()
{
 MODIFY_REG(PWR->CR, (PWR_CR_PDDS | PWR_CR_LPDS), PWR_CR_LPDS);   // Select the regulator state in Stop mode: Set PDDS and LPDS bits according to PWR_Regulator value
 SCB->SCR |= ((uint32_t)SCB_SCR_SLEEPDEEP_Msk);   // Set SLEEPDEEP bit of Cortex System Control Register
 __WFI(); // Select Stop mode entry : Request Wait For Interrupt
 SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP_Msk); // Reset SLEEPDEEP bit of Cortex System Control Register
}

/// @brief Set the system in sleep mode for low power consumption.
void port_system_power_sleep()
{
 MODIFY_REG(PWR->CR, (PWR_CR_PDDS | PWR_CR_LPDS), PWR_CR_LPDS);   // Select the regulator state in Stop mode: Set PDDS and LPDS bits according to PWR_Regulator value
 SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);   // Reset SLEEPDEEP bit of Cortex System Control Register
 __WFI(); // Select Sleep mode entry : Request Wait For Interrupt
}

/// @brief Enable low power consumption in sleep mode.
/// @param void This function does not take any parameters.
void port_system_sleep(void)
{
  port_system_systick_suspend();
  port_system_power_sleep();

}