
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
int fluxel_number = 128;

int xsize = 16;
int ysize = 8;
int posX = 2024;
int posY = 2024;
int init_flags=1;
int displayX=250;
int displayY=55;
int electrodes_loaded=0;

boolean invert=true;

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
int butX=180;
final int butY=hdispY_pos+240;

color  rectColor = color(0,0,0);
color  rectHighlight = color(0,180,0);
int boxSize = 21;     // Diameter of rect


int csize = 10;
int max_frame_no = 200;
float[][] data = new float[xsize][ysize+1];
float[] control_data_in = new float[csize];
float[] control_data_out = new float[csize];
boolean[][] fluxels = new boolean[fluxel_number][max_frame_no+1];

int time_start=0;

int wSize_x=1600;
int wSize_y=1000;
int eSize=46;
int cSize=30;

int imageShift=47;

boolean play=false;
boolean life=true;
boolean cont_flag=false;
int frame_no=1;

float imgScale=1;

PFont f;
boolean changed=true;

JSONArray values;

  int x = 0;
  int y = 0;

int last_w=0;
int last_h=0;

Electrode[] electrodeArray;



void setup() {

  electrodesLoad();
  control_data_out[0]=xsize;

// printArray(PFont.list());
  f = createFont("Ubuntu Medium", 32);
  textFont(f);
  
 // pixelDensity(2);
  size(1024, 768, P2D); //???
  surface.setResizable(true);
  surface.setSize(1024, 768);
  surface.setLocation(100,100);
      
  interval_ms= int(60000/interval);
  img = loadImage("OpenDropFrame.png");  // Load the image into the program  


// Open Serial Port
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


   init_flags=0;
   control_data_out[6]=init_flags;
   
   

 } //setup



void draw() {
    stroke(0);
 // fill(100,100);

 
// *** Play 
  if (play&&(millis()-time_start>interval_ms))
    {
      time_start=millis();
      if (frame_no<frame_max) frame_no++; else frame_no=1;
      changed=true;
    }
    
 // ** Write to device
if (changed&&life) 
    {
    changed=false;
    for (int x=0; x < fluxel_number; x++)
    myPort.write(int(fluxels[x][frame_no])); 
     }
 
   drawPlane(data, xsize, ysize,y-1);





} //draw


void drawPlane(float[][] vals, int sizeX, int sizeY,int indexY) {

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
  text("play", coX(34),coY(7.5));    
  text("life", coX(34),coY(11.5));  
  text("clear all", coX(34),coY(15.5));  
  text("save", coX(32),coY(-18.5));  
  text("load", coX(38),coY(-18.5));     
  text("frame_no:", coX(-20),coY(16)) ; 
  text("frames/min:", coX(+10),coY(16)) ; 


  
  stroke(255,255,255);
  // rect(displayX,displayY,sizeX*rectsize,sizeY*rectsize);
  fill(255, 200, 0);
  textSize(11);
  strokeWeight(6*imgScale);
  
// draw electrodes
  for (int i=0; i<electrodes_loaded; i++) 
  {
     fillColorFlux(electrodeArray[i].e,frame_no); 
     electrodeArray[i].draw();
  }

// draw buttons
  int boxSize=int(imgScale*eSize);
 
  if (play)  fill(255, 255, 255); else fill(255, 0, 0);
  rect(coX(31),coY(6),boxSize,boxSize);
  
  if (life)  fill(255, 255, 255); else fill(255, 0, 0);
  rect(coX(31),coY(10),boxSize,boxSize);
  
  fill(255, 0, 0);
  rect(coX(31),coY(14),boxSize,boxSize);
  
  fill(150, 150, 150);

// draw frame graphics
  for (int i=1; i<20; i++) 
    {
     if(frame_no==i)  fill(255, 0, 0);  else   fill(150, 150, 150);
     ellipse (coX(-20+i*2),coY(19.5),cSize*imgScale,cSize*imgScale); 
    }
      
      
} // drawPlain



