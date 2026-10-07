# İTÜ Rover 2026 - UART ile iki GPIO kontrolü

Ödevin **6. Kodlama** bölümü. Hedef: **NUCLEO-F446RE / STM32F446RET6**. STM32CubeIDE projesi ve STM32F4 HAL sürücüleri dahildir.

**Doğrulama:** STM32CubeIDE 2.2.0 ile Debug derlemesi: **0 hata / 0 uyarı**. Gerçek C ayrıştırıcısı ve pin komut dağıtımı ARM emülatöründe **86 testi geçti**. GPIO HAL çağrıları test doubles ile temsil edildi; UART donanımı, saat kararlılığı ve fiziksel pin gerilimleri ölçülmedi.

## Bağlantı

| İşlev | Pin | Kart bağlantısı |
|---|---|---|
| Çıkış 1 | PA5 | Arduino D13 / CN5 pin 6; kartın LD2 LED'i |
| Çıkış 2 | PA6 | Arduino D12 / CN5 pin 5 |
| UART TX | PA2 | USART2 TX / ST-Link Virtual COM |
| UART RX | PA3 | USART2 RX / ST-Link Virtual COM |

Bilgisayarı kartın **ST-Link USB bağlantısına** veri destekli USB kablosuyla bağlayın. Standart MB1136 kartında USART2 için mevcut Virtual COM bağlantısını kullanın; kart revizyonu veya değiştirilmiş lehim köprüleri varsa UM1724'e göre doğrulayın. PA6'yı multimetre/lojik analizörle ölçün veya PA6 -> 330 ohm -> LED anot, LED katot -> GND bağlayın. GPIO doğrudan motor sürmez; mantık seviyeleri yaklaşık 0 V ve 3,3 V'tur.

## CubeIDE'de açma ve yükleme

1. **File > Import > General > Existing Projects into Workspace** seçin.
2. Kök dizin olarak `Rover_UART_GPIO` klasörünü gösterin. Projeyi içe aktarın.
3. **Project > Build Project** ile derleyin. Hedef `STM32F446RETx`, semboller `STM32F446xx` ve `USE_HAL_DRIVER` olmalıdır.
4. Kart bağlıyken **Run > Debug Configurations > STM32 C/C++ Application** altında ST-Link / SWD seçin; ELF dosyası `Debug/Rover_UART_GPIO.elf` olmalıdır.
5. **Debug** ile Flash'a yükleyin; **Resume** ile çalıştırın. Terminal için kartın Virtual COM portunu seçin.

Proje ST'nin resmi F446RE CubeIDE şablonundan türetilmiştir. `.project` ve `.cproject` hazırdır; harici STM32Cube deposuna yol bağımlılığı yoktur. HAL ve CMSIS dosyaları proje içindedir. `.ioc` dosyası ve CubeMX ekran görüntüsü üretilmedi; kullanılan gerçek HAL ve IDE yapılandırmaları raporda tablolarla belgelenmiştir. CubeIDE 2.x sürümlerinde grafik pin/saat yapılandırması ayrı STM32CubeMX uygulamasındadır.

## Terminal ve mesaj biçimi

- **115200 baud, 8 data bit, no parity, 1 stop bit (8N1)**.
- Flow control: **None**. Terminal gönderimi: **ASCII**, hex değil.
- Her mesaj iki karakter ve **Enter / CR / LF** ile biter. Örnek: `21\r\n`.
- İlk karakter PA5'i, ikinci karakter PA6'yı kontrol eder: `0` LOW, `1` HIGH, `2` TOGGLE.
- İlk açılışta iki çıkış LOW'dur. `11` yazıp Enter'a basınca ikisi HIGH olur.
- CRLF yalnızca bir komut uygular. Boş satır etkisizdir.
- Eksik, fazla uzun, boşluk içeren veya 0/1/2 dışı karakter içeren satır uygulanmaz. Örneğin `111` Enter ve `x21` Enter çıkışları değiştirmez.
- Kısmi mesajda karakterler arası 1 saniyeden uzun boşluk oluşursa biriktirme iptal edilir. UART alım hatasında çerçeve atılır.
- Firmware cevap/echo göndermez. PuTTY'de local echo ve local line editing açık seçilebilir; gerçek sonucu GPIO/LED üzerinden kontrol edin. Hercules'te Send alanına ASCII komut ve satır sonu ekleyin.

