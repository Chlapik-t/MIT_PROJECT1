#include <stdbool.h>
#include <stm8s.h>
#include <stdio.h>
#include "main.h"
#include "milis.h"
//#include "delay.h"
#include "uart1.h"

void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz

    //Piny
    GPIO_Init(GPIOE,GPIO_PIN_0,GPIO_MODE_IN_PU_IT); // DATA in PIN
    GPIO_Init(GPIOG, GPIO_PIN_0,GPIO_MODE_OUT_OD_HIZ_SLOW); // Trigger
    GPIO_Init(BTN_PORT,BTN_PIN,GPIO_MODE_IN_FL_NO_IT);
    

   
    //Interupty
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOE,EXTI_SENSITIVITY_RISE_FALL);
    ITC_SetSoftwarePriority(ITC_IRQ_PORTE,ITC_PRIORITYLEVEL_1);
    enableInterrupts();

    //Časovač
    TIM4_TimeBaseInit(TIM4_PRESCALER_16,0xffff);
    TIM4_Cmd(ENABLE);

    //UART
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


int main(void)
{
  
    uint32_t time = 0;
    uint32_t poslední = 0;

    init();

    while (1) {
        if ((milis() - time) > 10000 && !PUSH(BTN)) {
            time = milis();
            TIM4_GetCounter();
        }
    }

}
/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
