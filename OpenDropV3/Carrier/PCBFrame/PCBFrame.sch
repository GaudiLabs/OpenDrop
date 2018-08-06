EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:GaudiLabsPartsLibrary
LIBS:PCBFrame-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MountPad3 X1
U 1 1 5713D45E
P 2900 3000
F 0 "X1" H 2929 3080 60  0000 L CNN
F 1 "MountPad3" H 2929 2974 60  0000 L CNN
F 2 "GaudiLabsFootPrints:MountingHoleRound3mm" H 2900 3000 60  0001 C CNN
F 3 "" H 2900 3000 60  0000 C CNN
	1    2900 3000
	1    0    0    -1  
$EndComp
$Comp
L MountPad3 X2
U 1 1 5713D4E6
P 2900 3350
F 0 "X2" H 2929 3430 60  0000 L CNN
F 1 "MountPad3" H 2929 3324 60  0000 L CNN
F 2 "GaudiLabsFootPrints:MountingHoleRound3mm" H 2900 3350 60  0001 C CNN
F 3 "" H 2900 3350 60  0000 C CNN
	1    2900 3350
	1    0    0    -1  
$EndComp
$Comp
L MountPad3 X3
U 1 1 5713D560
P 2900 3800
F 0 "X3" H 2929 3880 60  0000 L CNN
F 1 "MountPad3" H 2929 3774 60  0000 L CNN
F 2 "GaudiLabsFootPrints:MountingHoleRound3mm" H 2900 3800 60  0001 C CNN
F 3 "" H 2900 3800 60  0000 C CNN
	1    2900 3800
	1    0    0    -1  
$EndComp
$Comp
L MountPad3 X4
U 1 1 5713D593
P 2900 4200
F 0 "X4" H 2929 4280 60  0000 L CNN
F 1 "MountPad3" H 2929 4174 60  0000 L CNN
F 2 "GaudiLabsFootPrints:MountingHoleRound3mm" H 2900 4200 60  0001 C CNN
F 3 "" H 2900 4200 60  0000 C CNN
	1    2900 4200
	1    0    0    -1  
$EndComp
$Comp
L GNDREF #PWR1
U 1 1 5713D8C2
P 1900 4350
F 0 "#PWR1" H 1900 4100 50  0001 C CNN
F 1 "GNDREF" H 1908 4177 50  0000 C CNN
F 2 "" H 1900 4350 50  0000 C CNN
F 3 "" H 1900 4350 50  0000 C CNN
	1    1900 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 4200 2650 4200
Wire Wire Line
	2650 3800 2700 3800
Connection ~ 2650 4200
Wire Wire Line
	2650 3350 2700 3350
Connection ~ 2650 3800
Wire Wire Line
	1900 3000 2700 3000
$Comp
L GND #PWR2
U 1 1 5714C697
P 2650 4400
F 0 "#PWR2" H 2650 4150 50  0001 C CNN
F 1 "GND" H 2658 4227 50  0000 C CNN
F 2 "" H 2650 4400 50  0000 C CNN
F 3 "" H 2650 4400 50  0000 C CNN
	1    2650 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 3000 1900 4350
Wire Wire Line
	2650 3350 2650 4400
$Comp
L MountPad3 X5
U 1 1 5714D097
P 2150 2500
F 0 "X5" V 1991 2593 60  0000 L CNN
F 1 "MountPad3" V 2097 2593 60  0000 L CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 2150 2500 60  0001 C CNN
F 3 "" H 2150 2500 60  0000 C CNN
	1    2150 2500
	0    -1   -1   0   
$EndComp
$Comp
L MountPad3 X6
U 1 1 5714D121
P 2350 2450
F 0 "X6" V 2191 2543 60  0000 L CNN
F 1 "MountPad3" V 2297 2543 60  0000 L CNN
F 2 "Measurement_Points:Measurement_Point_Square-SMD-Pad_Big" H 2350 2450 60  0001 C CNN
F 3 "" H 2350 2450 60  0000 C CNN
	1    2350 2450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2150 2700 2150 3000
Connection ~ 2150 3000
Wire Wire Line
	2350 2650 2350 3000
Connection ~ 2350 3000
$EndSCHEMATC
