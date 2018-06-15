
import processing.serial.*;
import processing.net.*; 

import static javax.swing.JOptionPane.*;

final int hdispY_pos=307;
final boolean debug = true;

Serial myPort;  // Create object from Serial class
float val;      // Data received from the serial port
PImage img;  // Declare variable "a" of type PImage



float time = 1000;
float timeincrement = 0.01;
int xsize = 14;
int ysize = 8;
int posX = 2024;
int posY = 2024;
int init_flags=1;
int displayX=250;
int displayY=50;

boolean invert=true;

final int box0_size=10;
final int boxX0=20;
final int boxY0=hdispY_pos+124;

int frame_max=1;

int box1=1;
int box1_size=5;
int boxX1=20;
int boxY1=hdispY_pos+320;

int interval_ms=500;
int butX=180;
final int butY=hdispY_pos+240;

color  rectColor = color(0,0,0);
color  rectHighlight = color(0,180,0);
int boxSize = 21;     // Diameter of rect


int csize = 10;
float[][] data = new float[xsize+1][ysize+1];
float[] control_data_in = new float[csize];
float[] control_data_out = new float[csize];
boolean[][][] fluxels = new boolean[xsize][ysize][101];

int time_start=0;

int wSize_x=1600;
int wSize_y=1000;
int eSize=46;
int cSize=30;

boolean play=false;
boolean life=true;
boolean cont_flag=false;
int frame_no=1;

float imgScale=0.9;

PFont f;
boolean changed=true;

JSONArray values;
  
void setup() {

control_data_out[0]=xsize;

printArray(PFont.list());
  f = createFont("Ubuntu Medium", 32);
  textFont(f);
  
  
  size(1800, 1050, P3D);
surface.setResizable(true);
  img = loadImage("OpenDropFrame.png");  // Load the image into the program  




 String COMx, COMlist = "";
/*
  Other setup code goes here - I put this at
  the end because of the try/catch structure.
*/
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
      showMessageDialog(frame,"Device is not connected to the PC");
      exit();
    }
  }
  catch (Exception e)
  { //Print the type of error
    showMessageDialog(frame,"COM port is not available.\n(maybe in use by another program)");
    println("Error:", e);
    exit();
  }



/*
 String portName = Serial.list()[0];
 printArray(Serial.list());
 myPort = new Serial(this, portName, 115200);
*/
 
 init_flags=0;
 control_data_out[6]=init_flags;
 } //setup


int x = 0;
int y = 0;


void draw() {
  background(255,250,240);
  stroke(0);
 // fill(100,100);

    update(mouseX, mouseY);
    
   // if (mousePressed) print("hell");
    
// rotateY((float)mouseX/100);

//

/*
if ( myPort.available() >= ((xsize*2)+(csize*2))) {

 for (int i=0; i < csize; i++){
   val = myPort.read();         // read Control Data
  val = val+myPort.read()*256;
  control_data_in[i] = val;
 }
 
for (int i=0; i < xsize; i++){

  val = myPort.read();         // read Image Data
  val = val+myPort.read()*256;

  data[xsize-i][y] = (val);
//println(val);
     }


y++;

while ( myPort.available() >0) {val = myPort.read();}; 

 control_data_out[5]=chanel;
 control_data_out[7]=laser;
 control_data_out[4]=fact;
 control_data_out[2]=posX;
 control_data_out[3]=posY;
 control_data_out[6]=init_flags;
 if (init_flags==1) init_flags=0;
 
 for (int i=0; i < csize; i++){
  myPort.write(byte(control_data_out[i]/256)); 
  myPort.write(byte(control_data_out[i]%256)); 

 }

 
 
if (y>ysize) {
  y=ysize;
  drawPlane(data, xsize, ysize,ysize/2);
saveFrame("dump/LaserDump "+nf(year(),4)+"-"+nf(month(),2)+"-"+nf(day(),2)+" "+nf(hour(),2)+":"+nf(minute(),2)+":"+nf(second(),2)+".png");
y=0; 
}

} //myPort Available
 */
 /*
 for (int x=0; x < 14; x++)
 for (int y=0; y < 8; y++)
 {
  myPort.write(int(true)); 
 }
 */
 
 if (play&&(millis()-time_start>interval_ms))
 {
   time_start=millis();
 if (frame_no<frame_max) frame_no++; else frame_no=1;
 changed=true;}
 
if (changed&&life) 
{
changed=false;

for (int x=0; x < 16; x++)
 for (int y=0; y < 8; y++)
 {
   if((x>0)&(x<15))
  myPort.write(int(fluxels[x-1][y][frame_no])); 
  else myPort.write(0);
  
 }
 }

drawPlane(data, xsize, ysize,y-1);
  //delay (1000);
} //draw

