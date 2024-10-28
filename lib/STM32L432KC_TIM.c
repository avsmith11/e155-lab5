// STM32F401RE_TIM.c
// TIM functions

#include "STM32L432KC_TIM.h"
#include "STM32L432KC_RCC.h"

void initTIM(TIM_TypeDef * TIMx, int base_unit){
  
  RCC->APB1ENR1 |= _VAL2FLD(RCC_APB1ENR1_TIM6EN, 1); // Enable clock to TIM6
  RCC->APB1ENR1 |= _VAL2FLD(RCC_APB1ENR1_TIM7EN, 1); // Enable clock to TIM7
  uint32_t pscval = 0;
  if (base_unit == TIM_us) {
    // Set prescaler to give 1 us time base
    pscval = (uint32_t) ((SystemCoreClock/1e6));
  }
  else if (base_unit == TIM_ms){
    // Set prescaler to give 1 ms time base
    pscval = (uint32_t) ((SystemCoreClock/1e4));
  }
  // Set prescaler division factor
  TIMx->PSC |= _VAL2FLD(TIM_PSC_PSC, (pscval - 1));

  // Generate an update event to update prescaler value
  TIMx->EGR |= 1;

  // Enable counter
  TIMx->CR1 |= 1; // Set CEN = 1
}

void delay(TIM_TypeDef * TIMx, uint32_t ms){
  TIMx->ARR |= _VAL2FLD(TIM_ARR_ARR, ms); // Set timer max count IS THIS PROPER SYNTAX?
  TIMx->EGR |= 1;     // Force update
  TIMx->SR &= ~(0x1); // Clear UIF
  TIMx->CNT = 0;      // Reset count

  while(!(TIMx->SR & 1)); // Wait for UIF to go high
}

void restart_timer(TIM_TypeDef * TIMx){
  TIMx->ARR |= _VAL2FLD(TIM_ARR_ARR, 0xFFFF); // set ARR high
  TIMx->CNT |= _VAL2FLD(TIM_CNT_CNT, 0x0000);// reset count
  TIMx->EGR |= 1; // Force update
  
}

uint32_t read_timer(TIM_TypeDef * TIMx){
  return TIMx->CNT;
}