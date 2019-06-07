//OpenDrop V3 Hardware Definitions


#define FluxlPad_width 16
#define FluxlPad_heigth 8


// If using software SPI (the default case):
#define OLED_MOSI   20
#define OLED_CLK   21
#define OLED_DC    0 //11
#define OLED_CS    22 //12
#define OLED_RESET 5

#define BL_pin         9           // BL Pin
#define LE_pin          4          // LE Pin
#define CLK_pin         24         // CLK Pin
#define DI_pin         23          // Data Out Pin
#define POL_pin         2         // POL Pin
#define AC_pin         A1         // AC Pin  10

#define INVERTED	true	//Invert the Opto-Coupler pins
#define INVERTED2	true	//Invert the Opto-Coupler pins

#define SW1_pin         8           // Buton 1 pin
#define SW2_pin         6           // Buton 2 pin
#define SW3_pin         7           // Buton 3 pin
#define JOY_pin         A4           // Jostic analog pin
#define FEEDBACK_pin    A2           // Feedback Amplifier pin
#define VSENS_pin       A3           // VSense pin
#define SENSPAD_pin       A5           // VSense pin

#define VSCK_pin        12           // Voltage set chip ckl      0 
#define VSI_pin         11           // Voltage set chip serial out 22
#define VCS_pin         38           // Voltage set chip

#define LED_D13_pin         LED_BUILTIN           // D13 LED
#define LED_Rx_pin         25           // Rx LED

#define sound_flag true           // Make Sound
#define feedback_flag false           // Use Feedback information
        




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
