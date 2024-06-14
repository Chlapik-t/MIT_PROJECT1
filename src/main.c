#include <stdbool.h>
#include <stm8s.h>
#include <stdio.h>
#include "main.h"
#include "milis.h"
#include "uart1.h"

typedef enum {NONE, START, READ} state_t;
state_t stav = NONE;

uint16_t index = 0;
uint16_t last_counter = 0;
uint64_t data = 0;

void init(void) {
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz

    // Piny
    GPIO_Init(GPIOE, GPIO_PIN_0, GPIO_MODE_IN_PU_IT); // DATA in PIN
    GPIO_Init(GPIOG, GPIO_PIN_0, GPIO_MODE_OUT_OD_HIZ_SLOW); // Trigger
    GPIO_Init(BTN_PORT, BTN_PIN, GPIO_MODE_IN_FL_NO_IT);

    // Interupty
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOE, EXTI_SENSITIVITY_RISE_FALL);
    ITC_SetSoftwarePriority(ITC_IRQ_PORTE, ITC_PRIORITYLEVEL_1);
    enableInterrupts();

    // Časovač
    TIM4_TimeBaseInit(TIM4_PRESCALER_16, 0xffff);
    TIM4_Cmd(ENABLE);

    // UART
    init_uart1();

    UART1_DeInit();
    UART1_Init(9600, 
               UART1_WORDLENGTH_8D, 
               UART1_STOPBITS_1,
               UART1_PARITY_NO, 
               UART1_SYNCMODE_CLOCK_DISABLE, 
               UART1_MODE_TXRX_ENABLE); 

    init_milis();
}

int main(void) {
    uint32_t time = 0;
    uint32_t poslední = 0;
    uint64_t j = 1LL << 39;
    uint8_t i = 0;

    init();

    while (1) {
        if ((milis() - time) > 10000 && !PUSH(BTN)) {
            time = milis();
            TIM4_GetCounter();
            stav = START;
        }

        if (stav == NONE) {
            poslední = milis();
        } else if (stav == START) {
            if (milis() - poslední < 19) {
                GPIO_WriteLow(GPIOG, GPIO_PIN_0);
            } else {
                poslední = milis();
                TIM4_SetCounter(0);
                last_counter = 0;
                index = 0;
                data = 0LL;
                GPIO_WriteHigh(GPIOG, GPIO_PIN_0);
                stav = READ;
            }
        } else if (stav == READ) {
            while (j) {
                if (data & j) {
                    putchar('1');
                } else {
                    putchar('0');
                }
                if (++i % 8 == 0) {
                    putchar(' ');
                }
                j >>= 1;
            }
            printf("\n");

            uint8_t temp_L = data >> 8;
            uint8_t temp_H = data >> 16;
            uint8_t RH_L = data >> 24;
            uint8_t RH_H = data >> 32;

            uint8_t kontrola = data;

            printf("data: 0x %02X %02X %02X %02X\n", RH_H, RH_L, temp_H, temp_L);
            printf("vlhkost: %d %%, teplota: %d.%d °C\n", RH_H, temp_H, temp_L);

            stav = NONE;
        }
    }

    return 0;
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
