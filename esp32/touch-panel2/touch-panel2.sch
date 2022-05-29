EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date "2022-05-29"
Rev ""
Comp "Joe Croft"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 "$Id$"
$EndDescr
$Comp
L Connector_Generic:Conn_01x05 J1
U 1 1 62914214
P 3250 4200
F 0 "J1" H 3168 3867 50  0000 C CNN
F 1 "Conn_01x05" H 3168 3866 50  0001 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" H 3250 4200 50  0001 C CNN
F 3 "~" H 3250 4200 50  0001 C CNN
	1    3250 4200
	-1   0    0    1   
$EndComp
$Comp
L power:GNDREF #PWR0101
U 1 1 62914E42
P 5100 5250
F 0 "#PWR0101" H 5100 5000 50  0001 C CNN
F 1 "GNDREF" H 5105 5077 50  0001 C CNN
F 2 "" H 5100 5250 50  0001 C CNN
F 3 "" H 5100 5250 50  0001 C CNN
	1    5100 5250
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C1
U 1 1 629157EC
P 3900 3450
F 0 "C1" H 3992 3450 50  0000 L CNN
F 1 "C_Small" H 3992 3405 50  0001 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 3900 3450 50  0001 C CNN
F 3 "~" H 3900 3450 50  0001 C CNN
	1    3900 3450
	1    0    0    -1  
$EndComp
$Comp
L croftj:VCC V1
U 1 1 62915D08
P 3600 3100
F 0 "V1" H 3500 3000 50  0001 C CNN
F 1 "VCC" H 3550 3100 50  0000 L CNN
F 2 "" H 3450 3150 50  0001 C CNN
F 3 "" H 3450 3150 50  0001 C CNN
	1    3600 3100
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small_US R1
U 1 1 6291643F
P 4250 3450
F 0 "R1" H 4318 3450 50  0000 L CNN
F 1 "R_Small_US" H 4318 3405 50  0001 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" H 4250 3450 50  0001 C CNN
F 3 "~" H 4250 3450 50  0001 C CNN
	1    4250 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Pack04 RN1
U 1 1 62916A23
P 6050 4150
F 0 "RN1" V 5725 4150 50  0000 C CNN
F 1 "R_Pack04" V 5724 4150 50  0001 C CNN
F 2 "Resistor_SMD:R_Array_Convex_4x0603" V 6325 4150 50  0001 C CNN
F 3 "~" H 6050 4150 50  0001 C CNN
	1    6050 4150
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint TP2
U 1 1 6291AE50
P 6350 4150
F 0 "TP2" V 6350 4338 50  0000 L CNN
F 1 "TestPoint" V 6395 4338 50  0001 L CNN
F 2 "croftj:Touch_Pad_Small" H 6550 4150 50  0001 C CNN
F 3 "~" H 6550 4150 50  0001 C CNN
	1    6350 4150
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint TP4
U 1 1 6291AF74
P 6350 4250
F 0 "TP4" V 6350 4438 50  0000 L CNN
F 1 "TestPoint" V 6395 4438 50  0001 L CNN
F 2 "croftj:Touch_Pad_Small" H 6550 4250 50  0001 C CNN
F 3 "~" H 6550 4250 50  0001 C CNN
	1    6350 4250
	0    1    1    0   
$EndComp
Wire Wire Line
	3450 4000 3600 4000
Wire Wire Line
	3600 4000 3600 3300
Wire Wire Line
	3600 3300 3900 3300
Wire Wire Line
	3900 3300 3900 3350
Connection ~ 3600 3300
Wire Wire Line
	3600 3300 3600 3250
Wire Wire Line
	3900 3300 4250 3300
Wire Wire Line
	4250 3300 4250 3350
Connection ~ 3900 3300
$Comp
L power:GNDREF #PWR0102
U 1 1 62920A33
P 3550 4500
F 0 "#PWR0102" H 3550 4250 50  0001 C CNN
F 1 "GNDREF" H 3555 4327 50  0001 C CNN
F 2 "" H 3550 4500 50  0001 C CNN
F 3 "" H 3550 4500 50  0001 C CNN
	1    3550 4500
	1    0    0    -1  
$EndComp
$Comp
L power:GNDREF #PWR0103
U 1 1 62920CAC
P 4100 4500
F 0 "#PWR0103" H 4100 4250 50  0001 C CNN
F 1 "GNDREF" H 4105 4327 50  0001 C CNN
F 2 "" H 4100 4500 50  0001 C CNN
F 3 "" H 4100 4500 50  0001 C CNN
	1    4100 4500
	1    0    0    -1  
$EndComp
$Comp
L power:GNDREF #PWR0104
U 1 1 62920FA9
P 3900 3650
F 0 "#PWR0104" H 3900 3400 50  0001 C CNN
F 1 "GNDREF" H 3905 3477 50  0001 C CNN
F 2 "" H 3900 3650 50  0001 C CNN
F 3 "" H 3900 3650 50  0001 C CNN
	1    3900 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 3550 4250 3900
Wire Wire Line
	4250 3900 4400 3900
Wire Wire Line
	3900 3550 3900 3650
Wire Wire Line
	3450 4100 4400 4100
Wire Wire Line
	3450 4200 4400 4200
Wire Wire Line
	3450 4300 4400 4300
Wire Wire Line
	3450 4400 3550 4400
Wire Wire Line
	3550 4400 3550 4500
Wire Wire Line
	5700 3950 5850 3950
Wire Wire Line
	5700 4050 5850 4050
Wire Wire Line
	5700 4150 5850 4150
Wire Wire Line
	5700 4250 5850 4250
Wire Wire Line
	6250 4050 6350 4050
Wire Wire Line
	6250 3950 6350 3950
Wire Wire Line
	6250 4250 6350 4250
Wire Wire Line
	6250 4150 6350 4150
Wire Wire Line
	4250 3300 5050 3300
Wire Wire Line
	5050 3300 5050 3700
Connection ~ 4250 3300
Wire Wire Line
	4400 4400 4100 4400
Wire Wire Line
	4100 4400 4100 4500
Wire Wire Line
	5100 5150 5100 5250
$Comp
L croftj:AT42Q2120 U1
U 1 1 6295C660
P 6050 3850
F 0 "U1" H 5200 2600 50  0000 C CNN
F 1 "AT42Q2120" H 5300 3950 50  0000 C CNN
F 2 "Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm" H 6000 3850 50  0001 C CNN
F 3 "" H 6000 3850 50  0001 C CNN
	1    6050 3850
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint TP3
U 1 1 6291AF12
P 6350 3950
F 0 "TP3" V 6350 4138 50  0000 L CNN
F 1 "TestPoint" V 6395 4138 50  0001 L CNN
F 2 "croftj:Touch_Pad_Small" H 6550 3950 50  0001 C CNN
F 3 "~" H 6550 3950 50  0001 C CNN
	1    6350 3950
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint TP1
U 1 1 6291753F
P 6350 4050
F 0 "TP1" V 6350 4238 50  0000 L CNN
F 1 "TestPoint" V 6395 4238 50  0001 L CNN
F 2 "croftj:Touch_Pad_Small" H 6550 4050 50  0001 C CNN
F 3 "~" H 6550 4050 50  0001 C CNN
	1    6350 4050
	0    1    1    0   
$EndComp
$EndSCHEMATC