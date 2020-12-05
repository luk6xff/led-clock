EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 8268 11693 portrait
encoding utf-8
Sheet 1 1
Title "led-clock_sensor"
Date "2020-12-05"
Rev "0.1.0"
Comp "Luktech"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Label 2800 3700 0    10   ~ 0
VCC
$Comp
L ledclock-sensor-eagle-import:VCC #3.3V01
U 1 1 2C269B87
P 2800 3600
F 0 "#3.3V01" H 2800 3600 50  0001 C CNN
F 1 "3.3V" V 2700 3500 59  0000 L BNN
F 2 "" H 2800 3600 50  0001 C CNN
F 3 "" H 2800 3600 50  0001 C CNN
	1    2800 3600
	1    0    0    -1  
$EndComp
$Comp
L ledclock-sensor-eagle-import:R-EU_R0603 R1
U 1 1 96E7E6D7
P 1950 4100
F 0 "R1" H 1800 4159 59  0000 L BNN
F 1 "10k" H 1800 3970 59  0000 L BNN
F 2 "ledclock-sensor:R0603" H 1950 4100 50  0001 C CNN
F 3 "" H 1950 4100 50  0001 C CNN
	1    1950 4100
	0    1    1    0   
$EndComp
$Comp
L MCU_ST_STM32L0:STM32L031F6Px U1
U 1 1 5FD423CF
P 2750 4900
F 0 "U1" H 2750 4011 50  0000 C CNN
F 1 "STM32L031F6Px" H 2750 3920 50  0000 C CNN
F 2 "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" H 2350 4200 50  0001 R CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00140359.pdf" H 2750 4900 50  0001 C CNN
	1    2750 4900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Male J1
U 1 1 5FE13037
P 5000 4300
F 0 "J1" H 5108 4681 50  0000 C CNN
F 1 "Conn_01x06_Male" H 5108 4590 50  0000 C CNN
F 2 "" H 5000 4300 50  0001 C CNN
F 3 "~" H 5000 4300 50  0001 C CNN
	1    5000 4300
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2750 4200 2800 4200
Connection ~ 2800 4200
Wire Wire Line
	2800 4200 2850 4200
Wire Wire Line
	2800 4100 3450 4100
$Comp
L power:GND #PWR01
U 1 1 5FEF3823
P 2750 7800
F 0 "#PWR01" H 2750 7550 50  0001 C CNN
F 1 "GND" H 2755 7627 50  0000 C CNN
F 2 "" H 2750 7800 50  0001 C CNN
F 3 "" H 2750 7800 50  0001 C CNN
	1    2750 7800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 7800 2750 7550
Wire Wire Line
	400  6100 400  3450
Wire Wire Line
	2750 6100 2750 5700
Wire Wire Line
	400  3450 3450 3450
Wire Wire Line
	400  6100 1600 6100
Wire Wire Line
	2250 4400 2150 4400
Wire Wire Line
	1950 4400 1950 4300
Wire Wire Line
	2150 3400 2150 4400
Connection ~ 2150 4400
Wire Wire Line
	2150 4400 1950 4400
Text GLabel 5450 4600 2    50   Output ~ 0
DBG_LPUART1_TX
Connection ~ 2800 4100
Wire Wire Line
	2800 4100 2800 4200
Wire Wire Line
	3250 4600 4800 4600
Wire Wire Line
	4250 3400 4250 4500
Wire Wire Line
	4250 4500 4800 4500
Wire Wire Line
	2150 3400 4250 3400
Wire Wire Line
	4000 3450 4000 4200
Wire Wire Line
	4000 4200 4800 4200
Text GLabel 5450 4500 2    50   Input ~ 0
NRST
Text GLabel 5450 4400 2    50   Input ~ 0
SYS_SWDIO
Text GLabel 5450 4300 2    50   Input ~ 0
SYS_SWCLK
Text GLabel 5450 4200 2    50   Input ~ 0
GND
Text GLabel 5450 4100 2    50   Input ~ 0
VDD+3.3V
Wire Wire Line
	4800 4300 4350 4300
Wire Wire Line
	4350 4300 4350 5500
Wire Wire Line
	4350 5500 3250 5500
Wire Wire Line
	4800 4400 4450 4400
Wire Wire Line
	4450 4400 4450 5400
Wire Wire Line
	4450 5400 3250 5400
$Comp
L RF_Module:Ai-Thinker-Ra-01 U2
U 1 1 5FCBC531
P 4350 6850
F 0 "U2" H 4350 7831 50  0000 C CNN
F 1 "Ai-Thinker-Ra-01" H 4350 7740 50  0000 C CNN
F 2 "RF_Module:Ai-Thinker-Ra-01-LoRa" H 5350 6450 50  0001 C CNN
F 3 "https://mikroelectron.com/Product/10KM-433M-LORA-LONG-RANGE-WIRELESS-MODULE-RA-01" H 4450 7550 50  0001 C CNN
	1    4350 6850
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 6450 4850 4700
Wire Wire Line
	4850 4700 3250 4700
