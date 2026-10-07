#ifndef COMMAND_H /* Basligin birden fazla eklenmesini onler. */
#define COMMAND_H /* Baslik korumasini etkinlestirir. */
#include <stdint.h> /* Sabit genislikli tamsayi turlerini saglar. */
typedef struct { /* UART mesajinin biriktirme durumunu tanimlar. */
    uint8_t data[2]; /* Iki GPIO icin iki komut karakterini tutar. */
    uint8_t length; /* Simdiye kadar alinan karakter sayisini tutar. */
    uint8_t invalid; /* Hatali veya fazla karakterli mesaji isaretler. */
} CommandParser; /* Yapiya kullanisli bir tur adi verir. */
void Command_Reset(CommandParser *p); /* Yeni mesaj icin durumu sifirlar. */
uint8_t Command_Feed(CommandParser *p, uint8_t byte, uint8_t out[2]); /* Gecerli tamamlanmis mesajda 1 doner. */
#endif /* Baslik korumasini sonlandirir. */