void mousePressed() {
  //int mx=mouseX;
  //int my=mouseY;
//println(floor((mx-(wSize_x/2-xsize*0.5*eSize))/eSize));
//println(floor((my-(80+img.height*imgScale*0.5-ysize*0.5*eSize))/eSize));
if ((cont_flag)&(frame_no<100)) frame_no++;
  int mx=floor((mouseX-(width/2-xsize*0.5*eSize))/eSize);
  int my=floor((mouseY-(80+img.height*imgScale*0.5-ysize*0.5*eSize))/eSize);

if ((mx>=0 & mx<xsize)&(my>=0 & my<ysize))
 if ((fluxels[mx][my][frame_no]==false)|(cont_flag)) fluxels[mx][my][frame_no]=true; else fluxels[mx][my][frame_no]=false;

changed=true;

 if (overRect(int(width/2-xsize*0.5+650),int(80+img.height*imgScale*0.5-ysize*0.5+320),40,40)) 
 {print("clear");
   fill(255, 255, 255);
 rect(width/2-xsize*0.5+650,80+img.height*imgScale*0.5-ysize*0.5+320,40,40);
//boolean[][][] fluxels = new boolean[xsize][ysize][101];
 for (int x=0; x < xsize; x++)
 for (int y=0; y < ysize; y++)
  for (int z=0; z < 101; z++)
  fluxels[x][y][z]=false;
frame_no=1;
 }
 
 if(overRect(int(width/2-xsize*0.5+650),int(80+img.height*imgScale*0.5-ysize*0.5+220),120,40))
 {
  if (play) play=false; else {
  play=true;  time_start=millis(); 
  int x=0;
  int y=0;
  int z=100;
 while (!fluxels[x][y][z]&&(z>1))
  {if (x<xsize-1) x++; else {x=0; if (y<ysize-1) y++; else {y=0;z--;}}}
  frame_max=z;
  }
 }
 
  if(overRect(int(width/2-xsize*0.5+650),int(80+img.height*imgScale*0.5-ysize*0.5+150),120,40))
 {
  if (life) life=false; else {
  life=true;  
  }
 }
 
 

 if(overRect(int(width/2-xsize*0.5+650),int(80+img.height*imgScale*0.5-ysize*0.5-420),80,40))
{
println("load");
 selectInput("Select a file to process:", "fileLoad");

}   
 
 if(overRect(int(width/2-xsize*0.5+750),int(80+img.height*imgScale*0.5-ysize*0.5-420),80,40))
{
println("save");
 selectOutput("Select desination;", "fileSaved");
 
}   



}

void keyPressed() {
if  ((keyCode == RIGHT)&(frame_no<100)) frame_no++;
if  ((keyCode == LEFT)&(frame_no>1)) frame_no--;
if (keyCode==SHIFT) cont_flag=true;
changed=true;
}

void keyReleased()
{
  if (keyCode==SHIFT) cont_flag=false;
  changed=true;
}

void mouseWheel(MouseEvent event) {
  float e = event.getCount();
  //println(e);
 if (play)
 {float v=(exp((log(interval_ms)*10+e)/10));
 if((v>50)&(v<3000)) interval_ms=int(v);
 //interval_ms=int(v);
 println(interval_ms);
 } else 
 {
  if ((e==-1)&(frame_no<100)) frame_no++;
  if ((e==1)&(frame_no>1)) frame_no--;
  changed=true;
 }
}



