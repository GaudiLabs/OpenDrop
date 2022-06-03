//OpenDrop V4.1 Hardware Definitions


#define board_ID	0x1     // 0x1=V42 ,OpenDrop ID = board_ID higher 4 bits, Adapter ID lower 4 bits 
#define OpenDropV40_downgrade 0  //use for V40

#define FluxlPad_width 16
#define FluxlPad_heigth 8


// If using software SPI (the default case):
#define OLED_MOSI   20
#define OLED_CLK   21
#define OLED_DC    0 //11
#define OLED_CS    22 //12
#define OLED_RESET 5

#define BL_pin         9           // BL Pin

#if OpenDropV40_downgrade
#define LE_pin          4          // LE Pin for V40
#else
#define LE_pin          1          // LE Pin 
#endif

#define CLK_pin         24         // CLK Pin
#define DI_pin         23          // Data Out Pin
#define POL_pin         2         // POL Pin
#define AC_pin         A1         // AC Pin  10

#define opto_LOW	LOW	    //HIGH LOW for Opto-Couplers
#define opto_HIGH	!opto_LOW   //can be used to invert Optos 

#if OpenDropV40_downgrade
#define P1_pin         1           // Extentsion pin 1  for V40
#else
#define P1_pin         4           // Extentsion pin 1  
#endif

#define P2_pin         3           // Extentsion pin 2

#define SW1_pin         8           // Buton 1 pin
#define SW2_pin         6           // Buton 2 pin
#define SW3_pin         7           // Buton 3 pin
#define JOY_pin         A4           // Jostic analog pin
#define FEEDBACK_pin    A2           // Feedback Amplifier pin
#define VSENS_pin       A3           // VSense pin
#define SENSPAD_pin     10           // SensPad pin
#define V_USB_pin     	 A5           // SensPad pin

#define VSCK_pin        12           // Voltage set chip ckl      0 
#define VSI_pin         11           // Voltage set chip serial out 22
#define VCS_pin         38           // Voltage set chip

#define LED_D13_pin         LED_BUILTIN           // D13 LED
#define LED_Rx_pin         25           // Rx LED

#define sound_flag true           // Make Sound
#define feedback_flag false           // Read pad feedback
#define closedloop_flag false        // Use Feedback information for close loop control (experimental)
        

#define STBY_pin         6           // MCP23008 Pin
#define MODE_pin         4           // MCP23008 Pin
#define BOOST_pin        5           // MCP23008 Pin
#define PHASE_A_pin      1           // MCP23008 Pin
#define ENABLE_A_pin     0           // MCP23008 Pin
#define PHASE_B_pin      3           // MCP23008 Pin
#define ENABLE_B_pin     2           // MCP23008 Pin

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

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
