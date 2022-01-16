EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 2
Title "NixieStat Thermostat Controller"
Date "2021-06-23"
Rev ""
Comp "Joe Croft"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 "$Id$"
$EndDescr
$Comp
L Diode_Bridge:DF01M D1
U 1 1 602547A5
P 4500 1250
F 0 "D1" V 4546 906 50  0000 R CNN
F 1 "DF01M" V 4700 1100 50  0000 R CNN
F 2 "Diode_THT:Diode_Bridge_DIP-4_W7.62mm_P5.08mm" H 4650 1375 50  0001 L CNN
F 3 "http://www.vishay.com/docs/88571/dfm.pdf" H 4500 1250 50  0001 C CNN
	1    4500 1250
	0    -1   -1   0   
$EndComp
$Comp
L Device:CP1 C1
U 1 1 60258417
P 5350 1250
F 0 "C1" H 5465 1296 50  0000 L CNN
F 1 "100uf 35V" H 5400 1100 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 5350 1250 50  0001 C CNN
F 3 "~" H 5350 1250 50  0001 C CNN
	1    5350 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 950  4500 900 
Wire Wire Line
	4500 900  5350 900 
Wire Wire Line
	5350 1100 5350 900 
Connection ~ 5350 900 
Wire Wire Line
	7050 1250 7200 1250
$Comp
L croftj:3.3V V7
U 1 1 6028EFE2
P 7200 650
F 0 "V7" H 7150 550 50  0001 C CNN
F 1 "3.3V" H 7200 650 50  0000 C CNN
F 2 "" H 7200 650 50  0001 C CNN
F 3 "" H 7200 650 50  0001 C CNN
	1    7200 650 
	1    0    0    -1  
$EndComp
$Comp
L power:GNDREF #PWR04
U 1 1 603CC9D1
P 5350 1550
F 0 "#PWR04" H 5350 1300 50  0001 C CNN
F 1 "GNDREF" H 5355 1377 50  0001 C CNN
F 2 "" H 5350 1550 50  0001 C CNN
F 3 "" H 5350 1550 50  0001 C CNN
	1    5350 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 1400 5350 1550
$Comp
L power:GNDREF #PWR05
U 1 1 603D065E
P 5950 1750
F 0 "#PWR05" H 5950 1500 50  0001 C CNN
F 1 "GNDREF" H 5955 1577 50  0001 C CNN
F 2 "" H 5950 1750 50  0001 C CNN
F 3 "" H 5950 1750 50  0001 C CNN
	1    5950 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 1750 5950 1600
$Comp
L power:GNDREF #PWR03
U 1 1 603F74A7
P 4500 1700
F 0 "#PWR03" H 4500 1450 50  0001 C CNN
F 1 "GNDREF" H 4505 1527 50  0001 C CNN
F 2 "" H 4500 1700 50  0001 C CNN
F 3 "" H 4500 1700 50  0001 C CNN
	1    4500 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 1700 4500 1550
$Comp
L power:GNDREF #PWR08
U 1 1 6042A550
P 7200 1950
F 0 "#PWR08" H 7200 1700 50  0001 C CNN
F 1 "GNDREF" H 7205 1777 50  0001 C CNN
F 2 "" H 7200 1950 50  0001 C CNN
F 3 "" H 7200 1950 50  0001 C CNN
	1    7200 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7200 1250 7200 1500
Connection ~ 7200 1250
$Comp
L croftj:ESP32-DevBoard2 U1
U 1 1 60AECB99
P 2150 4450
F 0 "U1" H 2200 2600 50  0000 C CNN
F 1 "ESP32-DevBoard2" H 1700 4250 50  0000 C CNN
F 2 "croftj:ESP32-DevKit" H 2150 4450 50  0001 C CNN
F 3 "" H 2150 4450 50  0001 C CNN
	1    2150 4450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4800 1250 4900 1250
$Comp
L croftj:AQH3213 RY1
U 1 1 60B3D6E9
P 1900 850
F 0 "RY1" H 1900 725 50  0000 C CNN
F 1 "AQH3213" H 1900 634 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 1900 850 50  0001 C CNN
F 3 "" H 1900 850 50  0001 C CNN
	1    1900 850 
	1    0    0    -1  
$EndComp
$Comp
L croftj:AQH3213 RY2
U 1 1 60B3FD2B
P 1900 1700
F 0 "RY2" H 1900 1575 50  0000 C CNN
F 1 "AQH3213" H 1900 1484 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 1900 1700 50  0001 C CNN
F 3 "" H 1900 1700 50  0001 C CNN
	1    1900 1700
	1    0    0    -1  
$EndComp
$Comp
L croftj:AQH3213 RY3
U 1 1 60B40C2B
P 1900 2550
F 0 "RY3" H 1900 2425 50  0000 C CNN
F 1 "AQH3213" H 1900 2334 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 1900 2550 50  0001 C CNN
F 3 "" H 1900 2550 50  0001 C CNN
	1    1900 2550
	1    0    0    -1  
