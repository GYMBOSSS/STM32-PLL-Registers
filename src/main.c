#include <stdint.h>
#include "stm32f1xx.h"

#define HSE_STARTUP_TIMEOUT 0x5000 

void SystemClock_Config()
{
    uint32_t StartupCounter = 0;

    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) { 
        if (StartupCounter++ > HSE_STARTUP_TIMEOUT) return; // Fallback to HSI
    }

    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2; // 2 wait states for 72MHz
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // AHB = 72MHz
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB1 = 36MHz
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // APB2 = 72MHz

    RCC->CFGR |= RCC_CFGR_PLLSRC;     // PLL source = HSE
    RCC->CFGR &= ~RCC_CFGR_PLLXTPRE;  // HSE not divided
    RCC->CFGR |= RCC_CFGR_PLLMULL9;   // 8MHz * 9 = 72MHz

    RCC->CR |= RCC_CR_PLLON;
    StartupCounter = 0;
    while (!(RCC->CR & RCC_CR_PLLRDY)) { 
        if (StartupCounter++ > HSE_STARTUP_TIMEOUT) return;
    }

    // Switch system clock to PLL
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL; // Используем макрос вместо 0b10
    
    StartupCounter = 0;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) { 
        if (StartupCounter++ > HSE_STARTUP_TIMEOUT) return;
    }
}

// Делаем тип uint32_t, чтобы избежать переполжения при больших значениях
void delay(volatile uint32_t time)
{
    while(time--) {
        __asm__("nop");
    }
}

int main()
{
    SystemClock_Config();

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA5 -> Output push-pull, 2MHz
    GPIOA->CRL &= ~(0xF << 20);
    GPIOA->CRL |= (0x2 << 20);

    while(1)
    {
        GPIOA->BSRR = (1 << 5);       // LED ON
        delay(5000000);               // ~70 мс на 72 МГц
        
        GPIOA->BSRR = (1 << (5 + 16)); // LED OFF
        delay(5000000);               // ~70 мс на 72 МГц
    }
}
