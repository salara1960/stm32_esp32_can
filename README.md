# stm32_esp32_can
###########################################################################################
#
#  Can bus network example project : stm32_master (tx msg), esp32_slave(listen all msg)
#
###########################################################################################


## Состав рабочего оборудования:

```
stm32_Master:
  * stm32f103C8t6 (Black Pill board) - плата микроконтроллера
  * ssd1306 - OLED дисплей 0.96" 128x64 (интерфейс SPI)
  * TJA1050 - приемопередатчик интерфейса CAN.
esp32_Slave:
  * esp32 (DevKitC board) - плата микроконтроллера
  * ssd1306 - OLED дисплей 0.96" 128x64 (интерфейс I2C)
  * TJA1050 - приемопередатчик интерфейса CAN.
```


# Средства разработки:

```
stm32_Master:
  * STM32CubeMX - графический пакет для создание проектов (на языке Си) под микроконтроллеры семейства STM32
    (https://www.st.com/en/development-tools/stm32cubemx.html).
  * System Workbench for STM32 - IDE среда разработки ПО для микроконтроллеров семейства STM32
    (https://www.st.com/en/development-tools/sw4stm32.html).
  * stm32flash - утилита для программирования (прошивки) flash-ROM микроконтроллеров семейства STM32
    через встроенный порт UART1 (сырцы тут - https://sourceforge.net/projects/stm32flash/files/)
xmlParser:
  * Qt Creator (4.11.0)
  * Qt framework (5.12.1)
esp32_Slave:
  * esp-idf - SDK для разработки ПО для микроконтроллера ESP32
```


# Пример лог-файлов:

```
* stm32_Master:

        ----- Start default task (mode=CAN_MODE_LOOPBACK speed=125 KHz) -----
000.00:00:10 [sCAN] Id=0x321 #1, Vcc=3.247 Time=10 TX[8]= 03 00 F7 00 0A 00 00 00
000.00:00:20 [sCAN] Id=0x321 #2, Vcc=3.248 Time=20 TX[8]= 03 00 F8 00 14 00 00 00
000.00:00:30 [sCAN] Id=0x321 #3, Vcc=3.250 Time=30 TX[8]= 03 00 FA 00 1E 00 00 00
000.00:00:40 [sCAN] Id=0x321 #4, Vcc=3.252 Time=40 TX[8]= 03 00 FC 00 28 00 00 00
000.00:00:50 [sCAN] Id=0x321 #5, Vcc=3.237 Time=50 TX[8]= 03 00 ED 00 32 00 00 00
000.00:01:00 [sCAN] Id=0x321 #6, Vcc=3.258 Time=60 TX[8]= 03 00 02 01 3C 00 00 00
000.00:01:10 [sCAN] Id=0x321 #7, Vcc=3.252 Time=70 TX[8]= 03 00 FC 00 46 00 00 00
000.00:01:20 [sCAN] Id=0x321 #8, Vcc=3.250 Time=80 TX[8]= 03 00 FA 00 50 00 00 00
000.00:01:30 [sCAN] Id=0x321 #9, Vcc=3.269 Time=90 TX[8]= 03 00 0D 01 5A 00 00 00
...

* esp32_Slave:

App version 0.2 (14.02.2020) | MAC 30:ae:a4:05:58:bc | SDK Version v4.1-dev-2196-g64654c0 | FreeMem 277160
[VFS] Started timer with period 100 ms, time since boot: 1027923/0
[VFS] DEVICE_ID='A40558BC'
[VFS] SNTP_SERVER 'pool.ntp.org' TIME_ZONE 'EET-2'
[VFS] WIFI_MODE (1): STA
[VFS] WIFI_STA_PARAM: 'ssid:password'
[WIFI] WIFI_MODE - STA: 'ssid':'pasword'
[NTP] Start sntp_client task | FreeMem 240264
[WIFI] Connected to AP 'ssid' auth(3):'AUTH_WPA2_PSK' chan:4 rssi:-28
[LOG] Start NetLogServer task (port=8008)| FreeMem 235164
[eCAN] Start can_task with mode='CAN_MODE_NORMAL'(0) speed=125 KHz | FreeMem 230240
[WIFI] Local ip_addr : 192.168.0.103
[NTP] Getting time from SNTP server 'pool.ntp.org'.
[LOG] Wait new log_client... | FreeMem 229300
[NTP] The current date/time is: Fri Feb 14 11:52:50 2020 EET-2
[NTP] Stop sntp_client task | FreeMem 229220
14.02 11:53:01 [eCAN] Id=0x321 #1 Vcc=3.247 Time=10 RX[8]= 03 00 F7 00 0A 00 00 00
14.02 11:53:11 [eCAN] Id=0x321 #2 Vcc=3.248 Time=20 RX[8]= 03 00 F8 00 14 00 00 00
14.02 11:53:21 [eCAN] Id=0x321 #3 Vcc=3.250 Time=30 RX[8]= 03 00 FA 00 1E 00 00 00
14.02 11:53:31 [eCAN] Id=0x321 #4 Vcc=3.252 Time=40 RX[8]= 03 00 FC 00 28 00 00 00
14.02 11:53:41 [eCAN] Id=0x321 #5 Vcc=3.237 Time=50 RX[8]= 03 00 ED 00 32 00 00 00
14.02 11:53:51 [eCAN] Id=0x321 #6 Vcc=3.258 Time=60 RX[8]= 03 00 02 01 3C 00 00 00
14.02 11:54:01 [eCAN] Id=0x321 #7 Vcc=3.252 Time=70 RX[8]= 03 00 FC 00 46 00 00 00
14.02 11:54:11 [eCAN] Id=0x321 #8 Vcc=3.250 Time=80 RX[8]= 03 00 FA 00 50 00 00 00
14.02 11:54:21 [eCAN] Id=0x321 #9 Vcc=3.269 Time=90 RX[8]= 03 00 0D 01 5A 00 00 00
...
```