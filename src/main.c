#include <stdbool.h>
#include <stm8s.h>
#include <stdio.h>
#include "main.h"
#include "milis.h"
#include "uart1.h"

typedef enum { START, READ_DATA, SLEEP } state_t;
state_t stav = SLEEP;

uint16_t current_index = 0;
uint16_t previous_counter = 0;
uint64_t data = 0;

void initialize_system(void) {
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

    GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(DATA_PORT, DATA_PIN, GPIO_MODE_IN_PU_IT);             //Čtecí 
    GPIO_Init(PULSE_PORT, PULSE_PIN, GPIO_MODE_OUT_OD_HIZ_SLOW);    // Zapínací pulse
    GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);        // prozátím LED později sepínání relé

    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB, EXTI_SENSITIVITY_RISE_FALL);
    ITC_SetSoftwarePriority(ITC_IRQ_PORTE, ITC_PRIORITYLEVEL_1);
    
    TIM2_TimeBaseInit(TIM2_PRESCALER_16, 0xFFFF);
    TIM2_Cmd(ENABLE);
    
    enableInterrupts();
    init_milis();
    init_uart1();
}

int main(void) {
    uint32_t current_time = 0;
    uint32_t last = 0;


    initialize_system();

    while (1) {
        if (milis() - current_time > 1000 ) {
            GPIO_WriteReverse(LED_PORT, LED_PIN);
            current_time = milis();
            printf("\nTimer value: %u\n", TIM2_GetCounter());
            stav = START;
        }

        switch (stav) {
            case SLEEP:
                last = milis();
                break;

            case START:
                if (milis() - last < 19) {
                    GPIO_WriteLow(PULSE_PORT, PULSE_PIN);
                } else {
                    last = milis();
                    TIM2_SetCounter(0);
                    previous_counter = 0;
                    current_index = 0;
                    data = 0LL;
                    GPIO_WriteHigh(PULSE_PORT, PULSE_PIN);
                    stav = READ_DATA;
                }
                break;

            case READ_DATA:
                if (milis() - last > 6) {
                    last = milis();

                    uint64_t m = 1LL << 39;
                    uint8_t i = 0;
                    {
                      if (data & m) {
                        putchar('1');
                    } else {
                        putchar('0');
                    }
                    if (++i % 8 == 0)
                        putchar(' ');
                    
                    m >>=1;
                    }
                    printf("\n");
                    uint8_t RH_H = data >> 32;
                    uint8_t RH_L = data >> 24;
                    uint8_t temp_H = data >> 16;
                    uint8_t temp_L = data >> 8;
                    uint8_t kontrola = data;
                    
                    printf("\n");
                    printf("RH: %d %%, Teplota: %d.%d °C\n", RH_H, temp_H, temp_L);

                    if (temp_H > 27 || RH_H > 75) {
                        GPIO_WriteHigh(GPIOD, GPIO_PIN_3);
                    } else {
                        GPIO_WriteLow(GPIOD, GPIO_PIN_3);
                    }

                    stav = SLEEP;
                }
                break;

            default:
                stav = SLEEP;
                break;
        }
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
