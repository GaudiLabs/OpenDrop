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

#define P1_pin         1           // Extentsion pin 1
#define P2_pin         3           // Extentsion pin 2

#define SW1_pin         8           // Buton 1 pin
#define SW2_pin         6           // Buton 2 pin
#define SW3_pin         7           // Buton 3 pin
#define JOY_pin         A4           // Jostic analog pin
#define FEEDBACK_pin    A2           // Feedback Amplifier pin
#define VSENS_pin       A3           // VSense pin
#define SENSPAD_pin     10           // VSense pin

#define VSCK_pin        12           // Voltage set chip ckl      0 
#define VSI_pin         11           // Voltage set chip serial out 22
#define VCS_pin         38           // Voltage set chip

#define LED_D13_pin         LED_BUILTIN           // D13 LED
#define LED_Rx_pin         25           // Rx LED

#define sound_flag true           // Make Sound
#define feedback_flag false           // Use Feedback information
        




const byte FluxelID [16][8] PROGMEM ={
{0,63,1,62,2,61,3,60},
{4,59,5,58,6,57,7,56},
{8,55,9,54,10,53,11,52},
{12,51,13,50,14,49,15,48},
{16,47,17,46,18,45,19,44},
{20,43,21,42,22,41,23,40},
{24,39,25,38,26,37,27,36},
{28,35,29,34,30,95,96,97},
{33,32,31,94,98,93,99,92},
{100,91,101,90,102,89,103,88},
{104,87,105,86,106,85,107,84},
{108,83,109,82,110,81,111,80},
{112,79,113,78,114,77,115,76},
{116,75,117,74,118,73,119,72},
{120,71,121,70,122,69,123,68},
{124,67,125,66,126,65,127,64}};



 const byte pad_lookup_x [64] PROGMEM = {
0,0,0,0,1,1,1,1,
2,2,2,2,3,3,3,3,
4,4,4,4,5,5,5,5,
6,6,6,6,7,7,7,8,
8,8,7,7,6,6,6,6,
5,5,5,5,4,4,4,4,
3,3,3,3,2,2,2,2,
1,1,1,1,0,0,0,0
};

const byte pad_lookup_y [64] PROGMEM = {
0,2,4,6,0,2,4,6,
0,2,4,6,0,2,4,6,
0,2,4,6,0,2,4,6,
0,2,4,6,0,2,4,2,
1,0,3,1,7,5,3,1,
7,5,3,1,7,5,3,1,
7,5,3,1,7,5,3,1,
7,5,3,1,7,5,3,1
};
