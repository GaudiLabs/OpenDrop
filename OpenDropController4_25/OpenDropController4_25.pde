// Use thread() /  / speedup uP software / baud rate


import processing.serial.*;
import processing.net.*; 

import static javax.swing.JOptionPane.*;

final int hdispY_pos=307;
final boolean debug = true;

String arrayLayout = "electrodes.json";

String version_str = "OpenDrop V4.0";

boolean showNumbers = false;
float setTemperature1=25;
float setTemperature2=25;
float setTemperature3=25;

boolean showSetTemp = false;
float Temperature1;
float Temperature2;
float Temperature3;

Serial myPort;  // Create object from Serial class
float val;      // Data received from the serial port
PImage img;  // Declare variable "a" of type PImage

float time = 1000;
float timeincrement = 0.01;
int fluxel_number = 128+16; // fluexel array plus two control lines

int xsize = 16;
int ysize = 8;
int posX = 2024;
int posY = 2024;
int init_flags=1;
int displayX=250;
int displayY=55;
int electrodes_loaded=0;

boolean invert=true;
boolean dragg_mode=false;
boolean transmitting=false;
boolean old_transmit=false;

boolean showTempUI = false;
boolean showFeedbackUI = false;


final int box0_size=10;
final int boxX0=20;
final int boxY0=hdispY_pos+124;

int frame_max=1;

int box1=1;
int box1_size=5;
int boxX1=20;
int boxY1=hdispY_pos+320;

int interval=120;
int interval_ms;
int interval_update_ms=300;
int interval_edit_ms=1800;

int butX=180;
final int butY=hdispY_pos+240;

color  rectColor = color(0,0,0);
color  rectHighlight = color(0,180,0);
int boxSize = 21;     // Diameter of rect



int max_frame_no = 200;
float[][] data = new float[xsize][ysize+1];
int[] control_data_in = new int[24];
int[] control_data_out = new int[14];
boolean[][] fluxels = new boolean[fluxel_number][max_frame_no+1];
boolean[] fluxels_state = new boolean[fluxel_number];

int time_start=0;
int time_start_update=0;
int time_edit=0;
int timeout=0;

int wSize_x=1600;
int wSize_y=1000;
int eSize=46;
int cSize=30;

int imageShift=47;

boolean play=false;
boolean life=true;
boolean feedback=false;
boolean cont_flag=false;
boolean connected=false;
boolean hasMagnet=false;


int frame_no=1;

float imgScale=1;

PFont f;
boolean changed=true;

JSONArray values;

  int x = 0;
  int y = 0;
int send_value = 0;

int last_w=0;
int last_h=0;

Electrode[] electrodeArray;



void setup() {

  

// printArray(PFont.list());
  f = createFont("Ubuntu Medium", 32);
  textFont(f);
  
 // pixelDensity(2);
  size(1024, 768); //P2D
  surface.setResizable(true);
  surface.setSize(1024, 768);
  surface.setLocation(100,100);
      
  interval_ms= int(60000/interval);
  img = loadImage("OpenDropFrame.png");  // Load the image into the program  

  myOpenSerialPort();
  transmit();
  if (!connected) 
  {old_transmit=true;
   for (int x=0; x < 96; x++)
    myPort.write(int(fluxels[x][frame_no])); 
     } else 
     readControlData();
  electrodesLoad();
  
  frameRate(60);


 ///  init_flags=1;
 //  control_data_out[6]=init_flags;
   



 } //setup



void draw() {
    
 // fill(100,100);

   thread("animate");
   drawPlane(data, xsize, ysize,y-1);


} //draw


