/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 09/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - Configures a Arduino M0 Pro to act as an I2C slave and prints the received I2C
  payload\buffer to the Serial Monitor. Uses the standard M0 Pro I2C port.

  Designed to be used with test program '60_I2C_Transmit_to_Slave_Test' which transmits a 6 byte payload,
  the first byte is the address being sent to, then ASCII characters '0' to '4'. Sample printout below.

  12 Process buffer > 18 30 31 32 33 34
  13 Process buffer > 18 30 31 32 33 34
  
  Note that if this slave program is running and an I2C scanner is run on the board there will be a device
  detected on the Slave_Address used and 0x28 which is the Arduino Zero's EDBG which is connected as a 
  slave to the processor.

  Serial monitor baud rate is set at 9600.

*******************************************************************************************************/

#include <Wire.h>
#include <Arduino.h>

#define LED1 13                           //on board LED on M0 PRo
#define Slave_Address 0x18                //slave address for this board

#define ADAPTER_IODIR 0x00
#define ADAPTER_GPIO 0x01
#define ADAPTER_GETTEMP_H_1 0x02
#define ADAPTER_GETTEMP_L_1 0x03
#define ADAPTER_GETTEMP_H_2 0x04
#define ADAPTER_GETTEMP_L_2 0x05
#define ADAPTER_GETTEMP_H_3 0x06
#define ADAPTER_GETTEMP_L_3 0x07
#define ADAPTER_SETTEMP_H_1 0x08
#define ADAPTER_SETTEMP_L_1 0x09
#define ADAPTER_SETTEMP_H_2 0x0A
#define ADAPTER_SETTEMP_L_2 0x0B
#define ADAPTER_SETTEMP_H_3 0x0C
#define ADAPTER_SETTEMP_L_3 0x0D



#define STBY_regpos         6
#define MODE_regpos         4
#define BOOST_regpos        5
#define PHASE_A_regpos      1
#define ENABLE_A_regpos     0
#define PHASE_B_regpos      3
#define ENABLE_B_regpos     2

#define stby_Pin      10
#define mode_Pin      12
#define boostPin      5
#define phaseA_Pin    7
#define enableA_Pin   8
#define phaseB_Pin    11
#define enableB_Pin   6

#define PIDp 0.015
#define PIDi 0.000035//0.0001
#define PIDd 0.0007

uint16_t I2CmessagesCount;
uint8_t I2Cbuffer[64];                    //buffer for received I2C payload
boolean I2CbufferReady = false;
uint8_t I2CbytesReceived;
uint8_t ReceivedAddr=0;
uint8_t ReceivedData=0;


float TEMPset1;
float TEMPdif1;
float TEMPi1;
bool PIDIntegration1 = true;
float TEMPcontrol1;
float TEMPdif_a1[10];
float TEMPd1;

float TEMPset2;
float TEMPdif2;
float TEMPi2;
bool PIDIntegration2 = true;
float TEMPcontrol2;
float TEMPdif_a2[10];
float TEMPd2;

float TEMPset3;
float TEMPdif3;
float TEMPi3;
bool PIDIntegration3 = true;
float TEMPcontrol3;
float TEMPdif_a3[10];
float TEMPd3;


long TEMPclick=0;

int IODIR_byte;
int GPIO_byte;

const int TemperatureSensor1 = A1;  // Analog input pin that the temperature sensor is attached to
const int TemperatureSensor2 = A2;  // Analog input pin that the temperature sensor is attached to
const int TemperatureSensor3 = A3;  // Analog input pin that the temperature sensor is attached to

const int heaterPin1 = 9;
const int heaterPin2 = 4;
const int heaterPin3 = 3;



int pp;

int outputValue = 0;        // value output to the PWM (heater)

float temperature_mean1 =0;
float temperature_mean2 =0;
float temperature_mean3 =0;
float temperature1 = 0;
float temperature2 = 0;
float temperature3 = 0;

float getTemperature(int analogInPin)

{
const int NTC_B = 3435;
const float NTC_TN = 298.15;
const int NTC_RN = 10000;
const float NTC_R0 = 4.7;

int sensorValue;
float sensorVoltage;
float sensorResistance;
float temperature;


  sensorValue = analogRead(analogInPin);
  sensorVoltage=3.3*sensorValue/4096;
  sensorResistance=((sensorVoltage*NTC_R0)/(3.3-sensorVoltage));
  temperature=  1/(log(sensorResistance*1000/NTC_RN)/NTC_B+1/NTC_TN)-273.15 ;
  
return temperature;
};