$EndComp
$Comp
L croftj:AQH3213 RY4
U 1 1 60B41D2A
P 1900 3350
F 0 "RY4" H 1900 3225 50  0000 C CNN
F 1 "AQH3213" H 1900 3134 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 1900 3350 50  0001 C CNN
F 3 "" H 1900 3350 50  0001 C CNN
	1    1900 3350
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R4
U 1 1 60B6B54A
P 1250 4050
F 0 "R4" V 1363 4050 50  0000 C CNN
F 1 "R_US" V 1364 4050 50  0001 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1290 4040 50  0001 C CNN
F 3 "~" H 1250 4050 50  0001 C CNN
	1    1250 4050
	0    1    -1   0   
$EndComp
$Comp
L Device:R_US R3
U 1 1 60B6C8BB
P 1250 3250
F 0 "R3" V 1363 3250 50  0000 C CNN
F 1 "R_US" V 1364 3250 50  0001 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1290 3240 50  0001 C CNN
F 3 "~" H 1250 3250 50  0001 C CNN
	1    1250 3250
	0    1    -1   0   
$EndComp
$Comp
L Device:R_US R2
U 1 1 60B6CFBB
P 1250 2400
F 0 "R2" V 1363 2400 50  0000 C CNN
F 1 "R_US" V 1364 2400 50  0001 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1290 2390 50  0001 C CNN
F 3 "~" H 1250 2400 50  0001 C CNN
	1    1250 2400
	0    1    -1   0   
$EndComp
$Comp
L Device:R_US R1
U 1 1 60B6D73D
P 1250 1550
F 0 "R1" V 1363 1550 50  0000 C CNN
F 1 "R_US" V 1364 1550 50  0001 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1290 1540 50  0001 C CNN
F 3 "~" H 1250 1550 50  0001 C CNN
	1    1250 1550
	0    1    -1   0   
$EndComp
Text GLabel 1000 1550 0    50   Input ~ 0
HEAT
Text GLabel 1000 2400 0    50   Input ~ 0
COOL
Text GLabel 1000 3250 0    50   Input ~ 0
FAN
Text GLabel 1000 4050 0    50   Input ~ 0
VALVE
Wire Wire Line
	1000 1550 1100 1550
Wire Wire Line
	1000 2400 1100 2400
Wire Wire Line
	1000 3250 1100 3250
Wire Wire Line
	1000 4050 1100 4050
Wire Wire Line
	1400 1550 1600 1550
Wire Wire Line
	1400 4050 1600 4050
Wire Wire Line
	1400 3250 1600 3250
Wire Wire Line
	1400 2400 1600 2400
Wire Wire Line
	1600 3700 1500 3700
Wire Wire Line
	1500 3700 1500 2900
Wire Wire Line
	1500 1200 1600 1200
Wire Wire Line
	1600 2050 1500 2050
Connection ~ 1500 2050
Wire Wire Line
	1500 2050 1500 1200
Wire Wire Line
	1600 2900 1500 2900
Connection ~ 1500 2900
Wire Wire Line
	1500 2900 1500 2050
$Comp
L croftj:3.3V V1
U 1 1 60C4D2D0
P 1500 800
F 0 "V1" H 1450 700 50  0001 C CNN
F 1 "3.3V" H 1500 800 50  0000 C CNN
F 2 "" H 1500 800 50  0001 C CNN
F 3 "" H 1500 800 50  0001 C CNN
	1    1500 800 
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 950  1500 1200
Connection ~ 1500 1200
$Comp
L croftj:3.3V V2
U 1 1 60C5D7FA
P 2100 4300
F 0 "V2" H 2050 4200 50  0001 C CNN
F 1 "3.3V" H 2200 4200 50  0001 C CNN
F 2 "" H 2100 4300 50  0001 C CNN
F 3 "" H 2100 4300 50  0001 C CNN
	1    2100 4300
	1    0    0    -1  
$EndComp
Text Notes 2000 4350 0    50   ~ 0
3.3V
$Comp
L Jumper:SolderJumper_3_Bridged12 JP1
U 1 1 60BBD52A
P 2450 1550
F 0 "JP1" V 2450 1618 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 2405 1618 50  0001 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_RoundedPad1.0x1.5mm_NumberLabels" H 2450 1550 50  0001 C CNN
F 3 "~" H 2450 1550 50  0001 C CNN
	1    2450 1550
	0    1    -1   0   
$EndComp
$Comp
L Jumper:SolderJumper_3_Bridged12 JP2
U 1 1 60BC0DB1
P 2450 2400
F 0 "JP2" V 2450 2468 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 2405 2468 50  0001 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_RoundedPad1.0x1.5mm_NumberLabels" H 2450 2400 50  0001 C CNN
F 3 "~" H 2450 2400 50  0001 C CNN
	1    2450 2400
	0    1    -1   0   