void mousePressed() {

  if ((cont_flag)&(frame_no<max_frame_no)) frame_no++;
  
  int mx=floor((mouseX-(width/2)+100*eSize*imgScale)/(imgScale*eSize/2))-200;
  int my=floor((mouseY-(img.height/2-imageShift)*imgScale+100*eSize*imgScale)/(imgScale*(eSize/2)))-200;

  if ((mx>-22)&&(mx<22)&&(my>-10)&&(my<10))
    {
     for (int i=0; i<electrodes_loaded; i++) 
       {
        if (inRect(mx,my,electrodeArray[i].x,electrodeArray[i].y,electrodeArray[i].h,electrodeArray[i].v))
         {
           if ((fluxels[electrodeArray[i].e][frame_no]==false)|(cont_flag)) fluxels[electrodeArray[i].e][frame_no]=true; else fluxels[electrodeArray[i].e][frame_no]=false;
          }
        }
     }

changed=true;

// check life button
  if(inRect(mx,my,31,10,2,2))
     {
      if (life) life=false; else 
        {
      life=true;  
       }
     }
     
// check play button
  if  (inRect(mx,my,31,6,2,2))
     {
      if (play) play=false; else 
      {
      play=true;  time_start=millis(); 
      frame_max=frameMax();
      }
     }

// check clear button
  if (inRect(mx,my,31,14,2,2))
     { print("clear");
       fill(255, 255, 255);
        rect(coX(31),coY(14),eSize*imgScale,eSize*imgScale);
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

} // mouse pressed


void keyPressed() 
  {
    if  ((keyCode == RIGHT)&(frame_no<max_frame_no)) frame_no++;
    if  ((keyCode == LEFT)&(frame_no>1)) frame_no--;
    if (keyCode==SHIFT) cont_flag=true;
    changed=true;
  } // keyPressed

void keyReleased()
  {
    if (keyCode==SHIFT) cont_flag=false;
    changed=true;
  } //keyReleased

void mouseWheel(MouseEvent event) {
  float e = event.getCount();
  //println(e);
  if (play)
   {
   interval=int(interval-e*5);
   if (interval<5) interval=5;
   if (interval>600) interval=600;
   interval_ms= int(60000/interval);
    //float v=(exp((log(interval_ms)*10+e)/10));
   // if((v>50)&(v<3000)) interval_ms=int(v);
    //interval_ms=int(v);
    println(interval_ms);
   } else 
   {
    if ((e==-1)&(frame_no<max_frame_no)) frame_no++;
    if ((e==1)&(frame_no>1)) frame_no--;
    changed=true;
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
    for (int j = 0; j < xsize; j++) { if(name.charAt(j)=='1') fluxels[j*ysize+0][i+1]=true;}
     name = item.getString("y1");
    for (int j = 0; j < xsize; j++) { if(name.charAt(j)=='1') fluxels[j*ysize+1][i+1]=true;}
     name = item.getString("y2");
    for (int j = 0; j < xsize; j++) { if(name.charAt(j)=='1') fluxels[j*ysize+2][i+1]=true;}
     name = item.getString("y3");
    for (int j = 0; j < xsize; j++) { if(name.charAt(j)=='1') fluxels[j*ysize+3][i+1]=true;}
     name = item.getString("y4");
    for (int j = 0; j < xsize; j++) { if(name.charAt(j)=='1') fluxels[j*ysize+4][i+1]=true;}
     name = item.getString("y5");
    for (int j = 0; j < xsize; j++) { if(name.charAt(j)=='1') fluxels[j*ysize+5][i+1]=true;}
     name = item.getString("y6");
    for (int j = 0; j < xsize; j++) { if(name.charAt(j)=='1') fluxels[j*ysize+6][i+1]=true;}
     name = item.getString("y7");
    for (int j = 0; j < xsize; j++) { if(name.charAt(j)=='1') fluxels[j*ysize+7][i+1]=true;}
  
    }
  }
  changed=true;
}// fileLoad

void electrodesLoad()
{
  JSONArray electrodeJSON;
  electrodeJSON = loadJSONArray("electrodes.json");

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

    electrodeArray[i].x = item.getInt("x");
    electrodeArray[i].y = item.getInt("y");
    electrodeArray[i].h = item.getInt("h");
    electrodeArray[i].v = item.getInt("v");
    electrodeArray[i].e = item.getInt("e"); 
  }
}// electrodesLoad



void fillColorFlux(int x, int f)
{ 
  if (fluxels[x][f]) {fill(0, 200, 0);} else 
     { if (fluxels[x][f-1]) {fill(210, 200, 0);} else fill(255, 200, 0);}
}


int coX(float x)
{
return int((width/2)+x*(imgScale*eSize/2));
}

int coY(float y)
{
return int((img.height/2-imageShift)*imgScale+y*eSize/2*imgScale);
}


boolean inRect(int mx, int my, int x,int y, int h, int v)  
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
  for (int j = 0; j < xsize; j++) {
  val+=int(fluxels[(xsize-1-j)*ysize+(y)][frame])*pow(2,j);  };
  return val;
} 


class Electrode 
{
           int x;
           int y;
           int h;
           int v;
           int e;
    
    
    Electrode(int xpos,int ypos, int xsize, int ysize, int no)
    {
      x=xpos;
      y=ypos;
      h=xsize;
      v=ysize;
      e=no;
    }
    
    void draw()
    {
    rect(width/2+(x*eSize/2)*imgScale,img.height*imgScale/2-imgScale*imageShift+(y*eSize/2)*imgScale,(h*eSize/2)*imgScale,(v*eSize/2)*imgScale);
    }
    
} //Electrode Object
