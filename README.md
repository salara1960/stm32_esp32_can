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

        ----- Start default task (mode=CAN_MODE_NORMAL speed=125 KHz) -----
000.00:00:10 [sCAN] Id=0x321 #1, Vcc=3.260 Time=10 TX[8]= 0A 00 00 00 03 00 04 01
000.00:00:20 [sCAN] Id=0x321 #2, Vcc=3.252 Time=20 TX[8]= 14 00 00 00 03 00 FC 00
000.00:00:30 [sCAN] Id=0x321 #3, Vcc=3.254 Time=30 TX[8]= 1E 00 00 00 03 00 FE 00
000.00:00:40 [sCAN] Id=0x321 #4, Vcc=3.255 Time=40 TX[8]= 28 00 00 00 03 00 FF 00
000.00:00:50 [sCAN] Id=0x321 #5, Vcc=3.254 Time=50 TX[8]= 32 00 00 00 03 00 FE 00
000.00:01:00 [sCAN] Id=0x321 #6, Vcc=3.252 Time=60 TX[8]= 3C 00 00 00 03 00 FC 00
000.00:01:10 [sCAN] Id=0x321 #7, Vcc=3.254 Time=70 TX[8]= 46 00 00 00 03 00 FE 00
000.00:01:20 [sCAN] Id=0x321 #8, Vcc=3.247 Time=80 TX[8]= 50 00 00 00 03 00 F7 00
000.00:01:30 [sCAN] Id=0x321 #9, Vcc=3.238 Time=90 TX[8]= 5A 00 00 00 03 00 EE 00
...

* esp32_Slave:

App version 0.4 (15.02.2020) | MAC 30:ae:a4:05:58:bc | SDK Version v4.1-dev-2196-g64654c0 | Free8
[VFS] Started timer with period 100 ms, time since boot: 2029039/0
[VFS] DEVICE_ID='A40558BC'
[VFS] SNTP_SERVER 'pool.ntp.org' TIME_ZONE 'EET-2'
[VFS] WIFI_MODE (1): STA
[VFS] WIFI_STA_PARAM: 'ssid:password'
[VFS] CAN_SPEED: 125 KHz
[VFS] WS_PORT: 8899
[WIFI] WIFI_MODE - STA: 'ssid':'password'
[NTP] Start sntp_client task | FreeMem 240404
[LOG] Start NetLogServer task (port=8008)| FreeMem 235832
[eCAN] Start can_task with mode='CAN_MODE_NORMAL'(0) speed=125 KHz | FreeMem 230908
[WS] Start WebSocket task (port=8899)| FreeMem 226096
[WIFI] Connected to AP 'ssid' auth(3):'AUTH_WPA2_PSK' chan:11 rssi:-37
[WIFI] Local ip_addr : 192.168.0.103
[NTP] Getting time from SNTP server 'pool.ntp.org'.
[LOG] Wait new log_client... | FreeMem 224528
[WS] Wait new web_socket client... | FreeMem 224256
01.01 00:00:08 [eCAN] Id=0x321 #1 Vcc=3.260 Time=28750 RX[8]= 4E 70 00 00 03 00 04 01
[NTP] The current date/time is: Sat Feb 15 20:35:43 2020 EET-2
[NTP] Stop sntp_client task | FreeMem 223908
15.02 20:35:50 [eCAN] Id=0x321 #2 Vcc=3.254 Time=28760 RX[8]= 58 70 00 00 03 00 FE 00
15.02 20:35:55 [WS] ws_client 192.168.0.101:41912 (soc=56) online | FreeMem 223408
15.02 20:35:55 [WS] MD5 hash=65E02DAE2F69868BFE92C42277C53641
15.02 20:35:55 [WS] To client : {"ts":1581791755}
15.02 20:36:00 [eCAN] Id=0x321 #3 Vcc=3.254 Time=28770 RX[8]= 62 70 00 00 03 00 FE 00
15.02 20:36:07 [WS] Access granted !
15.02 20:36:07 [WS] From client : {"auth":"65E02DAE2F69868BFE92C42277C53641"}
15.02 20:36:07 [WS] To client : {"status":0}
15.02 20:36:10 [eCAN] Id=0x321 #4 Vcc=3.248 Time=28780 RX[8]= 6C 70 00 00 03 00 F8 00
15.02 20:36:20 [eCAN] Id=0x321 #5 Vcc=3.250 Time=28790 RX[8]= 76 70 00 00 03 00 FA 00
15.02 20:36:30 [eCAN] Id=0x321 #6 Vcc=3.254 Time=28800 RX[8]= 80 70 00 00 03 00 FE 00
15.02 20:36:40 [eCAN] Id=0x321 #7 Vcc=3.248 Time=28810 RX[8]= 8A 70 00 00 03 00 F8 00
15.02 20:36:50 [eCAN] Id=0x321 #8 Vcc=3.254 Time=28820 RX[8]= 94 70 00 00 03 00 FE 00
15.02 20:37:00 [eCAN] Id=0x321 #9 Vcc=3.241 Time=28830 RX[8]= 9E 70 00 00 03 00 F1 00
...
```