Wire Wire Line
	3850 6750 3600 6750
Wire Wire Line
	3600 6750 3600 4800
Wire Wire Line
	3600 4800 3250 4800
Wire Wire Line
	3850 6550 2100 6550
Wire Wire Line
	2100 6550 2100 5500
Wire Wire Line
	2100 5500 2250 5500
Wire Wire Line
	3850 6850 3550 6850
Wire Wire Line
	3550 6850 3550 4900
Wire Wire Line
	3550 4900 3250 4900
Wire Wire Line
	3850 6950 3500 6950
Wire Wire Line
	3500 6950 3500 5000
Wire Wire Line
	3500 5000 3250 5000
Wire Wire Line
	3850 7050 3450 7050
Wire Wire Line
	3450 7050 3450 5100
Wire Wire Line
	3450 5100 3250 5100
Wire Wire Line
	2750 6100 2750 7550
Connection ~ 2750 6100
Connection ~ 2750 7550
Wire Wire Line
	2750 7550 4350 7550
$Comp
L Sensor:BME280 U3
U 1 1 5FCCBB5A
P 6600 6700
F 0 "U3" H 6171 6746 50  0000 R CNN
F 1 "BME280" H 6171 6655 50  0000 R CNN
F 2 "Package_LGA:Bosch_LGA-8_2.5x2.5mm_P0.65mm_ClockwisePinNumbering" H 8100 6250 50  0001 C CNN
F 3 "https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280-DS002.pdf" H 6600 6500 50  0001 C CNN
	1    6600 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 5900 6700 6100
Text Label 6700 5900 0    10   ~ 0
VCC
$Comp
L ledclock-sensor-eagle-import:VCC #3.3V03
U 1 1 5FCD0B06
P 6700 5800
F 0 "#3.3V03" H 6700 5800 50  0001 C CNN
F 1 "3.3V" V 6600 5700 59  0000 L BNN
F 2 "" H 6700 5800 50  0001 C CNN
F 3 "" H 6700 5800 50  0001 C CNN
	1    6700 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 5850 5500 6050
Text Label 5500 5850 0    10   ~ 0
VCC
$Comp
L ledclock-sensor-eagle-import:VCC #3.3V02
U 1 1 5FCD1AE5
P 5500 5750
F 0 "#3.3V02" H 5500 5750 50  0001 C CNN
F 1 "3.3V" V 5400 5650 59  0000 L BNN
F 2 "" H 5500 5750 50  0001 C CNN
F 3 "" H 5500 5750 50  0001 C CNN
	1    5500 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 6050 4350 6050
Wire Wire Line
	6500 6100 6700 6100
Connection ~ 6700 6100
Wire Wire Line
	6700 7300 6500 7300
Wire Wire Line
	6500 7300 6500 7550
Wire Wire Line
	6500 7550 4350 7550
Connection ~ 6500 7300
Connection ~ 4350 7550
Wire Wire Line
	7200 6600 7700 6600
Wire Wire Line
	7700 6600 7700 5200
Wire Wire Line
	7700 5200 3250 5200
Wire Wire Line
	7200 7000 7500 7000
Wire Wire Line
	7500 7000 7500 5300
Wire Wire Line
	7500 5300 3250 5300
Text GLabel 7700 6100 2    50   Input ~ 0
SENSOR_SCL
Text GLabel 7500 5800 2    50   Input ~ 0
SENSOR_SDA
$Comp
L ledclock-sensor-eagle-import:R-EU_R0603 R2
U 1 1 5FCEF6A4
P 1600 4800
F 0 "R2" H 1450 4859 59  0000 L BNN
F 1 "100k" H 1450 4670 59  0000 L BNN
F 2 "ledclock-sensor:R0603" H 1600 4800 50  0001 C CNN
F 3 "" H 1600 4800 50  0001 C CNN
	1    1600 4800
	0    1    1    0   
$EndComp
Wire Wire Line
	2800 3700 2800 3900
Wire Wire Line
	1600 4600 2250 4600
Wire Wire Line
	1600 5000 1600 6100
Connection ~ 1600 6100
Wire Wire Line
	1600 6100 2750 6100
Wire Wire Line
	1950 3900 2800 3900
Connection ~ 2800 3900
Wire Wire Line
	2800 3900 2800 4100
$Comp
L Device:CP C1
U 1 1 5FD05A4C
P 3450 3800
F 0 "C1" H 3333 3754 50  0000 R CNN
F 1 "100uF" H 3333 3845 50  0000 R CNN
F 2 "" H 3488 3650 50  0001 C CNN
F 3 "~" H 3450 3800 50  0001 C CNN
	1    3450 3800
	1    0    0    1   
$EndComp
Wire Wire Line
	3450 3950 3450 4100
Connection ~ 3450 4100
Wire Wire Line
	3450 4100 4800 4100
Wire Wire Line
	3450 3650 3450 3450
Connection ~ 3450 3450
Wire Wire Line
	3450 3450 4000 3450
Text GLabel 3400 4500 2    50   Output ~ 0
SENSOR_VDD
$EndSCHEMATC