void setHeater(float temperature,float power,int heaterPin)
{
const int t0=25;
const int tmax=130;
int pinPower;


  if (temperature>tmax) temperature=tmax;
  if (temperature<t0) temperature=t0;
  
if (power>0){
  pinPower=constrain(255*(power/(1-(temperature-t0)/(tmax-t0))), 0, 255);
   analogWrite(heaterPin,pinPower);
   pp=pinPower;
  } else
  { 
     pinPower=constrain(255*(1+power)*((temperature-t0-20)/(tmax-t0)), 0, 255);
    // pinPower=0;
      analogWrite(heaterPin,pinPower);
      pp=pinPower;
    }

  };

void runPID()
{
   TEMPcontrol1= PIDp*TEMPdif1+PIDd*TEMPd1+(int)PIDIntegration1*PIDi*TEMPi1;
  setHeater(temperature_mean1, TEMPcontrol1,heaterPin1);

   TEMPcontrol2= PIDp*TEMPdif2+PIDd*TEMPd2+(int)PIDIntegration2*PIDi*TEMPi2;
  setHeater(temperature_mean2, TEMPcontrol2,heaterPin2);

   TEMPcontrol3= PIDp*TEMPdif3+PIDd*TEMPd3+(int)PIDIntegration3*PIDi*TEMPi3;
  setHeater(temperature_mean3, TEMPcontrol3,heaterPin3);
  };


void setup()
{
  

  pinMode(heaterPin1, OUTPUT);
  pinMode(heaterPin2, OUTPUT);
  pinMode(heaterPin3, OUTPUT);

  digitalWrite(heaterPin1, LOW);
  digitalWrite(heaterPin2, LOW);
  digitalWrite(heaterPin3, LOW);


      

  Wire.begin(Slave_Address);
  Wire.onReceive(receiveEvent);        //data from slave recieved
  Wire.onRequest(requestEvent);        //data from slave recieved

  configureAdc();
  analogReadResolution(12);
  pinMode(17, OUTPUT);


}

void loop()
{

  TEMPdif1=TEMPset1-temperature_mean1;
 // if (abs(TEMPdif)<10) PIDIntegration=true; else {PIDIntegration=false;TEMPi=0;};
  TEMPi1=TEMPi1+(TEMPset1-temperature_mean1);
  if (TEMPi1>8000)TEMPi1=8000;
   if (TEMPi1<-8000)TEMPi1=-8000;

  TEMPdif2=TEMPset2-temperature_mean2;
 // if (abs(TEMPdif)<10) PIDIntegration=true; else {PIDIntegration=false;TEMPi=0;};
  TEMPi2=TEMPi2+(TEMPset2-temperature_mean2);
  if (TEMPi2>8000)TEMPi2=8000;
   if (TEMPi2<-8000)TEMPi2=-8000;

   TEMPdif3=TEMPset3-temperature_mean3;
 // if (abs(TEMPdif)<10) PIDIntegration=true; else {PIDIntegration=false;TEMPi=0;};
  TEMPi3=TEMPi3+(TEMPset3-temperature_mean3);
  if (TEMPi3>8000)TEMPi3=8000;
   if (TEMPi3<-8000)TEMPi3=-8000;
   
  runPID();
  
  if (millis()-TEMPclick>200) {
  TEMPclick=millis();
  TEMPd1=TEMPdif_a1[4]-TEMPset1-temperature1;
  TEMPdif_a1[4]=TEMPdif_a1[3];
  TEMPdif_a1[3]=TEMPdif_a1[2];
  TEMPdif_a1[2]=TEMPdif_a1[1];
  TEMPdif_a1[1]=TEMPdif_a1[0];
  TEMPdif_a1[0]=TEMPdif1;
 
  TEMPd2=TEMPdif_a2[4]-TEMPset2-temperature2;
  TEMPdif_a2[4]=TEMPdif_a2[3];
  TEMPdif_a2[3]=TEMPdif_a2[2];
  TEMPdif_a2[2]=TEMPdif_a2[1];
  TEMPdif_a2[1]=TEMPdif_a2[0];
  TEMPdif_a2[0]=TEMPdif2;
 
  TEMPd3=TEMPdif_a3[4]-TEMPset3-temperature3;
  TEMPdif_a3[4]=TEMPdif_a3[3];
  TEMPdif_a3[3]=TEMPdif_a3[2];
  TEMPdif_a3[2]=TEMPdif_a3[1];
  TEMPdif_a3[1]=TEMPdif_a3[0];
  TEMPdif_a3[0]=TEMPdif3;
  
  }

 temperature1=getTemperature(TemperatureSensor1);
 temperature_mean1= (temperature_mean1*39+temperature1)/40;
 temperature2=getTemperature(TemperatureSensor2);
 temperature_mean2= (temperature_mean2*39+temperature2)/40;
 temperature3=getTemperature(TemperatureSensor3);
 temperature_mean3= (temperature_mean3*39+temperature3)/40;

 delay(5);
}