$EndComp
$Comp
L Jumper:SolderJumper_3_Bridged12 JP3
U 1 1 60BC1A04
P 2450 3250
F 0 "JP3" V 2450 3318 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 2405 3318 50  0001 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_RoundedPad1.0x1.5mm_NumberLabels" H 2450 3250 50  0001 C CNN
F 3 "~" H 2450 3250 50  0001 C CNN
	1    2450 3250
	0    1    -1   0   
$EndComp
$Comp
L Jumper:SolderJumper_3_Bridged12 JP4
U 1 1 60BC1E08
P 2450 4050
F 0 "JP4" V 2450 4118 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 2405 4118 50  0001 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_RoundedPad1.0x1.5mm_NumberLabels" H 2450 4050 50  0001 C CNN
F 3 "~" H 2450 4050 50  0001 C CNN
	1    2450 4050
	0    1    -1   0   
$EndComp
Wire Wire Line
	2300 4050 2200 4050
Wire Wire Line
	2200 1550 2300 1550
Wire Wire Line
	2200 2400 2300 2400
Wire Wire Line
	2200 3250 2300 3250
Wire Wire Line
	2450 3450 2750 3450
Wire Wire Line
	2750 3450 2750 4250
Wire Wire Line
	2750 4250 2450 4250
Wire Wire Line
	2450 2600 2750 2600
Wire Wire Line
	2750 2600 2750 3450
Connection ~ 2750 3450
Wire Wire Line
	2450 1750 2750 1750
Wire Wire Line
	2750 1750 2750 2600
Connection ~ 2750 2600
$Comp
L Jumper:SolderJumper_3_Bridged12 JP5
U 1 1 60BF745C
P 3050 1200
F 0 "JP5" V 3050 1268 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 3005 1268 50  0001 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_RoundedPad1.0x1.5mm_NumberLabels" H 3050 1200 50  0001 C CNN
F 3 "~" H 3050 1200 50  0001 C CNN
	1    3050 1200
	0    1    1    0   
$EndComp
$Comp
L Jumper:SolderJumper_3_Bridged12 JP6
U 1 1 60BF8A97
P 3100 2050
F 0 "JP6" V 3100 2118 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 3055 2118 50  0001 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_RoundedPad1.0x1.5mm_NumberLabels" H 3100 2050 50  0001 C CNN
F 3 "~" H 3100 2050 50  0001 C CNN
	1    3100 2050
	0    1    1    0   
$EndComp
$Comp
L Jumper:SolderJumper_3_Bridged12 JP7
U 1 1 60BF95F1
P 3100 2900
F 0 "JP7" V 3100 2968 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 3055 2968 50  0001 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_RoundedPad1.0x1.5mm_NumberLabels" H 3100 2900 50  0001 C CNN
F 3 "~" H 3100 2900 50  0001 C CNN
	1    3100 2900
	0    1    1    0   
$EndComp
$Comp
L Jumper:SolderJumper_3_Bridged12 JP8
U 1 1 60BFA7A3
P 3100 3700
F 0 "JP8" V 3100 3768 50  0000 L CNN
F 1 "SolderJumper_3_Bridged12" V 3055 3768 50  0001 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged12_RoundedPad1.0x1.5mm_NumberLabels" H 3100 3700 50  0001 C CNN
F 3 "~" H 3100 3700 50  0001 C CNN
	1    3100 3700
	0    1    1    0   
$EndComp
Wire Wire Line
	2200 1200 2900 1200
Wire Wire Line
	2200 2050 2950 2050
Wire Wire Line
	2200 2900 2950 2900
Wire Wire Line
	2200 3700 2950 3700
Wire Wire Line
	3100 3500 3100 3400
Wire Wire Line
	3100 3400 3550 3400
Wire Wire Line
	3550 3400 3550 3000
Wire Wire Line
	3550 3000 3850 3000
Wire Wire Line
	3100 2700 3300 2700
Wire Wire Line
	3300 2700 3300 2900
Wire Wire Line
	3100 1850 3400 1850
Wire Wire Line
	3400 1850 3400 2800
Wire Wire Line
	3400 2800 4050 2800
Wire Wire Line
	3050 1000 3050 950 
Wire Wire Line
	3050 950  3500 950 
Wire Wire Line
	3500 950  3500 1450
Wire Wire Line
	3500 2700 4150 2700
Wire Wire Line
	2450 1350 2900 1350
Wire Wire Line
	2900 1350 2900 1850
Wire Wire Line
	2900 1850 3100 1850
Connection ~ 3100 1850
Wire Wire Line
	3100 2250 3100 2700
Connection ~ 3100 2700
Wire Wire Line
	2450 2200 2450 2150
Wire Wire Line
	2450 2150 2900 2150
Wire Wire Line
	2900 2150 2900 3400
Wire Wire Line
	4900 2200 3800 2200
Wire Wire Line
	3800 2200 3800 2500
Wire Wire Line
	3800 2500 4350 2500
Wire Wire Line
	4900 1250 4900 2200
Wire Wire Line
	4200 1250 3700 1250
Wire Wire Line
	3700 1250 3700 2100
