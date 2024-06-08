#include <stdbool.h>
#include <stm8s.h>
//#include <stdio.h>
#include "main.h"
#include "milis.h"
//#include "delay.h"
//#include "uart1.h"
bool on = ture;
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6);
uint16_t temp11 = 0b0000000000000000;
void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz
    GPIO_Config();

    GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST) //D6  TIM4
                                                            
    GPIO_Init(GPIOE, GPIO_PIN_1, GPIO_MODE_IN_FL_IT)        //D2 DHT22 READ
    GPIO_Init(GPIOG, GPIO_PIN_0, GPIO_MODE_IN_FL_IT)        //D4 DHT11 READ

    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOE, EXTI_SENSITIVITY_RISE_FALL);     // nastavení priority přerušení
    ITC_SetSoftwarePriority(ITC_IRQ_PORTE, ITC_PRIORITYLEVEL_1);     // povolení přeruření     
    EnableInterrupts()

    CLK_PeripheralClockConfing(CLK_PERIPHERAL_TIMER4, ENABLE);
    TIM4_TimeBaseInit(TIM4_PRESCALER_16,69);
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_UTConfing(TIM4_IT_UPDATE, ENABLE);
    TIM4_Cmd(ENABLE);






    init_milis();
    //init_uart1();

void DHT_zapnut(void)
{

    for(volatile uint32_t i = 0;i <10000; i++);
    GPIO_WriteLow(GPIOC, GPIO_PIN_3);

    for(volatile uint32_t i = 0;i <1000; i++);
    GPIO_WriteHigh(GPIOC, GPIO_PIN_3);

    for(volatile uint32_t i = 0;i <30; i++);
    GPIO_WriteLow(GPIOC, GPIO_PIN_3);

}
}


int main(void)
{
  
    uint32_t time = 0;

    init();

    while (1) {         // Dořešit zapisování do paměti 
        DHT_zapnut();

        if (GPIO_READInputPin(GPIOG, GPIO_PIN_0)) 
        {
            temp11 << 1;
        }
        else 
        {
            temp11 << 0;
        }
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"

