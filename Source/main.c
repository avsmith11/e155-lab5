// main.c
// use interrupts to read the speed of a motor using interrupts
// Avery Smith
// avsmith@hmc.edu
// 10/25/24

// include device header
#include "../lib/STM32L432KC.h"

int us; // declare global microsecond variable

void main(){ 

    //configureClock();
    initTIM(TIM6, TIM_us);
    initTIM(TIM7, TIM_ms);
    gpioEnable(GPIO_PORT_A);

    // configure GPIO
    // output A to PA6
    // output B to PA8
    pinMode(PA6, GPIO_INPUT);
    pinMode(PA8, GPIO_INPUT);
    pinResistor(PA6, GPIO_PULL_UP);
    pinResistor(PA8, GPIO_PULL_UP);

    // enable and configure interrupts
    SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR2_EXTI5, 0b000); // set EXTI5 source to PA6
    SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR2_EXTI6, 0b000); // set EXTI6 source to PA8

    __enable_irq(); // globally enable interrupts

    EXTI->IMR1 |= (0b1 << EXTI_IMR1_IM6_Pos); // unmask PA6 interrupt
    EXTI->IMR1 |= (0b1 << EXTI_IMR1_IM8_Pos); // unmask PA8 interrupt

    EXTI->RTSR1 |= (0b1 << EXTI_RTSR1_RT6_Pos); // enable rising edge trigger for PA6
    EXTI->RTSR1 |= (0b1 << EXTI_RTSR1_RT8_Pos); // enable rising edge trigger for PA8
    EXTI->FTSR1 |= (0b1 << EXTI_FTSR1_FT6_Pos); // enable falling edge trigger for PA6
    EXTI->FTSR1 |= (0b1 << EXTI_FTSR1_FT8_Pos); // enable falling edge trigger for PA8
    
    NVIC->ISER[0] |= (0b1 << EXTI9_5_IRQn); // turn on EXTI9_5 interrupt in interrupt set-enable register

    volatile float T_interrupt = 0; // Time between interrupts in seconds
    volatile float T_revolution; // Rotation period in seconds
    volatile float revs; // motor rpm

    while (1) {
        // compute motor speed from us between interrupt
        // 4 interrupts per 1 pulse, 120 interrupts per rotation
        T_interrupt = 1e-6 * us; // set T_interrupt based on us (global variable set by interrupt handler)
        T_revolution = 480 * T_interrupt;
        revs = (1/T_revolution);

        //check interrupt timer to set speed to zero (1 second limit)
        if (read_timer(TIM7) >= 1000) {
            T_interrupt = 0;
        }
        printf("%d", read_timer(TIM7));
        printf("Motor Speed: %f rev/s\n", revs);


    }
}

void EXTI9_5_IRQHandler() {
    // set these variables to establish the interrupt case
    int triggerA = ((EXTI->PR1) & (0b1 << 6)); // 1 if A triggered interrupt, 0 otherwise
    int triggerB = ((EXTI->PR1) & (0b1 << 8)); //1 if B triggered interrupt, 0 otherwise
    int A = digitalRead(PA6); // 1 if A is high
    int B = digitalRead(PA8); // 1 if B is high

    // reset interrupt timer 
    restart_timer(TIM7);

    // read and reset counter
    int us_local = read_timer(TIM6);
    restart_timer(TIM6);

    if (triggerA) {
        EXTI->PR1 |= (0b1 << 6); // reset interrupt flag 6

        if ((A ^ B)) {
            us = us_local;
        }
        else{ 
            us = -us_local;
        }
    }

    else if (triggerB) {
        EXTI->PR1 |= (0b1 << 8); // reset interrupt flag 8

        if ((A ^ B)) {
            us = -us_local;
        }
        else{ 
            us = us_local;
        }
    }
}