void drawPlane(float[][] vals, int sizeX, int sizeY,int indexY) {
  stroke(0);
  fill(255, 255, 255);
  if (width<img.width) imgScale=float(width)/img.width; else imgScale=1;
//if (!(width==last_w)||!(height==last_h)||changed)  {
background(255,250,240);
image(img, width/2-img.width*imgScale/2, 0, img.width*imgScale, img.height*imgScale); last_w=width;last_h=height;
//}

//img.resize(500, 500);

  textSize(32*imgScale);
  fill(255, 255, 255);
  text(frame_no, coX(-12),coY(16)) ;   
  text(interval, coX(+18),coY(16)) ;   
  text("play", coX(37),coY(-7.5));    
  text("life", coX(37),coY(-4.5));  
 if (showFeedbackUI) text("feedback", coX(37),coY(-1.5));  
  
  text("copy last frame", coX(37),coY(5.5));  
  text("clear frame", coX(37),coY(8.5));  
  text("clear all", coX(37),coY(11.5));  
  
  text("save", coX(32),coY(-18.5));  
  text("load", coX(38),coY(-18.5));     
  text("frame_no:", coX(-20),coY(16)); 
  text("frames/min:", coX(+10),coY(16)); 

  text("reconnect", coX(-41),coY(16));  

  text(version_str, coX(-43),coY(-18));    
  
  stroke(255,255,255);
  // rect(displayX,displayY,sizeX*rectsize,sizeY*rectsize);
  fill(255, 200, 0);
  textSize(11);
  strokeWeight(6*imgScale);
  
// draw electrodes
  for (int i=0; i<electrodes_loaded; i++) 
  {
     fillColorFlux(electrodeArray[i].e,frame_no); 
     electrodeArray[i].drawElectrode();
  }



// draw buttons
  int boxSize=int(imgScale*eSize);
 
  if (play)  fill(255, 255, 255); else fill(255, 0, 0);
  rect(coX(34),coY(-9),boxSize,boxSize);
  
  if (life)  fill(255, 255, 255); else fill(255, 0, 0);
  rect(coX(34),coY(-6),boxSize,boxSize);
  
  //feedback button
   if (showFeedbackUI){
  if (feedback)  fill(255, 255, 255); else fill(255, 0, 0);
  rect(coX(34),coY(-3),boxSize,boxSize);
   };
   
  fill(255, 0, 0);
  // reconect button
  rect(coX(-43.5),coY(14.5),boxSize,boxSize);

  // copy last
  rect(coX(34),coY(4),boxSize,boxSize);
  
  // clear frame
  rect(coX(34),coY(7),boxSize,boxSize);
  
  // clear all
  rect(coX(34),coY(10),boxSize,boxSize);
  
  // Temperatures
 if(showTempUI){
   fill(255, 0, 0);
   rect(coX(-43.5),coY(-6),boxSize*3,boxSize);
   rect(coX(-43.5),coY(-2),boxSize*3,boxSize);
   rect(coX(-43.5),coY(2),boxSize*3,boxSize);
   
    fill(255, 255, 255);

  triangle(coX(-36.5),coY(-5.3),coX(-36.5)+boxSize/2,coY(-5.3),coX(-36.5)+boxSize/4,coY(-5.3)-boxSize/4);
  triangle(coX(-36.5),coY(-4.6),coX(-36.5)+boxSize/2,coY(-4.6),coX(-36.5)+boxSize/4,coY(-4.6)+boxSize/4);

  triangle(coX(-36.5),coY(-1.3),coX(-36.5)+boxSize/2,coY(-1.3),coX(-36.5)+boxSize/4,coY(-1.3)-boxSize/4);
  triangle(coX(-36.5),coY(-0.6),coX(-36.5)+boxSize/2,coY(-0.6),coX(-36.5)+boxSize/4,coY(-0.6)+boxSize/4);

  triangle(coX(-36.5),coY(2.7),coX(-36.5)+boxSize/2,coY(2.7),coX(-36.5)+boxSize/4,coY(2.7)-boxSize/4);
  triangle(coX(-36.5),coY(3.4),coX(-36.5)+boxSize/2,coY(3.4),coX(-36.5)+boxSize/4,coY(3.4)+boxSize/4);
 
   

   textSize(32*imgScale);
   text("T1:", coX(-46),coY(-4.6));  
   text("T2:", coX(-46),coY(-0.6));  
   text("T3:", coX(-46),coY(3.4));  
     
     
     if (showSetTemp)  {
       
       fill(255, 255, 0);
  text(setTemperature1+"°C", coX(-43),coY(-4.6));  
  text(setTemperature2+"°C", coX(-43),coY(-0.6));  
  text(setTemperature3+"°C", coX(-43),coY(3.4));  } else
  
  {
      fill(255, 255, 255);
  if(Temperature1<150)text(nf(Temperature1,0,1)+"°C", coX(-43),coY(-4.6));else text("---", coX(-43),coY(-4.6));   
  if(Temperature1<150)text(nf(Temperature2,0,1)+"°C", coX(-43),coY(-0.6));  else text("---", coX(-43),coY(-0.6)); 
  if(Temperature1<150)text(nf(Temperature3,0,1)+"°C", coX(-43),coY(3.4)); else text("---", coX(-43),coY(3.4));
}

 }

   //connected button
  if (connected)
         fill(0, 255, 0);
    else 
       fill(255,0, 0);    
  circle(coX(-44.5),coY(-18.5),boxSize/2);
  
  // reservoir buttons
    fill(255, 0, 0);
  rect(coX(28),coY(-6),boxSize,boxSize);
  rect(coX(28),coY(4),boxSize,boxSize);
  rect(coX(-30),coY(-6),boxSize,boxSize);
  rect(coX(-30),coY(4),boxSize,boxSize);
  
  
  fill(150, 150, 150);

// draw frame graphics
  for (int i=1; i<18; i++) 
    {
     if(frame_no==i)  fill(255, 0, 0);  else   fill(150, 150, 150);
     circle (coX(-18+i*2),coY(19.5),cSize*imgScale); 
    }
    fill(150, 150, 150);
  triangle(coX(-20),coY(19.5),coX(-20)+boxSize,coY(19.5)-boxSize/2,coX(-20)+boxSize,coY(19.5)+boxSize/2);
  if(frame_no>=18) fill(255, 0, 0); 
  triangle(coX(20),coY(19.5),coX(20)-boxSize,coY(19.5)-boxSize/2,coX(20)-boxSize,coY(19.5)+boxSize/2);
    
      
} // drawPlain