Wire Wire Line
	3700 2600 4250 2600
$Comp
L Device:C C3
U 1 1 60CFDC1E
P 7200 1650
F 0 "C3" H 7315 1696 50  0000 L CNN
F 1 "10uf 35V" H 7200 1550 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 7238 1500 50  0001 C CNN
F 3 "~" H 7200 1650 50  0001 C CNN
	1    7200 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7200 1950 7200 1900
$Comp
L dk_DC-DC-Converters:V7805-1000 U5
U 1 1 60D0ADA4
P 6650 1350
F 0 "U5" H 6650 1737 60  0000 C CNN
F 1 "V7803-1000" H 6650 1631 60  0000 C CNN
F 2 "digikey-footprints:3-SIP_Module_V7805-1000" H 6850 1550 60  0001 L CNN
F 3 "https://www.cui.com/product/resource/digikeypdf/v78xx-1000.pdf" H 6850 1650 60  0001 L CNN
F 4 "102-1715-ND" H 6850 1750 60  0001 L CNN "Digi-Key_PN"
F 5 "V7805-1000" H 6850 1850 60  0001 L CNN "MPN"
F 6 "Power Supplies - Board Mount" H 6850 1950 60  0001 L CNN "Category"
F 7 "DC DC Converters" H 6850 2050 60  0001 L CNN "Family"
F 8 "https://www.cui.com/product/resource/digikeypdf/v78xx-1000.pdf" H 6850 2150 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/cui-inc/V7805-1000/102-1715-ND/1828608" H 6850 2250 60  0001 L CNN "DK_Detail_Page"
F 10 "DC DC CONVERTER 5V 5W" H 6850 2350 60  0001 L CNN "Description"
F 11 "CUI Inc." H 6850 2450 60  0001 L CNN "Manufacturer"
F 12 "Active" H 6850 2550 60  0001 L CNN "Status"
	1    6650 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 900  5950 900 
$Comp
L Device:C C2
U 1 1 60D0C1E6
P 5950 1450
F 0 "C2" H 6065 1496 50  0000 L CNN
F 1 "10uf 35V" H 5950 1350 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5988 1300 50  0001 C CNN
F 3 "~" H 5950 1450 50  0001 C CNN
	1    5950 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 1300 5950 1250
Wire Wire Line
	6250 1250 5950 1250
Connection ~ 5950 1250
Wire Wire Line
	5950 1250 5950 900 
Wire Wire Line
	6300 5750 6800 5750
Wire Wire Line
	6300 5650 6800 5650
Text GLabel 7300 5750 2    50   Input ~ 0
SEG10
Text GLabel 7300 5650 2    50   Input ~ 0
SEG9
$Comp
L power:GNDREF #PWR0111
U 1 1 6107B063
P 6650 1800
F 0 "#PWR0111" H 6650 1550 50  0001 C CNN
F 1 "GNDREF" H 6655 1627 50  0001 C CNN
F 2 "" H 6650 1800 50  0001 C CNN
F 3 "" H 6650 1800 50  0001 C CNN
	1    6650 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 1800 6650 1650
Text GLabel 8650 2050 0    50   Input ~ 0
COOL
Text GLabel 8650 1950 0    50   Input ~ 0
FAN
Text GLabel 8650 2150 0    50   Input ~ 0
HEAT
Text GLabel 8650 1850 0    50   Input ~ 0
VALVE
$Comp
L croftj:3.3V V4
U 1 1 6110DE15
P 9150 1450
F 0 "V4" H 9100 1350 50  0001 C CNN
F 1 "3.3V" H 9150 1450 50  0000 C CNN
F 2 "" H 9150 1450 50  0001 C CNN
F 3 "" H 9150 1450 50  0001 C CNN
	1    9150 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 1600 9150 1750
Wire Wire Line
	9150 1750 9700 1750
$Comp
L Connector_Generic:Conn_01x08 J2
U 1 1 61138ECA
P 9900 1850
F 0 "J2" H 9818 1317 50  0000 C CNN
F 1 "Conn_01x08" H 9818 1316 50  0001 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 9900 1850 50  0001 C CNN
F 3 "~" H 9900 1850 50  0001 C CNN
	1    9900 1850
	1    0    0    1   
$EndComp
$Comp
L power:GNDREF #PWR0112
U 1 1 611488C6
P 9550 2300
F 0 "#PWR0112" H 9550 2050 50  0001 C CNN
F 1 "GNDREF" H 9555 2127 50  0001 C CNN
F 2 "" H 9550 2300 50  0001 C CNN
F 3 "" H 9550 2300 50  0001 C CNN
	1    9550 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 1650 9550 1650
Wire Wire Line
	9550 1650 9550 2300
Text GLabel 5050 2100 2    50   Input ~ 0
AC2
Text GLabel 5050 2200 2    50   Input ~ 0
AC1
Wire Wire Line
	5050 2200 4900 2200
Connection ~ 4900 2200
Wire Wire Line
	5050 2100 3700 2100
