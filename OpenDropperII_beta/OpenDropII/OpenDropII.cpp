/*--------------------------------------------------------------------
  This file is part of the OpenDrop library
  by Urs Gaudenz, GaudiLabs 2016
  --------------------------------------------------------------------*/



#include "OpenDropII.h"
#include "hardware_def.h"
#include "Adafruit_GFX.h"
#include <Adafruit_SSD1306.h>


  bool Fluxls[FluxlPad_width][FluxlPad_heigth];
  byte pad_feedback [128];

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


// ******** Functions *****************

void clear_Fluxels()
{
    for (int x = 0; x <FluxlPad_width ; x++) 
    for (int y = 0; y <FluxlPad_heigth ; y++) 
  Fluxls[x][y]=false;

};


bool free_Fluxel(uint8_t  x,uint8_t  y,uint8_t  dir)
{bool check=true;


if (check&&(x>0)&&(dir==4)) check = pad_feedback[pgm_read_byte_near(&FluxelID[x-1][y])]==1; //links
if (check&&(x<(FluxlPad_width-1))&&(dir==2)) check = pad_feedback[pgm_read_byte_near(&FluxelID[x+1][y])]==1; //rechts
if (check&&(y>0)&&(dir==1)) check = pad_feedback[pgm_read_byte_near(&FluxelID[x][y-1])]==1; // oben
if (check&&(y<(FluxlPad_heigth-1))&&(dir==3)) check = pad_feedback[pgm_read_byte_near(&FluxelID[x][y+1])]==1; //unten
if (check&&(x>0)&(y>0)&&((dir==1)||(dir==4))) check = pad_feedback[pgm_read_byte_near(&FluxelID[x-1][y-1])]==1; //links oben
if (check&&(x>0)&(y<(FluxlPad_heigth-1))&&((dir==3)||(dir==4))) check = pad_feedback[pgm_read_byte_near(&FluxelID[x-1][y+1])]==1; //links unten
if (check&&(x<(FluxlPad_width-1))&&(y>0)&&((dir==1)||(dir==2))) check = pad_feedback[pgm_read_byte_near(&FluxelID[x+1][y-1])]==1; //rechts oben
if (check&&(x<(FluxlPad_width-1))&&(y<(FluxlPad_heigth-1))&&((dir==2)||(dir==3))) check = pad_feedback[pgm_read_byte_near(&FluxelID[x+1][y+1])]==1; //rechts unten


return check;
};








// *************** Desinitions ****************33

OpenDrop::OpenDrop(uint8_t addr) {
  _addr = addr;

}

void OpenDrop::update_Display(void) // Updated Display with Fluxls data
   
{
// clear Display
  display.clearDisplay();  


// write coordinates
  display.setCursor(18,4);
  display.print("X: ");
  display.print(drops[0].position_x());
  display.print(" Y: ");
  display.print(drops[0].position_y());

// draw Grid
  for (int x = 1; x <FluxlPad_width ; x++) 
    for (int y = 1; y <FluxlPad_heigth ; y++) 
  display.drawPixel(x*6+16,y*6+16,WHITE);
  
  
// draw Fluxels
      for (int x = 0; x <FluxlPad_width ; x++) 
    for (int y = 0; y <FluxlPad_heigth ; y++) 
   if (Fluxls[x][y]) {
           display.drawRect(x*6+17, y*6+17, 5,5, 1);
    }
    
if (feedback_flag) {
// draw Feedback
      for (int x = 0; x <64 ; x++) 
   
   if (pad_feedback[x]==0) {
           display.fillRect(pgm_read_byte_near(pad_lookup_x+x)*6+18, pgm_read_byte_near(pad_lookup_y+x)*6+18, 3,3, 1);
    }
    
          for (int x = 0; x <64 ; x++) 
   
   if (pad_feedback[x+64]==0) {
           display.fillRect((15-pgm_read_byte_near(pad_lookup_x+x))*6+18, (7-pgm_read_byte_near(pad_lookup_y+x))*6+18, 3,3, 1);
    }
    }
//draw cursor
 display.drawRect(_joy_x*6+17, _joy_y*6+18, 5,3, 1);
     
// draw Rect
     display.drawRect(16, 16, 97,48, 1);

// update display
    display.display();

};



void OpenDrop::drive_Fluxels(void)  // Fill the chip with Fluxls array data

