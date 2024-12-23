#include <ch32v20x.h>
#include <stdint.h>

#include "src/buttons.h"
#include "src/matrix.h"
#include "src/rtc.h"



static uint8_t update_clock = 0;



void systick_init(void)
{
    SysTick->CMP = (SystemCoreClock / 600) - 1;     // we want a 600Hz interrupt
    SysTick->CNT = 0;                               // clear counter
    SysTick->CTLR = 0xF;                            // start counter in /1 mode, enable interrupts, auto-reset counter
    SysTick->SR = 0;                                // clear count comparison flag

    NVIC_EnableIRQ(SysTicK_IRQn);                   // enable interrupt
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();

    // Enable clocks for GPIOA and GPIOB
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // configure RTC
    RTC_Init();

    // configure our matrix GPIOs and state
    matrix_init();

    // buttons to make our clock be a clock
    btn_init();

    // configure periodic interrupt
    systick_init();

    while (1) {
        __WFI();

        if (update_clock) {
            update_clock = 0;
            matrix_gettime();
        }
    }
}

uint16_t tcnt;

__attribute__((interrupt("WCH-Interrupt-fast")))
void SysTick_Handler(void)
{
    tcnt++;
    if (tcnt >= 600) tcnt = 0;

    uint8_t ret;

    ret = matrix_next();
    if (ret == MATRIX_COLS - 1) {
        update_clock = 1;
    }

    // do our button tasks
    if (tcnt % 10 == 0) {
        btn_process();
    }

    SysTick->SR = 0;
}