Connection ~ 3700 2100
Wire Wire Line
	3700 2100 3700 2600
Text GLabel 9550 1450 0    50   Input ~ 0
AC2
Text GLabel 9550 1550 0    50   Input ~ 0
AC1
Wire Wire Line
	9700 1450 9550 1450
Wire Wire Line
	9700 1550 9550 1550
$Comp
L Device:R_Pack04 RN3
U 1 1 612C4504
P 7000 5650
F 0 "RN3" V 6850 5900 50  0000 C CNN
F 1 "R_Pack04" V 6674 5650 50  0001 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 7275 5650 50  0001 C CNN
F 3 "~" H 7000 5650 50  0001 C CNN
	1    7000 5650
	0    -1   1    0   
$EndComp
$Comp
L power:GNDREF #PWR0113
U 1 1 615B93CA
P 2100 7100
F 0 "#PWR0113" H 2100 6850 50  0001 C CNN
F 1 "GNDREF" H 2105 6927 50  0001 C CNN
F 2 "" H 2100 7100 50  0001 C CNN
F 3 "" H 2100 7100 50  0001 C CNN
	1    2100 7100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 6950 2100 7100
Wire Wire Line
	3700 2600 2750 2600
Connection ~ 3700 2600
Wire Wire Line
	3050 1400 3050 1450
Wire Wire Line
	3050 1450 3500 1450
Connection ~ 3500 1450
Wire Wire Line
	3500 1450 3500 2700
$Comp
L Connector_Generic:Conn_01x06 J5
U 1 1 61001ABB
P 9900 3100
F 0 "J5" H 9980 3046 50  0000 L CNN
F 1 "Conn_01x06" H 9980 3001 50  0001 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 9900 3100 50  0001 C CNN
F 3 "~" H 9900 3100 50  0001 C CNN
	1    9900 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 3000 9250 3000
Wire Wire Line
	9700 3200 9250 3200
$Comp
L power:GNDREF #PWR01
U 1 1 61062368
P 9600 3450
F 0 "#PWR01" H 9600 3200 50  0001 C CNN
F 1 "GNDREF" H 9605 3277 50  0001 C CNN
F 2 "" H 9600 3450 50  0001 C CNN
F 3 "" H 9600 3450 50  0001 C CNN
	1    9600 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	9700 3400 9600 3400
Wire Wire Line
	9600 3400 9600 3450
$Comp
L croftj:3.3V V6
U 1 1 6108B2C7
P 9550 2650
F 0 "V6" H 9500 2550 50  0001 C CNN
F 1 "3.3V" H 9550 2650 50  0000 C CNN
F 2 "" H 9550 2650 50  0001 C CNN
F 3 "" H 9550 2650 50  0001 C CNN
	1    9550 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	9550 2800 9550 2900
Wire Wire Line
	9550 2900 9700 2900
Text GLabel 9250 3000 0    50   Input ~ 0
KEY_IRQ
Text GLabel 950  5300 0    50   Input ~ 0
KEY_IRQ
Wire Wire Line
	950  5300 1200 5300
Wire Wire Line
	2900 3400 3100 3400
Connection ~ 3100 3400
Text GLabel 7300 6050 2    50   Input ~ 0
HEAT
Text GLabel 7300 6150 2    50   Input ~ 0
COOL
Text GLabel 7300 6250 2    50   Input ~ 0
FAN
Text GLabel 7300 6350 2    50   Input ~ 0
VALVE
Wire Wire Line
	7300 6050 6300 6050
Wire Wire Line
	7300 6150 6300 6150
Wire Wire Line
	7300 6250 6300 6250
Wire Wire Line
	7300 6350 6300 6350
Wire Wire Line
	9700 3100 9250 3100
Text GLabel 9250 3100 0    50   Input ~ 0
I2C_SCL
Text GLabel 9250 3200 0    50   Input ~ 0
I2C_SDA
Text GLabel 3550 4950 2    50   Input ~ 0
I2C_SCL
Text GLabel 3550 5050 2    50   Input ~ 0
I2C_SDA
Wire Wire Line
	2950 4950 3150 4950
Wire Wire Line
	3550 5050 3400 5050
Wire Wire Line
	9700 3300 9600 3300
Wire Wire Line
	9600 3300 9600 3400
Connection ~ 9600 3400
Wire Wire Line
	5600 5750 5500 5750
Wire Wire Line
	5600 5650 5350 5650
Text GLabel 5350 5650 0    50   Input ~ 0
I2C_SCL
Text GLabel 5350 5750 0    50   Input ~ 0
I2C_SDA
$Comp
L croftj:MAX7320 U2
U 1 1 615402A9
P 5800 6150
F 0 "U2" H 5950 6200 50  0000 C CNN
F 1 "MAX7320" H 5650 5800 50  0000 C CNN
F 2 "Package_SO:TSSOP-16_4.4x5mm_P0.65mm" H 5950 6150 50  0001 C CNN
F 3 "http://www.nxp.com/documents/data_sheet/PCF8574_PCF8574A.pdf" H 5950 6150 50  0001 C CNN
	1    5800 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 5900 5500 5900