void animate()
{
// *** Play 
  if (play&&(millis()-time_start>interval_ms))
    {
      time_start=millis();
      if (frame_no<frame_max) frame_no++; else frame_no=1;
      changed=true;
    }
    
      if (feedback&&(millis()-time_start_update>interval_update_ms))
    {
      time_start_update=millis();
      changed=true;
    }
    
    if (millis()-time_edit>interval_edit_ms)  showSetTemp=false; else showSetTemp=true;
    
 // ** Write to device
if (changed&&life&&!transmitting) 
    {transmitting=true;
    if (old_transmit) old_transmit(); else transmit();
    transmitting=false;}
    
}

void transmit()
{
    
    changed=false;
    while (myPort.available() > 0) myPort.read();

    
    for (int x=0; x < xsize+2; x++) //Send Fluxels plus two control lines
    {
    send_value=0;

    for (int y=0; y < 8; y++){
    send_value=send_value<<1;
    send_value=send_value+int(fluxels[(7-y)+x*8][frame_no]);
    }
    myPort.write(send_value); 

    }
    
    control_data_out[8]=(int)setTemperature1;
    control_data_out[9]=(int)setTemperature2;
    control_data_out[10]=(int)setTemperature3;

    if (feedback) control_data_out[6]=1; else
    control_data_out[6]=0;

    
    for (int x=0; x < 14; x++) //Send 14 control bytes
    myPort.write(control_data_out[x]); 
    
    x=0;

      timeout=millis();

while((x<16)&((millis()-timeout<500)))
{
    delay(1); // does not work without ... no idea why.
     while ((myPort.available() > 0)&(x<24)) {
     control_data_in[x] = myPort.read();
     //println(control_data_in[x] );
      x++;
     }  
   }
   Temperature1=(float)control_data_in[17]/100+control_data_in[18];
   Temperature2=(float)control_data_in[19]/100+control_data_in[20];
   Temperature3=(float)control_data_in[21]/100+control_data_in[22];

   if (millis()-timeout<500) connected=true; else  connected=false;
   
   // translate feedback
   for (int x=0; x < xsize; x++) //Send Fluxels plus two control lines
    {int readdata=control_data_in[x];
   // println(readdata);
   for (int y=0; y < 8; y++){
        fluxels_state[(7-y)+x*8]=boolean(((readdata) >> (y)) & 0x01);
    }
    }
   
 
} // trnsmit

