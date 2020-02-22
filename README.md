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
  * w25q64  - флэш память ёмкостью 8MB (интерфейс SPI)
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
wsClient:
  * Qt Creator (4.11.1)
  * Qt framework (5.12.1)
```


# Пример лог-файлов:

```
* stm32_Master:

Start default task | CAN : mode=CAN_MODE_NORMAL speed=125 KHz
Chip 'W25Q64':
        Page Size:      256 bytes
        Page Count:     32768
        Sector Size:    4096 bytes
        Sector Count:   2048
        Block Size:     65536 bytes
        Block Count:    128
        Capacity:       8192 KBytes
000.00:00:10 [sCAN] Id=0x321 #1, Vcc=3.238 Time=10 TX[8]= 0A 00 00 00 03 00 EE 00
000.00:00:20 [sCAN] Id=0x321 #2, Vcc=3.245 Time=20 TX[8]= 14 00 00 00 03 00 F5 00
000.00:00:30 [sCAN] Id=0x321 #3, Vcc=3.248 Time=30 TX[8]= 1E 00 00 00 03 00 F8 00
000.00:00:40 [sCAN] Id=0x321 #4, Vcc=3.252 Time=40 TX[8]= 28 00 00 00 03 00 FC 00
000.00:00:50 [sCAN] Id=0x321 #5, Vcc=3.248 Time=50 TX[8]= 32 00 00 00 03 00 F8 00
000.00:01:00 [sCAN] Id=0x321 #6, Vcc=3.238 Time=60 TX[8]= 3C 00 00 00 03 00 EE 00
000.00:01:10 [sCAN] Id=0x321 #7, Vcc=3.251 Time=70 TX[8]= 46 00 00 00 03 00 FB 00
000.00:01:20 [sCAN] Id=0x321 #8, Vcc=3.248 Time=80 TX[8]= 50 00 00 00 03 00 F8 00
000.00:01:30 [sCAN] Id=0x321 #9, Vcc=3.238 Time=90 TX[8]= 5A 00 00 00 03 00 EE 00
...

* esp32_Slave:

App version 0.6 (17.02.2020) | MAC 30:ae:a4:05:58:bc | SDK Version v4.1-dev-2196-g64654c0 | FreeMem 277040
[VFS] Started timer with period 100 ms, time since boot: 2029061/0
[VFS] DEVICE_ID='A40558BC'
[VFS] SNTP_SERVER 'pool.ntp.org' TIME_ZONE 'EET-2'
[VFS] WIFI_MODE (1): STA
[VFS] WIFI_STA_PARAM: 'ssid:password'
[VFS] CAN_SPEED: 500 KHz
[VFS] WS_PORT: 8899
[WIFI] WIFI_MODE - STA: 'ssid':'password'
[NTP] Start sntp_client task | FreeMem 240148
[WIFI] Connected to AP 'ssid' auth(3):'AUTH_WPA2_PSK' chan:1 rssi:-40
[LOG] Start NetLogServer task (port=8008)| FreeMem 235040
[eCAN] Start can_task with mode='CAN_MODE_NORMAL'(0) speed=500 KHz | FreeMem 230116
[WS] Start WebSocket task (port=8899)| FreeMem 225564
[WIFI] Local ip_addr : 192.168.0.103
[NTP] Getting time from SNTP server 'pool.ntp.org'.
[LOG] Wait new log_client... | FreeMem 224264
[WS] Wait new web_socket client... | FreeMem 224168
[NTP] The current date/time is: Mon Feb 17 13:53:14 2020 EET-2
[NTP] Stop sntp_client task | FreeMem 224228
17.02 13:53:16 [eCAN] Id=0x321 #1 Vcc=3.266 Time=900 RX[8]= 84 03 00 00 03 00 0A 01
17.02 13:53:26 [eCAN] Id=0x321 #2 Vcc=3.254 Time=910 RX[8]= 8E 03 00 00 03 00 FE 00
17.02 13:53:36 [eCAN] Id=0x321 #3 Vcc=3.254 Time=920 RX[8]= 98 03 00 00 03 00 FE 00
17.02 13:53:41 [WS] ws_client 192.168.0.101:33196 (soc=56) online | FreeMem 222544
17.02 13:53:41 [WS] MD5 hash=816FDB6C2EB48AC0A9678F77D44D9517
17.02 13:53:41 [WS] To client : {"ts":1581940421}
17.02 13:53:46 [eCAN] Id=0x321 #4 Vcc=3.254 Time=930 RX[8]= A2 03 00 00 03 00 FE 00
17.02 13:53:50 [WS] From client : {"auth":"816FDB6C2EB48AC0A9678F77D44D9517"}
17.02 13:53:50 [WS] Access granted !
17.02 13:53:50 [WS] To client : {"DevID":"A40558BC","Time":1581940430,"FreeMem":223504}
17.02 13:53:56 [eCAN] Id=0x321 #5 Vcc=3.273 Time=940 RX[8]= AC 03 00 00 03 00 11 01
17.02 13:54:06 [eCAN] Id=0x321 #6 Vcc=3.253 Time=950 RX[8]= B6 03 00 00 03 00 FD 00
17.02 13:54:07 [WS] From client : {"get":"sntp_zone"}
17.02 13:54:07 [WS] To client : {"DevID":"A40558BC","Time":1581940447,"FreeMem":223468}
17.02 13:54:16 [eCAN] Id=0x321 #7 Vcc=3.247 Time=960 RX[8]= C0 03 00 00 03 00 F7 00
17.02 13:54:19 [WS] From client : {"get":"time_zone"}
17.02 13:54:19 [WS] To client : {"DevID":"A40558BC","Time":1581940459,"FreeMem":223468,"time_zone":"EET-2"}
17.02 13:54:26 [eCAN] Id=0x321 #8 Vcc=3.254 Time=970 RX[8]= CA 03 00 00 03 00 FE 00
17.02 13:54:31 [WS] From client : {"get":"can_speed"}
17.02 13:54:31 [WS] To client : {"DevID":"A40558BC","Time":1581940471,"FreeMem":223468,"can_speed":500}
17.02 13:54:36 [eCAN] Id=0x321 #9 Vcc=3.241 Time=980 RX[8]= D4 03 00 00 03 00 F1 00
17.02 13:54:46 [eCAN] Id=0x321 #10 Vcc=3.254 Time=990 RX[8]= DE 03 00 00 03 00 FE 00
17.02 13:54:54 [WS] Closed connection (192.168.0.101:33196 soc=56) | FreeMem 225704
17.02 13:54:56 [eCAN] Id=0x321 #11 Vcc=3.247 Time=1000 RX[8]= E8 03 00 00 03 00 F7 00
17.02 13:55:06 [eCAN] Id=0x321 #12 Vcc=3.250 Time=1010 RX[8]= F2 03 00 00 03 00 FA 00
17.02 13:55:16 [eCAN] Id=0x321 #13 Vcc=3.254 Time=1020 RX[8]= FC 03 00 00 03 00 FE 00
...
```