$Comp
L power:GNDREF #PWR0120
U 1 1 61758C9E
P 5950 6700
F 0 "#PWR0120" H 5950 6450 50  0001 C CNN
F 1 "GNDREF" H 5955 6527 50  0001 C CNN
F 2 "" H 5950 6700 50  0001 C CNN
F 3 "" H 5950 6700 50  0001 C CNN
	1    5950 6700
	1    0    0    -1  
$EndComp
$Comp
L croftj:3.3V V10
U 1 1 61786201
P 5950 5250
F 0 "V10" H 5900 5150 50  0001 C CNN
F 1 "3.3V" H 5950 5250 50  0000 C CNN
F 2 "" H 5950 5250 50  0001 C CNN
F 3 "" H 5950 5250 50  0001 C CNN
	1    5950 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 5450 5950 5400
Wire Wire Line
	8650 1850 9700 1850
Wire Wire Line
	8650 1950 9700 1950
Wire Wire Line
	8650 2050 9700 2050
Wire Wire Line
	8650 2150 9700 2150
$Comp
L Connector:TestPoint TP1
U 1 1 61278EC3
P 7700 800
F 0 "TP1" H 7758 918 50  0000 L CNN
F 1 "Gnd" H 7758 827 50  0000 L CNN
F 2 "Connector_Pin:Pin_D1.3mm_L11.0mm" H 7900 800 50  0001 C CNN
F 3 "~" H 7900 800 50  0001 C CNN
	1    7700 800 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 800  7700 1900
Wire Wire Line
	7700 1900 7200 1900
Connection ~ 7200 1900
Wire Wire Line
	7200 1900 7200 1800
Text GLabel 7300 5900 2    50   Input ~ 0
~HV_EN~
Wire Wire Line
	7200 5650 7300 5650
Wire Wire Line
	7200 5750 7300 5750
Wire Wire Line
	6300 5850 6750 5850
Wire Wire Line
	6750 5900 6750 5850
Wire Wire Line
	6750 5900 7300 5900
Wire Wire Line
	5600 6000 5500 6000
Wire Wire Line
	5500 6000 5500 5900
Connection ~ 5500 5750
Wire Wire Line
	5500 5750 5350 5750
Connection ~ 5500 5900
Wire Wire Line
	5500 5900 5500 5750
$Comp
L croftj:3.3V V12
U 1 1 61AAFBEB
P 3700 6950
F 0 "V12" H 3650 6850 50  0001 C CNN
F 1 "3.3V" H 3800 6850 50  0001 C CNN
F 2 "" H 3700 6950 50  0001 C CNN
F 3 "" H 3700 6950 50  0001 C CNN
	1    3700 6950
	1    0    0    -1  
$EndComp
Text Notes 3600 7000 0    50   ~ 0
3.3V
Wire Wire Line
	3700 7100 3700 7150
$Comp
L croftj:3.3V V5
U 1 1 61ADC028
P 5250 6050
F 0 "V5" H 5200 5950 50  0001 C CNN
F 1 "3.3V" H 5350 5950 50  0001 C CNN
F 2 "" H 5250 6050 50  0001 C CNN
F 3 "" H 5250 6050 50  0001 C CNN
	1    5250 6050
	1    0    0    -1  
$EndComp
Text Notes 5150 6100 0    50   ~ 0
3.3V
Wire Wire Line
	5250 6200 5250 6250
Wire Wire Line
	5250 6250 5600 6250
$Comp
L Connector:TestPoint TP2
U 1 1 61C8B90A
P 3150 4600
F 0 "TP2" H 3208 4718 50  0000 L CNN
F 1 "SCL" H 3208 4627 50  0000 L CNN
F 2 "Connector_Pin:Pin_D1.3mm_L11.0mm" H 3350 4600 50  0001 C CNN
F 3 "~" H 3350 4600 50  0001 C CNN
	1    3150 4600
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint TP3
U 1 1 61CA27E5
P 3400 4600
F 0 "TP3" H 3458 4718 50  0000 L CNN
F 1 "SDA" H 3458 4627 50  0000 L CNN
F 2 "Connector_Pin:Pin_D1.3mm_L11.0mm" H 3600 4600 50  0001 C CNN
F 3 "~" H 3600 4600 50  0001 C CNN
	1    3400 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 4600 3400 5050
Wire Wire Line
	3400 5050 2950 5050
Wire Wire Line
	3150 4600 3150 4950
Wire Wire Line
	3150 4950 3550 4950
Wire Wire Line
	2100 4450 2100 4600
Connection ~ 3150 4950
Connection ~ 3400 5050
$Comp
L Device:C C9
U 1 1 61E42E4D
P 3700 7400
F 0 "C9" H 3815 7446 50  0000 L CNN
F 1 ".1uf" H 3815 7355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 3738 7250 50  0001 C CNN
F 3 "~" H 3700 7400 50  0001 C CNN
	1    3700 7400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 7150 3700 7250
