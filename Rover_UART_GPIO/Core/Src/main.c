#include "stm32f4xx_hal.h" /* STM32F4 HAL fonksiyon ve turlerini ekler. */
#include "command.h" /* Iki karakterli mesaj ayristiricisini ekler. */
UART_HandleTypeDef huart2; /* USART2 donaniminin HAL durumunu tutar. */
static void Error_Handler(void); /* Baslatma hatasi fonksiyonunu bildirir. */
static void SystemClock_Config(void); /* Sistem saatini ayarlayan fonksiyonu bildirir. */
static void MX_GPIO_Init(void); /* GPIO baslatma fonksiyonunu bildirir. */
static void MX_USART2_UART_Init(void); /* UART baslatma fonksiyonunu bildirir. */
static void ApplyPin(uint16_t pin, uint8_t command) /* Tek cikis pinine komut uygular. */
{ /* Pin kontrol fonksiyonunu baslatir. */
    if (command == '2') /* TOGGLE komutunu saptar. */
    { /* TOGGLE dalini baslatir. */
        HAL_GPIO_TogglePin(GPIOA, pin); /* Cikis mandalinin mevcut degerini tersler. */
    } /* TOGGLE dalini bitirir. */
    else /* LOW veya HIGH komutunu ele alir. */
    { /* Yazma dalini baslatir. */
        HAL_GPIO_WritePin(GPIOA, pin, command == '1' ? GPIO_PIN_SET : GPIO_PIN_RESET); /* 1 icin HIGH, 0 icin LOW yazar. */
    } /* Yazma dalini bitirir. */
} /* Pin kontrol fonksiyonunu bitirir. */
int main(void) /* Programin giris noktasini tanimlar. */
{ /* Ana fonksiyon govdesini baslatir. */
    CommandParser parser = {0}; /* Mesaj durumunu bos olarak baslatir. */
    uint8_t byte = 0U; /* Alinan tek UART baytini tutar. */
    uint8_t command[2] = {0}; /* Dogrulanmis iki pin komutunu tutar. */
    uint32_t lastByte = 0U; /* Son karakterin alinma zamanini tutar. */
    HAL_Init(); /* HAL'i, SysTick'i ve temel donanim durumunu baslatir. */
    SystemClock_Config(); /* 16 MHz dahili HSI saatini secer. */
    MX_GPIO_Init(); /* PA5 ve PA6 pinlerini LOW cikis olarak baslatir. */
    MX_USART2_UART_Init(); /* USART2'yi 115200 baud ve 8N1 ile baslatir. */
    while (1) /* Komutlari surekli dinleyen donguyu baslatir. */
    { /* Dongu govdesini baslatir. */
        HAL_StatusTypeDef status = HAL_UART_Receive(&huart2, &byte, 1U, 10U); /* Bir bayti en fazla 10 ms bekler. */
        if (((parser.length != 0U) || (parser.invalid != 0U)) && ((uint32_t)(HAL_GetTick() - lastByte) > 1000U)) /* Bekleyen mesaji 1 saniye boslukta iptal eder. */
        { /* Zaman asimi dalini baslatir. */
            Command_Reset(&parser); /* Eski kismi mesajin yeni mesajla birlesmesini onler. */
        } /* Zaman asimi dalini bitirir. */
        if (status == HAL_OK) /* Bayt basariyla alindiysa onu isler. */
        { /* Alim dalini baslatir. */
            lastByte = HAL_GetTick(); /* Son alinma zamanini gunceller. */
            if (Command_Feed(&parser, byte, command) != 0U) /* Tam ve gecerli mesaji denetler. */
            { /* Komut uygulama blogunu baslatir. */
                ApplyPin(GPIO_PIN_5, command[0]); /* Ilk karakteri birinci cikis PA5'e uygular. */
                ApplyPin(GPIO_PIN_6, command[1]); /* Ikinci karakteri ikinci cikis PA6'ya uygular. */
            } /* Komut uygulama blogunu bitirir. */
        } /* Alim dalini bitirir. */
        else if (status != HAL_TIMEOUT) /* Normal bos bekleme disindaki UART hatasini ele alir. */
        { /* UART hata dalini baslatir. */
            __HAL_UART_CLEAR_OREFLAG(&huart2); /* SR ve DR okunarak alinma hata bayraklarini temizler. */
            Command_Reset(&parser); /* Hatadan etkilenmis mesaji iptal eder. */
            parser.invalid = 1U; /* Sonraki satir sonuna kadar karakterleri reddeder. */
        } /* UART hata dalini bitirir. */
    } /* Sonsuz donguyu bitirir. */
} /* Ana fonksiyonu bitirir. */
static void SystemClock_Config(void) /* Sistem saati ayarlarini uygular. */
{ /* Saat fonksiyonunu baslatir. */
    RCC_OscInitTypeDef osc = {0}; /* Osilator ayarlarini sifirlar. */
    RCC_ClkInitTypeDef clk = {0}; /* Veri yolu saat ayarlarini sifirlar. */
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI; /* Dahili HSI osilatorunu secer. */
    osc.HSIState = RCC_HSI_ON; /* HSI osilatorunu acar. */
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT; /* Fabrika kalibrasyonunu kullanir. */
    osc.PLL.PLLState = RCC_PLL_OFF; /* Bu uygulama icin PLL'i kapali tutar. */
    if (HAL_RCC_OscConfig(&osc) != HAL_OK) Error_Handler(); /* Osilator hatasinda programi durdurur. */
    clk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2; /* Dort saat alanini yapilandirir. */
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI; /* Sistem saatinin kaynagini HSI yapar. */
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1; /* AHB'yi 16 MHz yapar. */
    clk.APB1CLKDivider = RCC_HCLK_DIV1; /* APB1'i 16 MHz yapar. */
    clk.APB2CLKDivider = RCC_HCLK_DIV1; /* APB2'yi 16 MHz yapar. */
    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_0) != HAL_OK) Error_Handler(); /* Saat ayarini uygular ve hatayi denetler. */
} /* Saat fonksiyonunu bitirir. */
static void MX_GPIO_Init(void) /* Iki GPIO cikisini yapilandirir. */
{ /* GPIO fonksiyonunu baslatir. */
    GPIO_InitTypeDef gpio = {0}; /* GPIO ayar yapisini sifirlar. */
    __HAL_RCC_GPIOA_CLK_ENABLE(); /* GPIOA donaniminin saatini acar. */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_RESET); /* Cikisa gecmeden iki mandali LOW yapar. */
    gpio.Pin = GPIO_PIN_5 | GPIO_PIN_6; /* PA5 ve PA6 pinlerini secer. */
    gpio.Mode = GPIO_MODE_OUTPUT_PP; /* Push-pull dijital cikis modunu secer. */
    gpio.Pull = GPIO_NOPULL; /* Dahili pull-up ve pull-down direncini kapatir. */
    gpio.Speed = GPIO_SPEED_FREQ_LOW; /* Dusuk hizli GPIO anahtarlamasini secer. */
    HAL_GPIO_Init(GPIOA, &gpio); /* Secilen GPIO ayarlarini uygular. */
} /* GPIO fonksiyonunu bitirir. */
static void MX_USART2_UART_Init(void) /* USART2 seri haberlesmesini yapilandirir. */
{ /* UART fonksiyonunu baslatir. */
    huart2.Instance = USART2; /* Kullanilacak USART donanimini secer. */
    huart2.Init.BaudRate = 115200U; /* Saniyedeki sembol sayisini belirler. */
    huart2.Init.WordLength = UART_WORDLENGTH_8B; /* Veri uzunlugunu 8 bit yapar. */
    huart2.Init.StopBits = UART_STOPBITS_1; /* Bir stop biti secer. */
    huart2.Init.Parity = UART_PARITY_NONE; /* Parite denetimini kapatir. */
    huart2.Init.Mode = UART_MODE_TX_RX; /* Gonderme ve alma yonlerini acar. */
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE; /* RTS/CTS akis kontrolunu kapatir. */
    huart2.Init.OverSampling = UART_OVERSAMPLING_16; /* 16 kat ornekleme secer. */
    if (HAL_UART_Init(&huart2) != HAL_OK) Error_Handler(); /* UART'i baslatir ve hatayi denetler. */
} /* UART fonksiyonunu bitirir. */
static void Error_Handler(void) /* Baslatma hatasinda guvenli durumu uygular. */
{ /* Hata fonksiyonunu baslatir. */
    __HAL_RCC_GPIOA_CLK_ENABLE(); /* Cikislari sifirlamak icin GPIOA saatini acar. */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_RESET); /* Cikis mandallarini LOW yapar. */
    __disable_irq(); /* Kesmeleri devre disi birakir. */
    while (1) {} /* Hata sonrasi calismayi durdurur. */
} /* Hata fonksiyonunu bitirir. */