void drawPlane(float[][] vals, int sizeX, int sizeY,int indexY) {

       fill(255, 255, 255);


 
 
   image(img, width/2-img.width*imgScale*0.5, 80, img.width*imgScale, img.height*imgScale);

      stroke(255,255,255);
     // rect(displayX,displayY,sizeX*rectsize,sizeY*rectsize);
      
       fill(255, 200, 0);
        textSize(11);
       strokeWeight(4);

  for (int i=0; i<sizeX; i++) 
    for (int j=0; j<sizeY; j++) {
      if (fluxels[i][j][frame_no]) {fill(0, 200, 0);} else 
        { if (fluxels[i][j][frame_no-1]) {fill(210, 200, 0);} else fill(255, 200, 0);}
 rect(width/2-sizeX*0.5*eSize+i*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+j*eSize,eSize,eSize);
 }

 rect(width/2-sizeX*0.5*eSize+(-1)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+1*eSize,eSize,eSize);
 rect(width/2-sizeX*0.5*eSize+(-1)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+6*eSize,eSize,eSize);
 rect(width/2-sizeX*0.5*eSize+(14)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+1*eSize,eSize,eSize);
 rect(width/2-sizeX*0.5*eSize+(14)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+6*eSize,eSize,eSize);
 
 rect(width/2-sizeX*0.5*eSize+(-1.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+0*eSize,eSize,eSize);
 rect(width/2-sizeX*0.5*eSize+(-1.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+2*eSize,eSize,eSize);
 rect(width/2-sizeX*0.5*eSize+(-1.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+5*eSize,eSize,eSize);
 rect(width/2-sizeX*0.5*eSize+(-1.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+7*eSize,eSize,eSize);
 rect(width/2-sizeX*0.5*eSize+(14.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+0*eSize,eSize,eSize);
 rect(width/2-sizeX*0.5*eSize+(14.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+2*eSize,eSize,eSize);
 rect(width/2-sizeX*0.5*eSize+(14.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+5*eSize,eSize,eSize);
 rect(width/2-sizeX*0.5*eSize+(14.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+7*eSize,eSize,eSize);

 rect(width/2-sizeX*0.5*eSize+(-2.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+1*eSize,eSize*1.5,eSize);
 rect(width/2-sizeX*0.5*eSize+(-2.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+6*eSize,eSize*1.5,eSize);
 rect(width/2-sizeX*0.5*eSize+(15)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+1*eSize,eSize*1.5,eSize);
 rect(width/2-sizeX*0.5*eSize+(15)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+6*eSize,eSize*1.5,eSize);
 
 rect(width/2-sizeX*0.5*eSize+(-2.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+1*eSize,eSize*1.5,eSize);
 rect(width/2-sizeX*0.5*eSize+(-2.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+6*eSize,eSize*1.5,eSize);
 rect(width/2-sizeX*0.5*eSize+(15)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+1*eSize,eSize*1.5,eSize);
 rect(width/2-sizeX*0.5*eSize+(15)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+6*eSize,eSize*1.5,eSize);
 
 rect(width/2-sizeX*0.5*eSize+(-3.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+0*eSize,eSize*2,eSize);
 rect(width/2-sizeX*0.5*eSize+(-3.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+2*eSize,eSize*2,eSize);
 rect(width/2-sizeX*0.5*eSize+(-3.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+5*eSize,eSize*2,eSize);
 rect(width/2-sizeX*0.5*eSize+(-3.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+7*eSize,eSize*2,eSize);
 rect(width/2-sizeX*0.5*eSize+(15.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+0*eSize,eSize*2,eSize);
 rect(width/2-sizeX*0.5*eSize+(15.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+2*eSize,eSize*2,eSize);
 rect(width/2-sizeX*0.5*eSize+(15.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+5*eSize,eSize*2,eSize);
 rect(width/2-sizeX*0.5*eSize+(15.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+7*eSize,eSize*2,eSize);
 //strokeWeight(0);
 rect(width/2-sizeX*0.5*eSize+(-3.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+0*eSize,eSize,eSize*3);
 rect(width/2-sizeX*0.5*eSize+(-3.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+5*eSize,eSize,eSize*3);
 rect(width/2-sizeX*0.5*eSize+(16.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+0*eSize,eSize,eSize*3);
 rect(width/2-sizeX*0.5*eSize+(16.5)*eSize,80+img.height*imgScale*0.5-sizeY*0.5*eSize+5*eSize,eSize,eSize*3);

  textSize(32);
  fill(255, 255, 255);
  text("frame_no:", width/2-400, 840) ; 
  text(frame_no, width/2-240,840) ;    
  
    if (play)  fill(255, 255, 255); else fill(255, 0, 0);
 rect(width/2-sizeX*0.5+650,80+img.height*imgScale*0.5-sizeY*0.5+220,40,40);
   fill(255, 255, 255);
        text("play", width/2-sizeX*0.5+700,80+img.height*imgScale*0.5-sizeY*0.5+250) ;    

    if (life)  fill(255, 255, 255); else fill(255, 0, 0);
 rect(width/2-sizeX*0.5+650,80+img.height*imgScale*0.5-sizeY*0.5+150,40,40);
   fill(255, 255, 255);
        text("life", width/2-sizeX*0.5+700,80+img.height*imgScale*0.5-sizeY*0.5+190) ;    



  fill(255, 0, 0);
 rect(width/2-sizeX*0.5+650,80+img.height*imgScale*0.5-sizeY*0.5+320,40,40);
  fill(255, 255, 255);
        text("clear all", width/2-sizeX*0.5+700,80+img.height*imgScale*0.5-sizeY*0.5+350) ;    

    fill(255, 255, 255);
        text("load", width/2-sizeX*0.5+650,80+img.height*imgScale*0.5-sizeY*0.5-380) ;    

     fill(255, 255, 255);
        text("save", width/2-sizeX*0.5+750,80+img.height*imgScale*0.5-sizeY*0.5-380) ;    

   
   
          fill(150, 150, 150);

      for (int i=1; i<20; i++) 
     {
    if(frame_no==i)              fill(255, 0, 0);    else         fill(150, 150, 150);

     ellipse (width/2-10*cSize*1.3+i*cSize*1.3,910,cSize,cSize);
      
    }


}


/*
void mousePressed() {
  float rectsize = max(800, 800)/max(xsize, ysize);
   
  if (overRect(boxX0, boxY0, boxSize*box0_size, boxSize))  box0=checkRadio(boxX0,boxY0,box0_size);
  if (overRect(boxX1, boxY1, boxSize*box1_size, boxSize))  box1=checkRadio(boxX1,boxY1,box1_size);
  if (overRect(boxX2, boxY2, boxSize*box2_size, boxSize))  box2=checkRadio(boxX2,boxY2,box2_size);
  if (overRect(boxX3, boxY3, boxSize*box3_size, boxSize))  box3=checkRadio(boxX3,boxY3,box3_size);
  if (overRect(boxX4, boxY4, boxSize*box4_size, boxSize))  box4=checkRadio(boxX4,boxY4,box4_size);
  if (overRect(boxX5, boxY5, boxSize*box5_size, boxSize))  box5=checkRadio(boxX5,boxY5,box5_size);
  if (overRect(boxX6, boxY6, boxSize*box6_size, boxSize))  box6=checkRadio(boxX6,boxY6,box6_size);

  if (overRect(displayX,displayY,int(xsize*rectsize),int(ysize*rectsize)))
  {
  posX=int(posX-fact*((mouseX-displayX)/(rectsize)-xsize/2));
  posY=int(posY+fact*((mouseY-displayY)/(rectsize)-ysize/2));
  }
   if (overRect(butX,butY,30,30)) {
     y=0;
     init_flags=1;
   }
  
  
 // println((mouseX-displayX)/(rectsize))
}

*/

int checkRadio(int boxX,int boxY, int box_size)
{int a=0;
 for (int i=0; i < box_size; i++){
    if (overRect(boxX+i*boxSize,boxY,boxSize,boxSize)) a=i ;
 }
 radioBut(a,boxX,boxY,box_size);

 return a;
}


void fileSaved(File selection) {
  if (selection == null) {
    println("Window was closed or the user hit cancel.");
  } else {
    println("User selected " + selection.getAbsolutePath());
 
  println(frameMax());
  values = new JSONArray();

for (int i = 1; i < frameMax()+1; i++) {

    JSONObject fluxel_jason = new JSONObject();

    fluxel_jason.setInt("frame", i);
    fluxel_jason.setString("y0", binary(fluxValue(0,i),16));
    fluxel_jason.setString("y1", binary(fluxValue(1,i),16));
    fluxel_jason.setString("y2", binary(fluxValue(2,i),16));
    fluxel_jason.setString("y3", binary(fluxValue(3,i),16));
    fluxel_jason.setString("y4", binary(fluxValue(4,i),16));
    fluxel_jason.setString("y5", binary(fluxValue(5,i),16));
    fluxel_jason.setString("y6", binary(fluxValue(6,i),16));
    fluxel_jason.setString("y7", binary(fluxValue(7,i),16));

    values.setJSONObject(i-1, fluxel_jason);
  }
  
  saveJSONArray(values, selection.getAbsolutePath()+".json");

}
} //fileSave

void fileLoad(File selection) {
  if (selection == null) {
    println("Window was closed or the user hit cancel.");
  } else {
    println("User selected " + selection.getAbsolutePath());
 
  
  
  values = loadJSONArray(selection.getAbsolutePath());

  // Get the first array of elements
  //JSONArray values = json.getJSONArray(0);
  
  for (int i = 0; i < values.size(); i++) {
    
    JSONObject item = values.getJSONObject(i); 

    String name = item.getString("y0");
  for (int j = 0; j < xsize; j++) { if(name.charAt(15-j)=='1') fluxels[xsize-j-1][0][i+1]=true;}
   name = item.getString("y1");
  for (int j = 0; j < xsize; j++) { if(name.charAt(15-j)=='1') fluxels[xsize-j-1][1][i+1]=true;}
   name = item.getString("y2");
  for (int j = 0; j < xsize; j++) { if(name.charAt(15-j)=='1') fluxels[xsize-j-1][2][i+1]=true;}
     name = item.getString("y3");
  for (int j = 0; j < xsize; j++) { if(name.charAt(15-j)=='1') fluxels[xsize-j-1][3][i+1]=true;}
     name = item.getString("y4");
  for (int j = 0; j < xsize; j++) { if(name.charAt(15-j)=='1') fluxels[xsize-j-1][4][i+1]=true;}
     name = item.getString("y5");
  for (int j = 0; j < xsize; j++) { if(name.charAt(15-j)=='1') fluxels[xsize-j-1][5][i+1]=true;}
     name = item.getString("y6");
  for (int j = 0; j < xsize; j++) { if(name.charAt(15-j)=='1') fluxels[xsize-j-1][6][i+1]=true;}
     name = item.getString("y7");
  for (int j = 0; j < xsize; j++) { if(name.charAt(15-j)=='1') fluxels[xsize-j-1][7][i+1]=true;}
  
  
    
  }
   
  }
  changed=true;
}// fileLoad


void radioBut(int box,int boxX,int boxY, int box_size)
{
 for (int i=0; i < box_size; i++){
  
    if (i==box) fill(rectHighlight); else fill(rectColor);
   rect(boxX+i*boxSize, boxY, boxSize, boxSize);

 }
}

int frameMax()

{
  int x=0;
  int y=0;
  int z=100;
 while (!fluxels[x][y][z]&&(z>1))
 {if (x<xsize-1) x++; else {x=0; if (y<ysize-1) y++; else {y=0;z--;}}}
 return z;
 
 }
 
 

int fluxValue(int y,int frame)
{
  int val=0;
  for (int j = 0; j < xsize; j++) {
  val+=int(fluxels[xsize-j-1][y][frame])*pow(2,j);  };
  return val;
  
} 


boolean overRect(int x, int y, int width, int height)  {
  if (mouseX >= x && mouseX <= x+width && 
      mouseY >= y && mouseY <= y+height) {
    return true;
  } else {
    return false;
  }
}

void update(int x, int y) {
 // if (overRect(100, 100, boxSize, boxSize)) {
  // println("go");
 // }
}