| Komut | PA5 | PA6 |
|---|---|---|
| 00 | LOW | LOW |
| 01 | LOW | HIGH |
| 02 | LOW | TOGGLE |
| 10 | HIGH | LOW |
| 11 | HIGH | HIGH |
| 12 | HIGH | TOGGLE |
| 20 | TOGGLE | LOW |
| 21 | TOGGLE | HIGH |
| 22 | TOGGLE | TOGGLE |

TOGGLE önceki çıkışı tersler. İki GPIO çağrısı sıralıdır; aynı saat çevriminde eşzamanlı değiştirme garantisi verilmez.

## Yapılandırma özeti

HSI 16 MHz; PLL kapalı; SYSCLK/HCLK/PCLK1/PCLK2 = 16 MHz; Flash latency 0; SysTick = 1 ms. PA5/PA6: Output Push-Pull, No Pull, Low Speed, başlangıç LOW. PA2/PA3: AF7 USART2, Push-Pull, Pull-Up, Very High Speed. UART polling ile bir bayt alınır, çağrı zaman aşımı 10 ms'dir. RTOS, DMA ve USART2 kesmesi kullanılmaz.

## Dosyalar ve testler

- `Rover_UART_GPIO/Core/Src/main.c`: HAL başlangıcı ve GPIO komutları.
- `command.c` / `Core/Inc/command.h`: satır tabanlı mesaj ayrıştırma.
- `stm32f4xx_hal_msp.c`: USART2 saat ve pin başlangıcı.
- `stm32f4xx_it.c`: SysTick zaman tabanı.
- `output/pdf/ITU_Rover_6_Kodlama_Raporu.pdf`: Türkçe rapor ve satır açıklamalı kod eki.
- `build_success.txt`: gerçek CubeIDE derleme günlüğü.
- `test_results.json`: 86 emülasyon testinin sonuçları.
- `firmware/`: derlenmiş ELF, HEX ve BIN çıktıları.

İsteğe bağlı komut satırı derlemesi: `powershell -File tools/build.ps1 -IdePath "C:\ST\STM32CubeIDE_2.2.0\STM32CubeIDE"`. İçe aktarma ve GUI derlemesi aynı proje ayarlarını kullanır.

Emülasyon testi için Python, `unicorn` ve ARM GNU araçları gerekir: `python -m pip install unicorn`; `python tests/test_firmware.py`. Test dosyasındaki araç dizinini kendi CubeIDE kurulumunuza göre düzenleyin. Bu test fiziksel donanım testi yerine geçmez.

## Kaynaklar ve lisans

- [ST NUCLEO-F446RE ürün sayfası](https://www.st.com/en/evaluation-tools/nucleo-f446re.html)
- [ST UM1724 kart kılavuzu](https://www.st.com/resource/en/user_manual/dm00105823.pdf)
- [ST UART başlangıç rehberi](https://wiki.st.com/stm32mcu/wiki/Getting_started_with_UART)
- [ST resmi STM32CubeF4 deposu](https://github.com/STMicroelectronics/STM32CubeF4)
- [ST CubeIDE 2.0 sürüm duyurusu](https://community.st.com/stm32cubeide-mcus-28/stm32cubeide-2-0-0-released-159542)

HAL/CMSIS ve ST şablon dosyalarının telif bildirimleri korunmuştur; ilgili lisanslar `Drivers` altındadır. Uygulamaya ait yeni kod MIT lisanslıdır. ST dosyaları kendi lisanslarına tabidir.
