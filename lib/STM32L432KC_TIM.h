// STM32F401RE_TIM.h
// Header for TIM functions

#ifndef STM32L4_TIM_H
#define STM32L4_TIM_H

#include <stdint.h> // Include stdint header
#include <stm32l432xx.h>
#include "STM32L432KC_GPIO.h"

// Definition for ms base vs us base
#define TIM_us 0
#define TIM_ms 1

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void initTIM(TIM_TypeDef * TIMx, int base_unit);
void delay(TIM_TypeDef * TIMx, uint32_t ms);
void restart_timer(TIM_TypeDef * TIMx);
uint32_t read_timer(TIM_TypeDef * TIMx);

#endif