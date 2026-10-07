#include "stm32f4xx_hal.h" /* HAL donanim turlerini ve fonksiyonlarini ekler. */
void HAL_MspInit(void) /* HAL'in genel alt seviye baslatma kancasini tanimlar. */
{ /* Genel baslatma govdesini baslatir. */
    __HAL_RCC_SYSCFG_CLK_ENABLE(); /* Sistem yapilandirma saatini acar. */
    __HAL_RCC_PWR_CLK_ENABLE(); /* Guc kontrol donanimi saatini acar. */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4); /* NVIC oncelik gruplamasini belirler. */
} /* Genel baslatma fonksiyonunu bitirir. */
void HAL_UART_MspInit(UART_HandleTypeDef *uart) /* HAL UART baslatmasinin donanim kancasini tanimlar. */
{ /* UART donanim baslatmasini baslatir. */
    GPIO_InitTypeDef gpio = {0}; /* Pin ayar yapisini sifirlar. */
    if (uart->Instance == USART2) /* Yalnizca USART2 icin ayar yapar. */
    { /* USART2 dalini baslatir. */
        __HAL_RCC_USART2_CLK_ENABLE(); /* USART2 cevre birimi saatini acar. */
        __HAL_RCC_GPIOA_CLK_ENABLE(); /* UART pinlerinin GPIOA saatini acar. */
        gpio.Pin = GPIO_PIN_2 | GPIO_PIN_3; /* PA2 TX ve PA3 RX pinlerini secer. */
        gpio.Mode = GPIO_MODE_AF_PP; /* Alternatif fonksiyon push-pull modunu secer. */
        gpio.Pull = GPIO_PULLUP; /* Bos seri hatta HIGH seviyesi icin pull-up secer. */
        gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH; /* UART pin hizini ayarlar. */
        gpio.Alternate = GPIO_AF7_USART2; /* AF7 ile pinleri USART2'ye baglar. */
        HAL_GPIO_Init(GPIOA, &gpio); /* UART pin ayarlarini uygular. */
    } /* USART2 dalini bitirir. */
} /* UART donanim baslatmasini bitirir. */
