/*--------------------------------------------------------------------
  This file is part of the OpenDrop library
  by Urs Gaudenz, GaudiLabs 2016
  --------------------------------------------------------------------*/



#include "OpenDrop.h"
#include "hardware_def.h"

// Constructor when length, pin and type are known at compile-time:
Drop::Drop(int no) 
{
 _no=no;
}


void Drop::begin(int x,int y) {
  _pos_x=x;
  _pos_y=y;
  //begun = true;
}

void Drop::move_right(void) {
  
if (_pos_x<FluxlPad_width-1)
  _pos_x++;

}

void Drop::move_left(void) {

if (_pos_x>0)
  _pos_x--;
}

void Drop::move_up(void) {

if (_pos_y>0) 
  _pos_y--;
}

void Drop::move_down(void) {
  
if (_pos_y<FluxlPad_heigth-1)
  _pos_y++;
}

int Drop::position_x(void) {
  
  return _pos_x;
}

int Drop::position_y(void) {
  
  return _pos_y;
}



