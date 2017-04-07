

#define FluxlPad_width 16
#define FluxlPad_heigth 8


// If using software SPI (the default case):
#define OLED_MOSI   2
#define OLED_CLK   3
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13



#define BL_pin         9           // BL Pin
#define LE_pin          4           // LE Pin
#define CLK_pin         15           // CLK Pin
#define DI_pin         16           // Digital Out Pin
#define POL_pin         A1           // Digital Out Pin


#define SW1_pin         8           // Left button
#define SW2_pin         A5           // Right button
#define SW3_pin         7           // Joy button
#define JOY_pin         A4           // JOY_pin analog 
#define FEEDBACK_pin    A2           // FEEDBACK pin



#define ESP_RESET_pin   A3           // Buton 1 pin
#define ESP_GPIO0_pin   10           // Buton 1 pin
#define ESP_RX_pin      1           // Buton 1 pin
#define ESP_TX_pin      0           // Buton 1 pin

#define LED_LIGHT_pin         5           // Digital  Pin for LEDs
#define SPK_pin       6           // Digital Pin for speaker

#define sound_flag true           // Make Sound
#define light_flag false           // Make Light

#define feedback_flag false           // Make Light

const byte FluxelID [16][8] PROGMEM ={
{3,2,1,0,63,62,61,60},
{6,7,5,4,59,58,56,57},
{10,9,11,8,55,52,54,53},
{13,14,15,12,51,48,49,50},
{17,18,19,16,47,44,45,46},
{20,21,22,23,40,41,42,43},
{24,28,27,31,32,37,35,39},
{29,25,26,30,33,36,38,34},
{98,102,100,97,94,90,89,93},
{103,99,101,96,95,91,92,88},
{107,106,105,104,87,86,85,84},
{110,109,108,111,80,83,82,81},
{114,113,112,115,76,79,78,77},
{117,118,116,119,72,75,73,74},
{121,120,122,123,68,69,71,70},
{124,125,126,127,64,65,66,67}};



 const byte pad_lookup_x [64] PROGMEM = {
0,0,0,0,1,1,1,1,
2,2,2,2,3,3,3,3,
4,4,4,4,5,5,5,5,
6,7,7,6,6,7,7,6,
6,7,7,6,7,6,7,6,
5,5,5,5,4,4,4,4,
3,3,3,3,2,2,2,2,
1,1,1,1,0,0,0,0
};

const byte pad_lookup_y [64] PROGMEM = {
3,2,1,0,3,2,0,1,
3,1,0,2,3,0,1,2,
3,0,1,2,0,1,2,3,
0,1,2,2,1,0,3,3,
4,4,7,6,5,5,6,7,
4,5,6,7,5,6,7,4,
5,6,7,4,5,7,6,4,
6,7,5,4,7,6,5,4
};
