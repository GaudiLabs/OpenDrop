/*--------------------------------------------------------------------
  This file is part of the OpenDrop library
  by Urs Gaudenz, GaudiLabs 2016
  --------------------------------------------------------------------*/



#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#include "Adafruit_GFX.h"

#define max_drops   8          


class OpenDrop;


class Drop {

 public:

  // Constructor: number of LEDs, pin number, LED type
  Drop(void);

  friend class OpenDrop;

 void(begin(int x,int y));
 void(move_right());
 void(move_left());
 void(move_up());
 void(move_down());
 void(go(int x,int y));
 int position_x();
 int position_y();
 int goal_x();
 int goal_y();
 int next_x();
 int next_y();
 int num();
 bool is_moving();

private:

uint8_t _pos_x;
uint8_t _pos_y;
uint8_t _goal_x;
uint8_t _goal_y;
uint8_t _next_x;
uint8_t _next_y;
uint8_t _dropnum;
bool _moving;


};


class OpenDrop
{
 public:
  OpenDrop(uint8_t addr = 0x60);
  friend class Drop;
  void begin(char code_str[]);

  bool run(void);
  void dispense(int reservoir, int delay_us);
  void update(void);
  void set_Fluxels(bool fluxels_array[][8]);
  bool get_Fluxel(int x, int y);
  void update_Display(void);
  void update_Drops(void);
  void drive_Fluxels(void);
  void read_Fluxels(void);
  void set_joy(uint8_t x,uint8_t y);
  void show_joy(boolean val);
  void show_feedback(boolean val);
  void set_voltage(uint16_t voltage, bool AC_on,uint16_t frequence);
  void set_Pin(uint8_t pin, boolean val);
  void set_Magnet(uint8_t magnet, bool state);
  uint8_t get_ID(void);
  
  float get_Temp_1(void);
  float get_Temp_2(void);
  float get_Temp_3(void);
  
  void set_Temp_1(uint8_t temperature);
  void set_Temp_2(uint8_t temperature);
  void set_Temp_3(uint8_t temperature);
  
  uint8_t get_Temp_L_1(void);
  uint8_t get_Temp_H_1(void);  
  uint8_t get_Temp_L_2(void);
  uint8_t get_Temp_H_2(void);  
  uint8_t get_Temp_L_3(void);
  uint8_t get_Temp_H_3(void);  
  
  Drop *getDrop();


 private:
    uint8_t _addr;
    uint16_t _freq;
    Drop drops[max_drops];
    uint8_t drop_count=0;
    int _runing;
    uint8_t _joy_x;
    uint8_t _joy_y;
    bool _show_joy;
  //  PWM _pwm;
};


void Menu(OpenDrop &theOpenDrop);
