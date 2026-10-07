#include "stm32f4xx_hal.h" /* HAL zaman tabani ve CMSIS bildirimlerini ekler. */
void SysTick_Handler(void) /* Her 1 ms olusan sistem kesmesini tanimlar. */
{ /* Kesme govdesini baslatir. */
    HAL_IncTick(); /* HAL milisaniye sayacini artirir. */
} /* Sistem kesmesini bitirir. */
void HardFault_Handler(void) /* Islemci donanim hata kesmesini tanimlar. */
{ /* Hata kesmesini baslatir. */
    while (1) {} /* Hata halinde hata ayiklayici incelemesi icin bekler. */
} /* Hata kesmesini bitirir. */