{  
    digitalWrite(LE_pin, LOW);   // set LE 
    digitalWrite(CLK_pin, LOW);   // set SCK 
   
    for (int i = 0; i <64 ; i++) 

{ digitalWrite(DI_pin,Fluxls[pgm_read_byte_near(pad_lookup_x+i)][pgm_read_byte_near(pad_lookup_y+i)]);

   digitalWrite(CLK_pin, HIGH);   // set LE
   digitalWrite(CLK_pin, LOW);   // set LE
  };
   
    for (int i = 0; i <64 ; i++) 
{
   digitalWrite(DI_pin,Fluxls[15-pgm_read_byte_near(pad_lookup_x+i)][7-pgm_read_byte_near(pad_lookup_y+i)]);

   digitalWrite(CLK_pin, HIGH);   // set LE
   digitalWrite(CLK_pin, LOW);   // set LE
 
   };
   
  
    digitalWrite(LE_pin, HIGH);   // set LE 
    digitalWrite(LE_pin, LOW);   // set LE 

    digitalWrite(BL_pin, HIGH); // turn on

};

void OpenDrop::read_Fluxels(void) 
{
    noInterrupts();

  bool reading;

// clear driver chip
digitalWrite(BL_pin, LOW);
 digitalWrite(DI_pin,LOW);
     for (int i = 0; i <128 ; i++) 
     {   digitalWrite(CLK_pin, HIGH);   // set LE
   digitalWrite(CLK_pin, LOW);   // set LE
     }

// turn first Electrode on
 digitalWrite(DI_pin,HIGH);

   digitalWrite(CLK_pin, HIGH);   // set LE
   digitalWrite(CLK_pin, LOW);   // set LE
       digitalWrite(LE_pin, HIGH);   // set LE 
    digitalWrite(LE_pin, LOW);   // set LE 
    
 digitalWrite(DI_pin,LOW);

// read them one by one
     for (int i = 0; i <128 ; i++) 
{

delayMicroseconds(10);

digitalWrite(BL_pin, HIGH);

reading=digitalRead(FEEDBACK_pin);
pad_feedback[127-i]=reading;

  digitalWrite(BL_pin, LOW);

  digitalWrite(CLK_pin, HIGH);   // set LE
  digitalWrite(CLK_pin, LOW);   // set LE
   
  digitalWrite(LE_pin, HIGH);   // set LE 
  digitalWrite(LE_pin, LOW);   // set LE 
      
   };
   

  interrupts();

}



void OpenDrop::begin(uint16_t freq) {

// Initialize HV Chip

  pinMode(BL_pin, OUTPUT); //BL pin
  digitalWrite(BL_pin, HIGH);   // set BL 
  pinMode(LE_pin, OUTPUT); //LE pin
  digitalWrite(LE_pin, LOW);   // clr LE 
  pinMode(CLK_pin, OUTPUT); //SCK pin
  digitalWrite(CLK_pin, LOW);   // clr SCK 
  pinMode(DI_pin, OUTPUT); //DI pin
  digitalWrite(DI_pin, LOW);   // clr DI 
  pinMode(POL_pin, OUTPUT); //POL pin
  digitalWrite(POL_pin, HIGH);   // set POL 

// Initialize LED
  pinMode(LED_LIGHT_pin, OUTPUT); //Set LED pin output


// Initialize Buttons
  pinMode(SW1_pin, INPUT); 
  digitalWrite(SW1_pin, HIGH);  
  pinMode(SW2_pin, INPUT); 
  digitalWrite(SW2_pin, HIGH);  
  pinMode(SW3_pin, INPUT); 
  digitalWrite(SW3_pin, HIGH);  
  pinMode(JOY_pin, INPUT); 

  display.begin(SSD1306_SWITCHCAPVCC);
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();
display.dim(false);

display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,20);
  display.println("Loading protocol...");
  display.display();
  delay(500);
  display.setCursor(5,29);
  display.println("Press Start");
display.display();

this->set_joy(5,5);

if (light_flag)   digitalWrite(LED_LIGHT_pin, HIGH);   // set SCK 

}

void OpenDrop::update_Drops(void) {

for (int i = 0; i <this->drop_count ; i++) 
{

if (feedback_flag){

if ((drops[i].position_x()!=drops[i].next_x())|(drops[i].position_y()!=drops[i].next_y()))
{
if ((pad_feedback[pgm_read_byte_near(&FluxelID[drops[i].position_x()][drops[i].position_y()])]==1)&(pad_feedback[pgm_read_byte_near(&FluxelID[drops[i].next_x()][drops[i].next_y()])]==0)) {drops[i].begin(drops[i].next_x(),drops[i].next_y());};  

};


} else
drops[i].begin(drops[i].next_x(),drops[i].next_y());

};
}


void OpenDrop::update(void) {

clear_Fluxels();                     //clear Fluxel Array


//Fill Fluxel Array
for (int i = 0; i <this->drop_count ; i++) 
{
  Fluxls[drops[i].next_x()][drops[i].next_y()]=true;
};

if (feedback_flag)
this->read_Fluxels();

this->drive_Fluxels();

this->update_Display();



 if(sound_flag)   tone(SPK_pin,1000,10);  

}

