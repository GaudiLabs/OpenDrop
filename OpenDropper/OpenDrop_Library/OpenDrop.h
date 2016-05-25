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



class Drop {

 public:

  // Constructor: number of LEDs, pin number, LED type
  Drop(int no);

 void(begin(int x,int y));
 void(move_right());
 void(move_left());
 void(move_up());
 void(move_down());
 int position_x();
 int position_y();
 bool status();

private:

int _pos_x;
int _pos_y;
int _no;


};