void receiveEvent(int howMany)
{
  uint8_t index;
  uint8_t receiveptr = 0;

  I2CbufferReady = false;               //buffer not saved yet  
  I2CbytesReceived = howMany;           //save the numebr of bytes available

if (howMany==2) {
  ReceivedAddr = Wire.read();

  switch (ReceivedAddr){
  case ADAPTER_SETTEMP_H_1:
    TEMPset1 = Wire.read();
        break;


  case ADAPTER_SETTEMP_H_2:
    TEMPset2 = Wire.read();
        break;


  case ADAPTER_SETTEMP_H_3:
    TEMPset3 = Wire.read();
        break;

  case ADAPTER_IODIR:
    IODIR_byte = Wire.read();

    pinMode(stby_Pin, !boolean(IODIR_byte&1<<STBY_regpos));
    pinMode(mode_Pin, !boolean(IODIR_byte&1<<MODE_regpos));
    pinMode(boostPin, !boolean(IODIR_byte&1<<BOOST_regpos));
    pinMode(phaseA_Pin, !boolean(IODIR_byte&1<<PHASE_A_regpos));
    pinMode(enableA_Pin, !boolean(IODIR_byte&1<<ENABLE_A_regpos));
    pinMode(phaseB_Pin, !boolean(IODIR_byte&1<<PHASE_B_regpos));      
    pinMode(enableB_Pin, !boolean(IODIR_byte&1<<ENABLE_B_regpos));
         
        break;

  case ADAPTER_GPIO:
    GPIO_byte = Wire.read();

    digitalWrite(stby_Pin, boolean(GPIO_byte&1<<STBY_regpos));
    digitalWrite(mode_Pin, boolean(GPIO_byte&1<<MODE_regpos));
    digitalWrite(boostPin, boolean(GPIO_byte&1<<BOOST_regpos));
    digitalWrite(phaseA_Pin, boolean(GPIO_byte&1<<PHASE_A_regpos));
    digitalWrite(enableA_Pin, boolean(GPIO_byte&1<<ENABLE_A_regpos));
    digitalWrite(phaseB_Pin, boolean(GPIO_byte&1<<PHASE_B_regpos));      
    digitalWrite(enableB_Pin, boolean(GPIO_byte&1<<ENABLE_B_regpos));

    
        break;
        
        
    default:
   Wire.read();
    break;

        
    }
    } else
{
    ReceivedAddr = Wire.read();
  };
  
  

}

void requestEvent() {

switch (ReceivedAddr){
  case ADAPTER_GETTEMP_H_1:
  Wire.write(int(temperature_mean1));
    break;

   case ADAPTER_GETTEMP_H_2:
  Wire.write(int(temperature_mean2));
    break;

   case ADAPTER_GETTEMP_H_3:
  Wire.write(int(temperature_mean3));
    break;

     case ADAPTER_GETTEMP_L_1:
  Wire.write(int((temperature_mean1-floor(temperature_mean1))*100));
    break;

   case ADAPTER_GETTEMP_L_2:
  Wire.write(int((temperature_mean2-floor(temperature_mean2))*100));
    break;

   case ADAPTER_GETTEMP_L_3:
  Wire.write(int((temperature_mean3-floor(temperature_mean3))*100));
    break;

    default:
    Wire.write(0);
    break;
    
}
   

}





  

// setting AnalogRead parameters , code by adamgarbo 
void configureAdc()
{
  ADC->CTRLA.bit.ENABLE = 0;                      // Disable ADC
  ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV512 |   // Divide Clock ADC GCLK by 512 (48MHz/512 = 93.7kHz)
                   ADC_CTRLB_RESSEL_10BIT;        // Set ADC resolution to 10 bits
  
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization
  ADC->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(63);   // Set Sampling Time Length (341.33 us)
  ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1 |  // Configure multisampling
                     ADC_AVGCTRL_ADJRES(0);       // Configure averaging
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization
  ADC->CTRLA.bit.ENABLE = 1;                      // Enable ADC
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  // Apply ADC gain and offset error calibration correction
   //analogReadCorrection(44, 2065); //CORREN = 1
}
