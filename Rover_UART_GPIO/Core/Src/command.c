#include "command.h" /* Mesaj ayristiricisinin tur ve bildirimlerini alir. */
void Command_Reset(CommandParser *p) /* Mesaj durumunu temizleyen fonksiyonu tanimlar. */
{ /* Fonksiyon govdesini baslatir. */
    p->length = 0U; /* Biriktirilmis karakter sayisini sifirlar. */
    p->invalid = 0U; /* Hatali mesaj bayragini temizler. */
} /* Sifirlama fonksiyonunu bitirir. */
uint8_t Command_Feed(CommandParser *p, uint8_t byte, uint8_t out[2]) /* Tek UART baytini isler. */
{ /* Ayristirma fonksiyonunu baslatir. */
    if ((byte == '\r') || (byte == '\n')) /* CR veya LF geldiginde mesaj sonunu algilar. */
    { /* Mesaj sonu dalini baslatir. */
        uint8_t ready = (p->length == 2U) && (p->invalid == 0U); /* Yalnizca iki gecerli karakteri kabul eder. */
        if (ready != 0U) /* Gecerli mesaj varsa cikti tamponunu doldurur. */
        { /* Kopyalama blogunu baslatir. */
            out[0] = p->data[0]; /* Birinci pinin komutunu aktarir. */
            out[1] = p->data[1]; /* Ikinci pinin komutunu aktarir. */
        } /* Kopyalama blogunu bitirir. */
        Command_Reset(p); /* Sonraki mesaj icin durumu temizler. */
        return ready; /* Mesajin uygulanabilir olup olmadigini bildirir. */
    } /* Mesaj sonu dalini bitirir. */
    if ((byte < '0') || (byte > '2') || (p->length >= 2U)) /* Gecersiz karakteri ve fazla uzun mesaji saptar. */
    { /* Hata dalini baslatir. */
        p->invalid = 1U; /* Satir sonuna kadar tum mesaji reddeder. */
    } /* Hata dalini bitirir. */
    else /* Tamponda yer ve gecerli karakter varsa bu dali kullanir. */
    { /* Biriktirme dalini baslatir. */
        p->data[p->length++] = byte; /* Karakteri kaydeder ve sayaci artirir. */
    } /* Biriktirme dalini bitirir. */
    return 0U; /* Satir sonu gelmedigi icin henuz komut uygulamaz. */
} /* Ayristirma fonksiyonunu bitirir. */