$Comp
L Device:C C8
U 1 1 61E5AA93
P 3300 7400
F 0 "C8" H 3415 7446 50  0000 L CNN
F 1 ".1uf" H 3415 7355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 3338 7250 50  0001 C CNN
F 3 "~" H 3300 7400 50  0001 C CNN
	1    3300 7400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3300 7150 3300 7250
$Comp
L Device:C C7
U 1 1 61E7273C
P 2900 7400
F 0 "C7" H 3015 7446 50  0000 L CNN
F 1 ".1uf" H 3015 7355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 2938 7250 50  0001 C CNN
F 3 "~" H 2900 7400 50  0001 C CNN
	1    2900 7400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 7150 2900 7250
Wire Wire Line
	3700 7150 3300 7150
Connection ~ 3700 7150
Connection ~ 3300 7150
Wire Wire Line
	3300 7150 2900 7150
Wire Wire Line
	2900 7550 3300 7550
Connection ~ 3300 7550
Wire Wire Line
	3300 7550 3700 7550
Connection ~ 3700 7550
Wire Wire Line
	3700 7550 3700 7600
$Comp
L power:GNDREF #PWR02
U 1 1 61DE16AB
P 3700 7600
F 0 "#PWR02" H 3700 7350 50  0001 C CNN
F 1 "GNDREF" H 3705 7427 50  0001 C CNN
F 2 "" H 3700 7600 50  0001 C CNN
F 3 "" H 3700 7600 50  0001 C CNN
	1    3700 7600
	1    0    0    -1  
$EndComp
Wire Wire Line
	7200 800  7200 1250
$Comp
L Connector_Generic:Conn_01x07 J1
U 1 1 621320A0
P 4750 2800
F 0 "J1" H 4830 2796 50  0000 L CNN
F 1 "Conn_01x07" H 4830 2751 50  0001 L CNN
F 2 "croftj:conn-phx-1703090" H 4750 2800 50  0001 C CNN
F 3 "~" H 4750 2800 50  0001 C CNN
	1    4750 2800
	1    0    0    -1  
$EndComp
$Comp
L power:GNDREF #PWR0110
U 1 1 6213597E
P 3750 3950
F 0 "#PWR0110" H 3750 3700 50  0001 C CNN
F 1 "GNDREF" H 3755 3777 50  0001 C CNN
F 2 "" H 3750 3950 50  0001 C CNN
F 3 "" H 3750 3950 50  0001 C CNN
	1    3750 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 3100 3750 3100
Wire Wire Line
	3750 3100 3750 3850
$Comp
L Connector_Generic:Conn_01x07 J6
U 1 1 621EABE5
P 4600 3550
F 0 "J6" H 4680 3546 50  0000 L CNN
F 1 "Conn_01x07" H 4680 3501 50  0001 L CNN
F 2 "croftj:Mill-Max-295x7" H 4600 3550 50  0001 C CNN
F 3 "~" H 4600 3550 50  0001 C CNN
	1    4600 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4350 2500 4350 3250
Wire Wire Line
	4350 3250 4400 3250
Connection ~ 4350 2500
Wire Wire Line
	4350 2500 4550 2500
Wire Wire Line
	4250 2600 4250 3350
Wire Wire Line
	4250 3350 4400 3350
Connection ~ 4250 2600
Wire Wire Line
	4250 2600 4550 2600
Wire Wire Line
	4150 2700 4150 3450
Wire Wire Line
	4150 3450 4400 3450
Connection ~ 4150 2700
Wire Wire Line
	4150 2700 4550 2700
Wire Wire Line
	3300 2900 3950 2900
Wire Wire Line
	4050 2800 4050 3550
Wire Wire Line
	4050 3550 4400 3550
Connection ~ 4050 2800
Wire Wire Line
	4050 2800 4550 2800
Wire Wire Line
	3950 2900 3950 3650
Wire Wire Line
	3950 3650 4400 3650
Connection ~ 3950 2900
Wire Wire Line
	3950 2900 4550 2900
Wire Wire Line
	3850 3000 3850 3750
Wire Wire Line
	3850 3750 4400 3750
Connection ~ 3850 3000
Wire Wire Line
	3850 3000 4550 3000
Wire Wire Line
	4400 3850 3750 3850
Connection ~ 3750 3850
Wire Wire Line
	3750 3850 3750 3950
$Sheet
S 10050 4900 750  950 
U 62313FED
F0 "IODrivers" 50
F1 "IODrivers.sch" 50
$EndSheet
Wire Wire Line
	5950 6550 5950 6700
Wire Wire Line
	6500 4600 6500 4700
Wire Wire Line
	5800 4300 6150 4300
Wire Wire Line
	5800 4250 5800 4300