void old_transmit()
{
 {
    changed=false;
    for (int x=0; x < 128; x++)
    myPort.write(int(fluxels[x][frame_no])); 
     }
   
 }

void readControlData() 
{print("BoardID: "); println(control_data_in[23]);
      switch(control_data_in[23]) {
  case 0: 
    version_str="OpenDrop: V4.1"; 
    if (arrayLayout=="electrodes_magnet.json") arrayLayout="electrodes.json";
    showTempUI=false;
    showFeedbackUI=false;
    break;
  case 1: 
    version_str="OpenDrop: V4.1 \n magnet"; 
    if (arrayLayout=="electrodes.json") arrayLayout="electrodes_magnet.json";
    showTempUI=false;
    showFeedbackUI=false;
    break;
    
   case 0x10: 
    version_str="OpenDrop: V4.2"; 
    if (arrayLayout=="electrodes_magnet.json") arrayLayout="electrodes.json";
    showTempUI=false;
    showFeedbackUI=true;
    break;
    
   case 0x11: 
    version_str="OpenDrop: V4.2 \n magnet"; 
    if (arrayLayout=="electrodes.json") arrayLayout="electrodes_magnet.json";
     showTempUI=false;
     showFeedbackUI=true;
    break;
    
   case 0x12: 
    version_str="OpenDrop: V4.2 \n magnet & temperature Adapter"; 
    if (arrayLayout=="electrodes.json") arrayLayout="electrodes_magnet.json";
     showTempUI=true;
     showFeedbackUI=true;
    break;
    
    
} //switch  
} //readControlData