bool OpenDrop::run(void) {
bool transition=false;
this->read_Fluxels();
this->update_Drops();

for (int i = 0; i <this->drop_count ; i++) 
{
bool up =true;
bool down = true;
bool left = true;
bool right = true;
int d1=6000;
int d2=6000;



_runing=true;


_runing= ((drops[i].position_x()!=drops[i].goal_x())|(drops[i].position_y()!=drops[i].goal_y()));
transition=transition|_runing;

if (_runing&!drops[i].is_moving()) {

if (drops[i].position_x()==0) left=false;
if (drops[i].position_x()==(FluxlPad_width-1)) right=false;
if (drops[i].position_y()==0) up=false;
if (drops[i].position_y()==(FluxlPad_heigth-1)) down=false;


if (right) if (!free_Fluxel(drops[i].position_x()+1,drops[i].position_y(),2)) right=false;
if (left) if (!free_Fluxel(drops[i].position_x()-1,drops[i].position_y(),4)) left=false;
if (up) if (!free_Fluxel(drops[i].position_x(),drops[i].position_y()-1,1)) up=false;
if (down) if (!free_Fluxel(drops[i].position_x(),drops[i].position_y()+1,3)) down=false;


if (right) d1=pow((drops[i].position_x()+1)-(drops[i].goal_x()),2)+pow((drops[i].position_y())-(drops[i].goal_y()),2);

if (left) {d2=pow((drops[i].position_x()-1)-(drops[i].goal_x()),2)+pow((drops[i].position_y())-(drops[i].goal_y()),2); if (d2<d1) {right=false;d1=d2;} else left=false;};

/*
if (up) Serial.println("up");
if (down) Serial.println("down");
if (left) Serial.println("left");
if (right) Serial.println("rigth");
*/

if (up) {d2=pow((drops[i].position_x())-(drops[i].goal_x()),2)+pow((drops[i].position_y()-1)-(drops[i].goal_y()),2); if (d2<d1) {left=false;right=false;d1=d2;} else up=false;}

if (down) {d2=pow((drops[i].position_x())-(drops[i].goal_x()),2)+pow((drops[i].position_y()+1)-(drops[i].goal_y()),2); if (d2<d1) {left=false;up=false;right=false;d1=d2;} else down=false;}

/*
Serial.println("pow");
Serial.println(d1);
Serial.println(drops[i].position_x());
Serial.println(drops[i].position_y());
*/




if (up) drops[i].move_up(); else
if (down) drops[i].move_down(); else
if (left) drops[i].move_left(); else
if (right) drops[i].move_right();

};  //if move  

}; // for loop


this->update();

return transition;
};


Drop *OpenDrop::getDrop() {
uint8_t num=this->drop_count;
num++;
this->drop_count=num;
    drops[num-1]._dropnum = num;
//Serial.println(max_drops);
  if (num > max_drops) return NULL;
   
  return &drops[num-1];
}


Drop::Drop(void) {

}


void OpenDrop::set_joy(uint8_t x,uint8_t y)
{
  _joy_x=x;
  _joy_y=y;
}

void Drop::begin(int x,int y) {
  _pos_x=x;
  _pos_y=y;
  _next_x=x;
  _next_y=y;
  _moving = false;
}

void Drop::move_right(void) {
  
if (_pos_x<FluxlPad_width-1)
  _next_x=_pos_x+1;
  _next_y=_pos_y;
  _moving = true;
}

void Drop::move_left(void) {

if (_pos_x>0)
  _next_x=_pos_x-1;
  _next_y=_pos_y;
  _moving = true;
}

void Drop::move_up(void) {

if (_pos_y>0) 
  _next_y=_pos_y-1;
  _next_x=_pos_x;
  _moving = true;
}

void Drop::move_down(void) {
  
if (_pos_y<(FluxlPad_heigth-1))
  _next_y=_pos_y+1;
  _next_x=_pos_x;
  _moving = true;
}

int Drop::position_x(void) {
  
  return _pos_x;
}


int Drop::position_y(void) {
  
  return _pos_y;
}

int Drop::next_x(void) {
  
  return _next_x;
}


int Drop::next_y(void) {
  
  return _next_y;
}



int Drop::goal_x(void) {
  
  return _goal_x;
}


int Drop::goal_y(void) {
  
  return _goal_y;
}



int Drop::num(void) {
  
  return _dropnum;
}

void Drop::go(int x,int y) {
  _goal_x=x;
  _goal_y=y;
}

bool Drop::is_moving(void) {
  
  return _moving;
}