Text Notes 5700 4150 0    50   ~ 0
3.3V
$Comp
L croftj:3.3V V9
U 1 1 61AF2737
P 5800 4100
F 0 "V9" H 5750 4000 50  0001 C CNN
F 1 "3.3V" H 5900 4000 50  0001 C CNN
F 2 "" H 5800 4100 50  0001 C CNN
F 3 "" H 5800 4100 50  0001 C CNN
	1    5800 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 3800 5700 3800
Wire Wire Line
	5950 4050 5950 3800
Wire Wire Line
	6150 4050 5950 4050
Wire Wire Line
	6050 3700 5700 3700
Wire Wire Line
	6050 3950 6050 3700
Wire Wire Line
	6500 3400 6500 3500
$Comp
L power:GNDREF #PWR0119
U 1 1 616E6067
P 6500 4700
F 0 "#PWR0119" H 6500 4450 50  0001 C CNN
F 1 "GNDREF" H 6505 4527 50  0001 C CNN
F 2 "" H 6500 4700 50  0001 C CNN
F 3 "" H 6500 4700 50  0001 C CNN
	1    6500 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 3950 6050 3950
$Comp
L croftj:MAX7320 U3
U 1 1 6153E074
P 6350 4200
F 0 "U3" H 6500 4300 50  0000 C CNN
F 1 "MAX7320" H 6750 3850 50  0000 C CNN
F 2 "Package_SO:TSSOP-16_4.4x5mm_P0.65mm" H 6500 4200 50  0001 C CNN
F 3 "http://www.nxp.com/documents/data_sheet/PCF8574_PCF8574A.pdf" H 6500 4200 50  0001 C CNN
	1    6350 4200
	1    0    0    -1  
$EndComp
Text GLabel 5700 3800 0    50   Input ~ 0
I2C_SDA
Text GLabel 5700 3700 0    50   Input ~ 0
I2C_SCL
Connection ~ 6050 3700
Wire Wire Line
	6150 3700 6050 3700
Connection ~ 5950 3800
Wire Wire Line
	6150 3800 5950 3800
$Comp
L Device:R_Pack04 RN1
U 1 1 612C4510
P 7400 3900
F 0 "RN1" V 7150 4100 50  0000 C CNN
F 1 "R_Pack04" V 7074 3900 50  0001 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 7675 3900 50  0001 C CNN
F 3 "~" H 7400 3900 50  0001 C CNN
	1    7400 3900
	0    -1   1    0   
$EndComp
$Comp
L Device:R_Pack04 RN2
U 1 1 612C450A
P 7400 4300
F 0 "RN2" V 7250 4550 50  0000 C CNN
F 1 "R_Pack04" V 7074 4300 50  0001 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 7675 4300 50  0001 C CNN
F 3 "~" H 7400 4300 50  0001 C CNN
	1    7400 4300
	0    -1   1    0   
$EndComp
$Comp
L croftj:3.3V V3
U 1 1 60C5C5A3
P 6500 3250
F 0 "V3" H 6450 3150 50  0001 C CNN
F 1 "3.3V" H 6500 3250 50  0000 C CNN
F 2 "" H 6500 3250 50  0001 C CNN
F 3 "" H 6500 3250 50  0001 C CNN
	1    6500 3250
	1    0    0    -1  
$EndComp
Text GLabel 7900 4000 2    50   Input ~ 0
SEG4
Text GLabel 7900 4200 2    50   Input ~ 0
SEG6
Text GLabel 7900 4100 2    50   Input ~ 0
SEG5
Text GLabel 7900 3700 2    50   Input ~ 0
SEG1
Text GLabel 7900 3900 2    50   Input ~ 0
SEG3
Text GLabel 7900 3800 2    50   Input ~ 0
SEG2
Text GLabel 7900 4300 2    50   Input ~ 0
SEG7
Text GLabel 7900 4400 2    50   Input ~ 0
SEG8
Wire Wire Line
	6850 3700 7200 3700
Wire Wire Line
	6850 3800 7200 3800
Wire Wire Line
	6850 3900 7200 3900
Wire Wire Line
	6850 4000 7200 4000
Wire Wire Line
	6850 4100 7200 4100
Wire Wire Line
	6850 4200 7200 4200
Wire Wire Line
	6850 4300 7200 4300
Wire Wire Line
	6850 4400 7200 4400
Wire Wire Line
	7600 4400 7900 4400
Wire Wire Line
	7600 4300 7900 4300
Wire Wire Line
	7600 4200 7900 4200
Wire Wire Line
	7600 4100 7900 4100
Wire Wire Line
	7600 4000 7900 4000
Wire Wire Line
	7600 3900 7900 3900
Wire Wire Line
	7600 3800 7900 3800
Wire Wire Line
	7600 3700 7900 3700
$Comp
L croftj:VCC V?
U 1 1 61E47E6F
P 5350 650
F 0 "V?" H 5250 550 50  0001 C CNN
F 1 "VCC" H 5300 650 50  0000 L CNN
F 2 "" H 5200 700 50  0001 C CNN
F 3 "" H 5200 700 50  0001 C CNN
	1    5350 650 
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 800  5350 900 
$EndSCHEMATC