void mouseDragged()
{
 float mx=(mouseX-(width/2)+100*eSize*imgScale)/(imgScale*eSize/2)-200;
  float my=(mouseY-(img.height/2-imageShift)*imgScale+100*eSize*imgScale)/(imgScale*(eSize/2))-200;
  
  if ((mx>-24)&&(mx<24)&&(my>-12)&&(my<12))
    {
     for (int i=0; i<electrodes_loaded; i++) 
       {
        if (inRect(mx,my,electrodeArray[i].x,electrodeArray[i].y,electrodeArray[i].h,electrodeArray[i].v))
         {
           fluxels[electrodeArray[i].e][frame_no]=dragg_mode;; 
          }
        }
     }
     changed=true;
}
void mousePressed() {

  if ((cont_flag)&(frame_no<max_frame_no)) frame_no++;
  
  float mx=(mouseX-(width/2)+100*eSize*imgScale)/(imgScale*eSize/2)-200;
  float my=(mouseY-(img.height/2-imageShift)*imgScale+100*eSize*imgScale)/(imgScale*(eSize/2))-200;

  if ((mx>-24)&&(mx<24)&&(my>-12)&&(my<12))
    {
     for (int i=0; i<electrodes_loaded; i++) 
       {
        if (inRect(mx,my,electrodeArray[i].x,electrodeArray[i].y,electrodeArray[i].h,electrodeArray[i].v))
         {
           if ((fluxels[electrodeArray[i].e][frame_no]==false)|(cont_flag)) {fluxels[electrodeArray[i].e][frame_no]=true;dragg_mode=true;} else {fluxels[electrodeArray[i].e][frame_no]=false;dragg_mode=false;}
          }
        }
     }

changed=true;

// check play button
  if  (inRect(mx,my,34,-9,6,2))
     {
      if (play) play=false; else 
      {
      play=true;  time_start=millis(); 
      frame_max=frameMax();
      }
     }

// check life button
  if(inRect(mx,my,34,-6,6,2))
     { 
      if (life) life=false; else 
        {
      life=true;  
       }
     }
  
     // check feedback button
  if(inRect(mx,my,34,-3,6,2)& showFeedbackUI)
     {
       
      if (feedback) feedback=false; else 
        {
      feedback=true;  
       }
     }
     

     // reconect button
  if (inRect(mx,my,-43.5,14.5,2,2))
     { print("reconnect");
      fill(255, 255, 255);
        rect(coX(-43.5),coY(14.5),eSize*imgScale,eSize*imgScale);
       myPort.stop();
       myOpenSerialPort();
              
   if (!old_transmit){ transmit();
    readControlData();
    electrodesLoad();}
     }


  
// check copy frame button
  if (inRect(mx,my,34,4,2,2)&&(frame_no>1))
     { print("copy frame");
       fill(255, 255, 255);
        rect(coX(34),coY(4),eSize*imgScale,eSize*imgScale);
       for (int x=0; x < fluxel_number; x++)
       fluxels[x][frame_no]=fluxels[x][frame_no-1];
     }  
  
  
// check clear frame button
  if (inRect(mx,my,34,7,2,2))
     { print("clear frame");
       fill(255, 255, 255);
        rect(coX(34),coY(7),eSize*imgScale,eSize*imgScale);
       for (int x=0; x < fluxel_number; x++)
       fluxels[x][frame_no]=false;
     }

  
// check clear all button
  if (inRect(mx,my,34,10,2,2))
     { print("clear");
       fill(255, 255, 255);
        rect(coX(34),coY(10),eSize*imgScale,eSize*imgScale);
       for (int x=0; x < fluxel_number; x++)
       for (int z=0; z < (max_frame_no+1); z++)
       fluxels[x][z]=false;
       frame_no=1;
     }


       
       

// check load button
 if(inRect(mx,my,38,-20,4,2))
     {
        println("load");
        selectInput("Select a file to process:", "fileLoad");
     }   
 
// check save button
  if(inRect(mx,my,32,-20,4,2))
    {
      println("save");
      selectOutput("Select desination;", "fileSaved");
    }   

// Check frame buttons
 if(inRect(mx,my,18,18.5,4,4)&(frame_no<max_frame_no)) {frame_no++; }
 if(inRect(mx,my,-20,18.5,4,4)&(frame_no>1)) {frame_no--; }
    
  
// check reservoir buttons

  if(inRect(mx,my,28,-6,2,2))
    {
      
      fluxels[15*ysize+3][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[15*ysize+3][frame_no]=true;
      fluxels[15*ysize+2][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[15*ysize+2][frame_no]=true;
      fluxels[15*ysize+1][frame_no]=true;
      fluxels[15*ysize+0][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[15*ysize+0][frame_no]=true;
      fluxels[14*ysize+1][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[14*ysize+1][frame_no]=true;
      fluxels[15*ysize+3][frame_no]=true;
      fluxels[15*ysize+2][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[14*ysize+1][frame_no]=true;
      fluxels[15*ysize+3][frame_no]=true;
    }   

  if(inRect(mx,my,28,4,2,2))
    {
      
      fluxels[15*ysize+4][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[15*ysize+4][frame_no]=true;
      fluxels[15*ysize+5][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[15*ysize+5][frame_no]=true;
      fluxels[15*ysize+6][frame_no]=true;
      fluxels[15*ysize+7][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[15*ysize+7][frame_no]=true;
      fluxels[14*ysize+6][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[14*ysize+6][frame_no]=true;
      fluxels[15*ysize+4][frame_no]=true;
      fluxels[15*ysize+5][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[14*ysize+6][frame_no]=true;
      fluxels[15*ysize+4][frame_no]=true;
    }   
    
      if(inRect(mx,my,-30,-6,2,2))
      {
      fluxels[0*ysize+3][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[0*ysize+3][frame_no]=true;
      fluxels[0*ysize+2][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[0*ysize+2][frame_no]=true;
      fluxels[0*ysize+1][frame_no]=true;
      fluxels[0*ysize+0][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[0*ysize+0][frame_no]=true;
      fluxels[1*ysize+1][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[1*ysize+1][frame_no]=true;
      fluxels[0*ysize+3][frame_no]=true;
      fluxels[0*ysize+2][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[1*ysize+1][frame_no]=true;
      fluxels[0*ysize+3][frame_no]=true;
    }   
      if(inRect(mx,my,-30,4,2,2))
{
      
      fluxels[0*ysize+4][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[0*ysize+4][frame_no]=true;
      fluxels[0*ysize+5][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[0*ysize+5][frame_no]=true;
      fluxels[0*ysize+6][frame_no]=true;
      fluxels[0*ysize+7][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[0*ysize+7][frame_no]=true;
      fluxels[1*ysize+6][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[1*ysize+6][frame_no]=true;
      fluxels[0*ysize+4][frame_no]=true;
      fluxels[0*ysize+5][frame_no]=true;
      if (frame_no<max_frame_no) frame_no++;
      fluxels[1*ysize+6][frame_no]=true;
      fluxels[0*ysize+4][frame_no]=true;
    }  
    
   if (showTempUI){
    if(inRect(mx,my,-37,-4.7,2,2)) {setTemperature1--; time_edit=millis();}
    if(inRect(mx,my,-37,-7.2,2,2)) {setTemperature1++; time_edit=millis();}
 
    if(inRect(mx,my,-37,-0.7,2,2)) {setTemperature2--; time_edit=millis();}
    if(inRect(mx,my,-37,-3.2,2,2)) {setTemperature2++; time_edit=millis();}

    if(inRect(mx,my,-37,3.3,2,2)) {setTemperature3--; time_edit=millis();}
    if(inRect(mx,my,-37,0.8,2,2)) {setTemperature3++; time_edit=millis();}

  if (inRect(mx,my,-43.5,-6,6,2)) time_edit=millis(); 
  if (inRect(mx,my,-43.5,-2,6,2)) time_edit=millis();
  if (inRect(mx,my,-43.5,2,6,2))  time_edit=millis();


    if (setTemperature1>120) setTemperature1=120;
    if (setTemperature1<20) setTemperature1=20;

    if (setTemperature2>120) setTemperature2=120;
    if (setTemperature2<20) setTemperature2=20;
    
    if (setTemperature3>120) setTemperature3=120;
    if (setTemperature3<20) setTemperature3=20;
   }

    
} // mouse pressed



void keyPressed() 
  {
    if  ((keyCode == RIGHT)&(frame_no<max_frame_no)) frame_no++;
    if  ((keyCode == LEFT)&(frame_no>1)) frame_no--;
    if (keyCode==SHIFT) cont_flag=true;
    if  (key == ' '){
      if (play) play=false; else 
      {play=true;  time_start=millis(); 
      frame_max=frameMax();}
     };

    changed=true;
  } // keyPressed

void keyReleased()
  {
    if (keyCode==SHIFT) cont_flag=false;
    changed=true;
  } //keyReleased

void mouseWheel(MouseEvent event) {
  
    float mx=(mouseX-(width/2)+100*eSize*imgScale)/(imgScale*eSize/2)-200;
  float my=(mouseY-(img.height/2-imageShift)*imgScale+100*eSize*imgScale)/(imgScale*(eSize/2))-200;
  
  
  float e = event.getCount();
  //println(e);
  
  if (inRect(mx,my,-43.5,-6,8,2)) {setTemperature1=setTemperature1-e;time_edit=millis(); } else
  if (inRect(mx,my,-43.5,-2,8,2)) {setTemperature2=setTemperature2-e;time_edit=millis();} else
  if (inRect(mx,my,-43.5,2,8,2)) {setTemperature3=setTemperature3-e;time_edit=millis();} else


   
  if (play)
   {
   interval=int(interval-e*5);
   if (interval<5) interval=5;
   if (interval>600) interval=600;
   interval_ms= int(60000/interval);
    //float v=(exp((log(interval_ms)*10+e)/10));
   // if((v>50)&(v<3000)) interval_ms=int(v);
    //interval_ms=int(v);
   // println(interval_ms);
   } else 
   {
    if ((e==-1)&(frame_no<max_frame_no)) frame_no++;
    if ((e==1)&(frame_no>1)) frame_no--;
    changed=true;
   }
   
  if (showTempUI) {
    if (setTemperature1>120) setTemperature1=120;
    if (setTemperature1<20) setTemperature1=20;

    if (setTemperature2>120) setTemperature2=120;
    if (setTemperature2<20) setTemperature2=20;
    
    if (setTemperature3>120) setTemperature3=120;
    if (setTemperature3<20) setTemperature3=20;
  }
} //mouseWheel



void fileSaved(File selection) {

  if  (selection == null) 
  {
    println("Window was closed or the user hit cancel.");
  } else 
  
  {
    println("User selected " + selection.getAbsolutePath());
    println(frameMax());
    values = new JSONArray();

    for (int i = 1; i < frameMax()+1; i++) 
    {
      JSONObject fluxel_jason = new JSONObject();

      fluxel_jason.setInt("frame", i);
      fluxel_jason.setString("y0", binary(fluxValue(0,i),18));
      fluxel_jason.setString("y1", binary(fluxValue(1,i),18));
      fluxel_jason.setString("y2", binary(fluxValue(2,i),18));
      fluxel_jason.setString("y3", binary(fluxValue(3,i),18));
      fluxel_jason.setString("y4", binary(fluxValue(4,i),18));
      fluxel_jason.setString("y5", binary(fluxValue(5,i),18));
      fluxel_jason.setString("y6", binary(fluxValue(6,i),18));
      fluxel_jason.setString("y7", binary(fluxValue(7,i),18));

      values.setJSONObject(i-1, fluxel_jason);
    }
  
  saveJSONArray(values, selection.getAbsolutePath()+".json");

  }
} //fileSave

void myOpenSerialPort()


// Open Serial Port
{
  String COMx, COMlist = "";

  try {
    if(debug) printArray(Serial.list());
    int i = Serial.list().length;
    if (i != 0) {
    int p=0;
       // need to check which port the inst uses -
       // for now we'll just let the user decide
        for (int j = 0; j < i;) {
          if(Serial.list()[j].indexOf("/dev/ttyS")!=0){print("hello");
          COMlist += char(10)+" "+char(p+'a')+" = " + Serial.list()[j];
              ++p;
          }
          ++j ;
      
        }
        
        if (p>1){
        COMx = showInputDialog("Which COM port is correct? (a,b,..):\n"+COMlist);
        if (COMx == null) exit();
        if (COMx.isEmpty()) exit();
        p = int(COMx.toLowerCase().charAt(0) - 'a') + 1;
        }
      String portName = Serial.list()[p-1];
      if(debug) println(portName);
      myPort = new Serial(this, portName, 115200); // change baud rate to your liking
      myPort.bufferUntil('\n'); // buffer until CR/LF appears, but not required..
    }
    else {
      showMessageDialog(null,"Device is not connected to the PC");
      exit();
    }
   }
     catch (Exception e)
    { //Print the type of error
    showMessageDialog(null,"COM port is not available.\n(maybe in use by another program)");
    println("Error:", e);
    exit();
    }
    
   }
void fileLoad(File selection) {

   if (selection == null) 
   {
    println("Window was closed or the user hit cancel.");
    } else 
   {
    println("User selected " + selection.getAbsolutePath());
    values = loadJSONArray(selection.getAbsolutePath());

 // Get the first array of elements
 //JSONArray values = json.getJSONArray(0);
  
   for (int i = 0; i < values.size(); i++) {
    
    JSONObject item = values.getJSONObject(i); 

    String name = item.getString("y0");
    for (int j = 0; j < name.length(); j++) { if(name.charAt(j)=='1') fluxels[j*ysize+0][i+1]=true;}
     name = item.getString("y1");
    for (int j = 0; j < name.length(); j++) { if(name.charAt(j)=='1') fluxels[j*ysize+1][i+1]=true;}
     name = item.getString("y2");
    for (int j = 0; j < name.length(); j++) { if(name.charAt(j)=='1') fluxels[j*ysize+2][i+1]=true;}
     name = item.getString("y3");
    for (int j = 0; j < name.length(); j++) { if(name.charAt(j)=='1') fluxels[j*ysize+3][i+1]=true;}
     name = item.getString("y4");
    for (int j = 0; j < name.length(); j++) { if(name.charAt(j)=='1') fluxels[j*ysize+4][i+1]=true;}
     name = item.getString("y5");
    for (int j = 0; j < name.length(); j++) { if(name.charAt(j)=='1') fluxels[j*ysize+5][i+1]=true;}
     name = item.getString("y6");
    for (int j = 0; j < name.length(); j++) { if(name.charAt(j)=='1') fluxels[j*ysize+6][i+1]=true;}
     name = item.getString("y7");
    for (int j = 0; j < name.length(); j++) { if(name.charAt(j)=='1') fluxels[j*ysize+7][i+1]=true;}
  
    }
  }
  changed=true;
}// fileLoad

void electrodesLoad()
{
  JSONArray electrodeJSON;
  electrodeJSON = loadJSONArray(arrayLayout);

  // Get the first array of elements
  //JSONArray values = json.getJSONArray(0);
  
  electrodes_loaded=electrodeJSON.size();
  
 // https://processing.org/examples/arrayobjects.html
  
  electrodeArray = new Electrode[electrodes_loaded];
  println("electrodes loded "+electrodes_loaded);
  for (int i = 0; i < electrodes_loaded; i++) 
  {
    JSONObject item = electrodeJSON.getJSONObject(i); 
    electrodeArray[i] = new Electrode(0,0,0,0,0);

    electrodeArray[i].x = item.getFloat("x");
    electrodeArray[i].y = item.getFloat("y");
    electrodeArray[i].h = item.getFloat("h");
    electrodeArray[i].v = item.getFloat("v");
    electrodeArray[i].e = item.getInt("e"); 
  }
}// electrodesLoad



void fillColorFlux(int x, int f)
{ 
  float red=0;
  float green=200;
  float blue=0;

  if (!fluxels[x][f]) 
     { if (fluxels[x][f-1]) red=210; else red=250;}
     if (fluxels_state[x]&feedback) {red=red*0.39+105;green=177;blue=106;}
     fill(red,green,blue);
}


int coX(float x)
{
return int((width/2)+x*(imgScale*eSize/2));
}

int coY(float y)
{
return int((img.height/2-imageShift)*imgScale+y*eSize/2*imgScale);
}


boolean inRect(float mx, float my, float x,float y, float h, float v)  
{
    return   ((mx>=x)&&(mx<x+h)&&(my>=y)&&(my<y+v));
}

int frameMax()
{
  int x=0;
  int z=max_frame_no;
   while (!fluxels[x][z]&&(z>1))
   {if (x<fluxel_number-1) x++;  else {x=0;z--;}}
  return z;
 }
 
 
int fluxValue(int y,int frame)
{
  int val=0;
  for (int j = 0; j < (xsize+2); j++) {
  val+=int(fluxels[(xsize+1-j)*ysize+(y)][frame])*pow(2,j);  };
  return val;
} 


class Electrode 
{
           float x;
           float y;
           float h;
           float v;
           int e;
    
    
    Electrode(float xpos,float ypos, float xsize, float ysize, int no)
    {
      x=xpos;
      y=ypos;
      h=xsize;
      v=ysize;
      e=no;
    }
    
    void drawElectrode()
    {
    rect(width/2+(x*eSize/2)*imgScale,img.height*imgScale/2-imgScale*imageShift+(y*eSize/2)*imgScale,(h*eSize/2)*imgScale,(v*eSize/2)*imgScale);
     fill(123, 123, 123);
    if (showNumbers) text(e,  width/2+(x*eSize/2)*imgScale+(h*eSize/5)*imgScale,img.height*imgScale/2-imgScale*imageShift+(y*eSize/2)*imgScale+(v*eSize/4)*imgScale);
    }
    
} //Electrode Object
