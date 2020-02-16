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

App version 0.5 (16.02.2020) | MAC 30:ae:a4:05:58:bc | SDK Version v4.1-dev-2196-g64654c0 | FreeMem 277040
[VFS] Started timer with period 100 ms, time since boot: 2029022/0
[VFS] DEVICE_ID='A40558BC'
[VFS] SNTP_SERVER 'pool.ntp.org' TIME_ZONE 'EET-2'
[VFS] WIFI_MODE (1): STA
[VFS] WIFI_STA_PARAM: 'ssid:password'
[VFS] CAN_SPEED: 125 KHz
[VFS] WS_PORT: 8899
[WIFI] WIFI_MODE - STA: 'ssid':'password'
[NTP] Start sntp_client task | FreeMem 240148
[WIFI] Connected to AP 'ssid' auth(3):'AUTH_WPA2_PSK' chan:1 rssi:-28
[LOG] Start NetLogServer task (port=8008)| FreeMem 235044
[eCAN] Start can_task with mode='CAN_MODE_NORMAL'(0) speed=125 KHz | FreeMem 230120
[WS] Start WebSocket task (port=8899)| FreeMem 225568
[WIFI] Local ip_addr : 192.168.0.102
[NTP] Getting time from SNTP server 'pool.ntp.org'.
[LOG] Wait new log_client... | FreeMem 224268
[WS] Wait new web_socket client... | FreeMem 224172
16.02 21:31:24 [eCAN] Id=0x321 #1 Vcc=3.238 Time=780 RX[8]= 0C 03 00 00 03 00 EE 00
[NTP] The current date/time is: Sun Feb 16 23:31:24 2020 EET-2
[NTP] Stop sntp_client task | FreeMem 223816
16.02 23:31:34 [eCAN] Id=0x321 #2 Vcc=3.260 Time=790 RX[8]= 16 03 00 00 03 00 04 01
16.02 23:31:41 [WS] ws_client 192.168.0.101:33478 (soc=56) online | FreeMem 222508
16.02 23:31:41 [WS] MD5 hash=9D392FCBD40738C9998132BDDA68E24D
16.02 23:31:41 [WS] To client : {"ts":1581888701}
16.02 23:31:44 [eCAN] Id=0x321 #3 Vcc=3.248 Time=800 RX[8]= 20 03 00 00 03 00 F8 00
16.02 23:31:51 [WS] From client : {"auth":"9D392FCBD40738C9998132BDDA68E24D"}
16.02 23:31:51 [WS] Access granted !
16.02 23:31:51 [WS] To client : {"DevID":"A40558BC","Time":1581888711,"FreeMem":223448}
16.02 23:31:54 [eCAN] Id=0x321 #4 Vcc=3.251 Time=810 RX[8]= 2A 03 00 00 03 00 FB 00
16.02 23:32:02 [WS] From client : {"get":"version"}
16.02 23:32:02 [WS] To client : {"DevID":"A40558BC","Time":1581888722,"FreeMem":223416,"version":"0.5 (16.02.2020)"}
16.02 23:32:04 [eCAN] Id=0x321 #5 Vcc=3.254 Time=820 RX[8]= 34 03 00 00 03 00 FE 00
16.02 23:32:14 [eCAN] Id=0x321 #6 Vcc=3.254 Time=830 RX[8]= 3E 03 00 00 03 00 FE 00
16.02 23:32:19 [WS] From client : {"get":"status"}
16.02 23:32:19 [WS] To client : {"DevID":"A40558BC","Time":1581888739,"FreeMem":223416}
16.02 23:32:24 [eCAN] Id=0x321 #7 Vcc=3.254 Time=840 RX[8]= 48 03 00 00 03 00 FE 00
16.02 23:32:30 [WS] From client : {"get":"can_speed"}
16.02 23:32:30 [WS] To client : {"DevID":"A40558BC","Time":1581888750,"FreeMem":223412,"can_speed":125}
16.02 23:32:34 [eCAN] Id=0x321 #8 Vcc=3.254 Time=850 RX[8]= 52 03 00 00 03 00 FE 00
16.02 23:32:39 [WS] From client : {"get":"can"}
16.02 23:32:39 [WS] To client : {"DevID":"A40558BC","Time":1581888759,"FreeMem":223420}
16.02 23:32:44 [eCAN] Id=0x321 #9 Vcc=3.254 Time=860 RX[8]= 5C 03 00 00 03 00 FE 00
16.02 23:32:53 [WS] Closed connection (192.168.0.101:33478 soc=56) | FreeMem 225644
16.02 23:32:54 [eCAN] Id=0x321 #10 Vcc=3.254 Time=870 RX[8]= 66 03 00 00 03 00 FE 00
16.02 23:33:04 [eCAN] Id=0x321 #11 Vcc=3.254 Time=880 RX[8]= 70 03 00 00 03 00 FE 00
16.02 23:33:14 [eCAN] Id=0x321 #12 Vcc=3.249 Time=890 RX[8]= 7A 03 00 00 03 00 F9 00
16.02 23:33:24 [eCAN] Id=0x321 #13 Vcc=3.253 Time=900 RX[8]= 84 03 00 00 03 00 FD 00
16.02 23:33:34 [eCAN] Id=0x321 #14 Vcc=3.254 Time=910 RX[8]= 8E 03 00 00 03 00 FE 00
...
```