


#include <SPI.h>
#include <RH_RF69.h>
#include <Wire.h>
#include <RHReliableDatagram.h>
#define LED 13
#include <Adafruit_PWMServoDriver.h>
//#include <EasyTransferI2C.h>




// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

// our servo # counter
uint8_t servonum = 0;




//----------------------------------------Time--------------
unsigned long CurrentMillis = 0;
unsigned long PreviousDisplayMillis = 0;
unsigned long PreviousReadMillis = 0;
unsigned long PreviousSendMillis = 0;
unsigned long PreviousShowTimeMillis = 0;
unsigned long PreviousHistoryMillis = 0;
unsigned long PreviousRadioMillis = 0;
unsigned long PreviousCheckMillis = 0;
unsigned long PreviousStanceMillis = 0;
unsigned long PreviousMoveMillis = 0;
unsigned long PreviousShowMillis = 0;
unsigned long PreviousLoopMillis = 0;
unsigned long ShowTime = 1;
const long DisplayInterval = 1000;
const long ReadInterval = 100;
const long SendInterval = 200; 
const long ResetHistoryInterval = 250; 
const long RadioInterval = 250; 
const long CheckInterval = 500; 
const long StanceInterval = 100; 
const long MoveInterval = 75;
const long LoopInterval = 100;
const long ShowInterval = 75; 
const long ShowTimeInterval = 200; 
/************ Radio Setup ***************/


  #define RF69_FREQ 919.0
  #define RFM69_CS      8
  #define RFM69_INT     3
  #define RFM69_RST     4
// Where to send packets to!
#define DEST_ADDRESS 255

// who am i? (server address)
#define MY_ADDRESS 6

struct dataStruct{
byte n1;
byte n2;
byte n3;

}RadioPacket;

 byte buf[sizeof(RadioPacket)] = {0};
 
// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);
//-----------------------------Constant (Never Change)---------------------------

#define CPUTipRt 12
#define CPUTipLt 11
#define CPUTipPow 10    
#define ZapZap 9
//#define PANELCOUNT 16
//#define I2CADDRESS 6
//#define SERVOI2CADDRESS 0x40
#define OEPin A1   



const int UArm = 0;
const int LArm = 1;
const int Blank = 2;
const int Data = 3;
const int UCB = 4;
const int LCB = 5;
const int L1 = 6;
const int L2 = 7;
const int L3 = 8;
const int L4 = 9;
const int RLong = 10;
const int LLong = 11;
const int Blank2 = 12;
const int CPU = 13;
const int Zap = 14;
const int CPUTip = 15;

const int UArmC = 450;
const int UArmM = 350;
const int UArmO = 190;
const int LArmC = 460;
const int LArmM = 410;
const int LArmO = 290;
const int DataC = 450;
const int DataM = 390;
const int DataO = 320;
const int UCBC = 450;
const int UCBM = 390;
const int UCBO = 320;
const int LCBC = 240;
const int LCBM = 310;
const int LCBO = 380;
const int L1C = 210;
const int L1M = 290;
const int L1O = 380;

const int L2C = 470;
const int L2M = 330;
const int L2O = 200;
const int L3O = 450;
const int L3M = 375;
const int L3C = 300;
const int L4O = 430;
const int L4M = 330;
const int L4C = 230;
const int RLongC = 250;
const int RLongM = 325;
const int RLongO = 400;
const int LLongC = 260;
const int LLongM = 330;
const int LLongO = 390;
const int CPUAirIn = 450;
const int CPUAirOut = 340;
const int ZapAirIn = 320;
const int ZapAirOut = 430;
const int CPUExtOut = 200;
const int CPUExtIn = 500;
//============================== variable (will Change)==============
String Recieved;
String Incoming1;
String Incoming2;
String Incoming3;
String Incoming4;
String Incoming5;
String Incoming6;
String Incoming7;
String Incoming8;
String Incoming9;
String IncomingMsg;
int IncomingInt;
int LastNum;
int CurrentNumber;
int CurrentShow;
int Num3Int;
int CPUOut = 0;
int CPUWorking = 0;
int ZapOut = 0;
int Show;
//------------------------------------------------------------------------------------------------
//==========================================================Setup=============================



void setup(){
  delay(500);
  Serial.begin(115200);
 pinMode(8, INPUT_PULLUP);
 pwm.begin();  
 pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
 delay(10);
// Wire.begin();
pinMode( CPUTipLt, OUTPUT);
pinMode( CPUTipRt, OUTPUT );
pinMode( CPUTipPow, OUTPUT );
pinMode( ZapZap, OUTPUT );   
//pinMode(RadioInput,INPUT);
pinMode( OEPin, OUTPUT );   
digitalWrite(OEPin, HIGH);  
digitalWrite(CPUTipLt,HIGH);
digitalWrite(CPUTipRt,HIGH);
digitalWrite(CPUTipPow,HIGH);
digitalWrite(ZapZap,HIGH);
digitalWrite(OEPin,HIGH);   
//==================================================Radio==================================     
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x09};
  rf69.setEncryptionKey(key);
  
//  pinMode(LED, OUTPUT);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
RadioPacket.n1 = 0;
RadioPacket.n2 = 0;
RadioPacket.n3 = 0;

  delay(500);
digitalWrite(OEPin, LOW); 
delay(100);
 pwm.setPWM(0, 0, UArmC); 
 pwm.setPWM(1, 0, LArmC);
 pwm.setPWM(3, 0, DataC); 
 pwm.setPWM(4, 0, UCBC);
 pwm.setPWM(5, 0, LCBC); 
 pwm.setPWM(6, 0, L1C);
 pwm.setPWM(7, 0, L2C); 
 pwm.setPWM(8, 0, L3C);
 pwm.setPWM(9, 0, L4C);
 pwm.setPWM(10, 0, RLongC); 
 pwm.setPWM(11, 0, LLongC);
 delay(100);
digitalWrite(OEPin, HIGH);  




  
}
//---------------------------------------------------Wave 1-------------------------
//Clockwise around till all open, Then Closed in same order
void Wave1(){ 
 
    if (ShowTime >= 1 && ShowTime <= 24) {
     digitalWrite(OEPin, LOW);
  } 
if (ShowTime >= 4 && ShowTime <= 14) { 
  pwm.setPWM(10, 0, RLongO);  
}
if (ShowTime >= 6 && ShowTime <= 14) { 
  pwm.setPWM(4, 0, UCBO);
}
if (ShowTime >= 7 && ShowTime <= 14) { 
  pwm.setPWM(5, 0, LCBO);
}
if (ShowTime >= 8 && ShowTime <= 14) { 
   pwm.setPWM(6, 0, L1O);
}
if (ShowTime >= 9 && ShowTime <= 14) { 
   pwm.setPWM(7, 0, L2O); 
}
if (ShowTime >= 10 && ShowTime <= 14) { 
  pwm.setPWM(3, 0, DataO);  
}
if (ShowTime >= 11 && ShowTime <= 14) { 
 pwm.setPWM(8, 0, L3O);
}
if (ShowTime >= 12 && ShowTime <= 14) { 
 pwm.setPWM(9, 0, L4O);
}
if (ShowTime >= 13 && ShowTime <= 14) { 
 pwm.setPWM(11, 0, LLongO); 
}
if (ShowTime >= 14) { 
  pwm.setPWM(10, 0, RLongC);  
}
if (ShowTime >= 15) { 
  pwm.setPWM(4, 0, UCBC);
}
if (ShowTime >= 16) { 
  pwm.setPWM(5, 0, LCBC);
}
if (ShowTime >= 17) { 
   pwm.setPWM(6, 0, L1C);
}
if (ShowTime >= 18) { 
   pwm.setPWM(7, 0, L2C); 
}
if (ShowTime >= 19) { 
  pwm.setPWM(3, 0, DataC);  
}
if (ShowTime >= 20) { 
 pwm.setPWM(8, 0, L3C);
}
if (ShowTime >= 21) { 
 pwm.setPWM(9, 0, L4C);
}
if (ShowTime >= 22) { 
 pwm.setPWM(11, 0, LLongC); 
}

if (ShowTime >= 25) {
  digitalWrite(OEPin, HIGH);  
  Show = 0;
  Shows();
} 
}
//---------------------------------------------------Wave 2-------------------------
//open from center out, and then closed in same order
void Wave2(){
  if (ShowTime >= 1 && ShowTime <= 28) {
digitalWrite(OEPin, LOW);
  } 
if (ShowTime >= 0 && ShowTime <= 14) { 
  pwm.setPWM(0, 0, UArmO);  
}
if (ShowTime >= 1 && ShowTime <= 14) { 
  pwm.setPWM(1, 0, LArmO);
}
if (ShowTime >= 3 && ShowTime <= 14) { 
  pwm.setPWM(3, 0, DataO);
}
if (ShowTime >= 4 && ShowTime <= 14) { 
   pwm.setPWM(4, 0, UCBO);
}
if (ShowTime >= 5 && ShowTime <= 14) { 
   pwm.setPWM(5, 0, LCBO); 
}
if (ShowTime >= 6 && ShowTime <= 14) { 
  pwm.setPWM(6, 0, L1O);  
}
if (ShowTime >= 7 && ShowTime <= 14) { 
 pwm.setPWM(7, 0, L2O);
}
if (ShowTime >= 8 && ShowTime <= 14) { 
 pwm.setPWM(8, 0, L3O);
}
if (ShowTime >= 9 && ShowTime <= 14) { 
 pwm.setPWM(9, 0, L4O); 
}
if (ShowTime >= 10 && ShowTime <= 14)  { 
  pwm.setPWM(10, 0, RLongO);  
}
if (ShowTime >= 11 && ShowTime <= 14)  { 
  pwm.setPWM(11, 0, LLongO);
}
if (ShowTime >= 12 && ShowTime <= 14)  { 
 
}
if (ShowTime >= 14 && ShowTime <= 27) { 
   pwm.setPWM(0, 0, UArmC);
}
if (ShowTime >= 15 && ShowTime <= 27) { 
   pwm.setPWM(1, 0, LArmC); 
}
if (ShowTime >= 16 && ShowTime <= 27) { 
  pwm.setPWM(3, 0, DataC);  
}
if (ShowTime >= 17 && ShowTime <= 27) { 
 pwm.setPWM(4, 0, UCBC);
}
if (ShowTime >= 18 && ShowTime <= 27) { 
 pwm.setPWM(5, 0, LCBC);
}
if (ShowTime >= 19 && ShowTime <= 27) { 
 pwm.setPWM(6, 0, L1C); 
}
if (ShowTime >= 20 && ShowTime <= 27) {
pwm.setPWM(7, 0, L2C); 
}
if (ShowTime >= 21 && ShowTime <= 27) {
pwm.setPWM(8, 0, L3C); 
}
if (ShowTime >= 22 && ShowTime <= 27) {
pwm.setPWM(9, 0, L4C); 
}
if (ShowTime >= 23 && ShowTime <= 27) {
pwm.setPWM(10, 0, RLongC); 
}
if (ShowTime >= 24 && ShowTime <= 27) {
pwm.setPWM(11, 0, LLongC); 
}


if (ShowTime >= 28) {
  digitalWrite(OEPin, HIGH);  
  Show = 0;
  Shows();
} 
}
//---------------------------------------------Wave 3-------------------------
//All open, then close from center out
void Wave3(){
 if (ShowTime >= 1 && ShowTime <= 14) {
digitalWrite(OEPin, LOW);
  }
  if (ShowTime >= 1 && ShowTime <= 4) { //open all
  pwm.setPWM(0, 0, UArmO); 
 pwm.setPWM(1, 0, LArmO);
 pwm.setPWM(3, 0, DataO); 
 pwm.setPWM(4, 0, UCBO);
 pwm.setPWM(5, 0, LCBO); 
 pwm.setPWM(6, 0, L1O);
 pwm.setPWM(7, 0, L2O); 
 pwm.setPWM(8, 0, L3O);
 pwm.setPWM(9, 0, L4O);
 pwm.setPWM(10, 0, RLongO); 
 pwm.setPWM(11, 0, LLongO); 
  }
if (ShowTime >= 4 && ShowTime <= 14) { 
   pwm.setPWM(3, 0, DataC);
 
}
if (ShowTime >= 6 && ShowTime <= 14) { 
  pwm.setPWM(4, 0, UCBC);
}
if (ShowTime >= 7 && ShowTime <= 14) { 
  pwm.setPWM(5, 0, LCBC);
}
if (ShowTime >= 8 && ShowTime <= 14) { 
   pwm.setPWM(8, 0, L3C);
}
 if (ShowTime >= 9 && ShowTime <= 14) { 
   pwm.setPWM(1, 0, LArmC);
   pwm.setPWM(0, 0, UArmC);   
   pwm.setPWM(7, 0, L2C); 
}
if (ShowTime >= 10 && ShowTime <= 14) { 
   pwm.setPWM(6, 0, L1C);
}

if (ShowTime >= 11 && ShowTime <= 14) { 
   pwm.setPWM(9, 0, L4C);
}

if (ShowTime >= 11 && ShowTime <= 14) { 
 pwm.setPWM(10, 0, RLongC);  
}
if (ShowTime >= 12 && ShowTime <= 14) { 
 pwm.setPWM(11, 0, LLongC); 
}

if (ShowTime >= 20) {
  digitalWrite(OEPin, HIGH);  
  Show = 0;
  Shows();
} 
}
//----------------------------------------------------------Wave 4-----------------------
void Wave4(){
 if (ShowTime >= 1 && ShowTime <= 24) {
digitalWrite(OEPin, LOW);
  }

if (ShowTime >= 4 && ShowTime <= 14) { 
   pwm.setPWM(3, 0, DataO);
 
}
if (ShowTime >= 6 && ShowTime <= 14) { 
  pwm.setPWM(4, 0, UCBO);
}
if (ShowTime >= 7 && ShowTime <= 14) { 
  pwm.setPWM(5, 0, LCBO);
}
if (ShowTime >= 8 && ShowTime <= 14) { 
   pwm.setPWM(8, 0, L3O);
}
 if (ShowTime >= 9 && ShowTime <= 14) { 
  
   pwm.setPWM(7, 0, L2O); 
}
if (ShowTime >= 10 && ShowTime <= 14) { 
   pwm.setPWM(6, 0, L1O);
}
if (ShowTime >= 11 && ShowTime <= 14) { 
 pwm.setPWM(10, 0, RLongO);  
}
if (ShowTime >= 12 && ShowTime <= 14) { 
 pwm.setPWM(11, 0, LLongO); 
}

if (ShowTime >= 13 && ShowTime <= 22) { 
   pwm.setPWM(3, 0, DataC);
 
}
if (ShowTime >= 14 && ShowTime <= 22) { 
  pwm.setPWM(4, 0, UCBC);
}
if (ShowTime >= 15 && ShowTime <= 22) { 
  pwm.setPWM(5, 0, LCBC);
}
if (ShowTime >= 16 && ShowTime <= 22) { 
   pwm.setPWM(8, 0, L3C);
}
 if (ShowTime >= 17 && ShowTime <= 22) {   
   pwm.setPWM(7, 0, L2C); 
}
if (ShowTime >= 18 && ShowTime <= 22) { 
   pwm.setPWM(6, 0, L1C);
}
if (ShowTime >= 19 && ShowTime <= 22) { 
 pwm.setPWM(10, 0, RLongC);  
}
if (ShowTime >= 20 && ShowTime <= 22) { 
 pwm.setPWM(11, 0, LLongC); 
}

if (ShowTime >= 24) {
  digitalWrite(OEPin, HIGH);  
  Show = 0;
  Shows();
} 
}
//---------------------------------------------------Scared-------------------------

void Scared(){
 if (ShowTime >= 1 && ShowTime <= 17) {
digitalWrite(OEPin, LOW);
  } 
if (ShowTime >= 3 && ShowTime <= 4) { 
AllPanelsOpen();
}
if (ShowTime >= 4 && ShowTime <= 5) { 
AllPanelsMid();
}
if (ShowTime >= 5 && ShowTime <= 6) { 
AllPanelsOpen();
}
if (ShowTime >= 6 && ShowTime <= 7) { 
AllPanelsMid();
}
if (ShowTime >= 7 && ShowTime <= 8) { 
AllPanelsOpen();
}
if (ShowTime >= 8 && ShowTime <= 9) { 
AllPanelsMid();
}
if (ShowTime >= 9 && ShowTime <= 10) { 
AllPanelsOpen();
}
if (ShowTime >= 10 && ShowTime <= 11) { 
AllPanelsMid();
}
if (ShowTime >= 11 && ShowTime <= 12) { 
AllPanelsOpen();
}
if (ShowTime >= 12 && ShowTime <= 13) { 
AllPanelsMid();
}
if (ShowTime >= 13 && ShowTime <= 14) { 
AllPanelsOpen();
}
if (ShowTime >= 14 && ShowTime <= 18) { 
AllPanelsClosed();
}
if (ShowTime >= 18) {
  digitalWrite(OEPin, HIGH);  
  CurrentShow = 0;
  Shows();
} 
}

//---------------------------------------------------Leia Speach-------------------------
void LeiaSpeach(){
  if (ShowTime >= 1 && ShowTime <= 174) {
digitalWrite(OEPin, LOW);
  pwm.setPWM(5, 0, LCBO);
  } 

if (ShowTime >= 170)  { 
  
  pwm.setPWM(5, 0, LCBC);
}


if (ShowTime >= 175) {
  digitalWrite(OEPin, HIGH);  
 Show = 0;
  Shows();
} 
}
//-------------------------------------------------Open CPU-----------------------
void OpenCPU(){
   if (ShowTime >= 1 && ShowTime <= 8) {
digitalWrite(OEPin, LOW);
   }
      if (ShowTime >= 1 && ShowTime <= 8) {
   pwm.setPWM(10, 0, RLongO);
  } 
if (ShowTime >= 4 && ShowTime <= 6) { 
  pwm.setPWM(13, 0, CPUAirOut);
}
if (ShowTime >= 7)  { 
 digitalWrite(OEPin, HIGH);
 
 Show = 0;
  Shows();
}}
//-------------------------------------------------Close CPU-----------------------
void CloseCPU(){  
   if (ShowTime >= 1 && ShowTime <= 8) {
digitalWrite(OEPin, LOW);
   }
  if (ShowTime >= 1 && ShowTime <= 3.9) {
    pwm.setPWM(10, 0, RLongO);
   pwm.setPWM(13, 0, CPUAirIn);
  } 
if (ShowTime >= 4 && ShowTime <= 6) { 
    pwm.setPWM(10, 0, RLongC);
}
if (ShowTime >= 7)  { 
 digitalWrite(OEPin, HIGH);
 Show = 0;
  Shows();
}} 

//-------------------------------------------------Open Zap-----------------------
void OpenZap(){
   if (ShowTime >= 1 && ShowTime <= 6) {
digitalWrite(OEPin, LOW);
   }
      if (ShowTime >= 3 && ShowTime <= 6) {
   pwm.setPWM(11, 0, LLongO);
  } 
if (ShowTime >= 5 && ShowTime <= 6) { 
  pwm.setPWM(14, 0, ZapAirOut);
}
if (ShowTime >= 7)  { 
 digitalWrite(OEPin, HIGH);

Show = 0;
  Shows();
}}
//-------------------------------------------------Close Zap-----------------------
void CloseZap(){
   if (ShowTime >= 1 && ShowTime <= 15) {
digitalWrite(OEPin, LOW);
   }
 if (ShowTime >= 1 && ShowTime <= 4) {
  pwm.setPWM(11, 0, LLongO);
 }
  if (ShowTime >= 5 && ShowTime <= 11) {
   pwm.setPWM(14, 0, ZapAirIn);
  } 
if (ShowTime >= 12 && ShowTime <= 15) { 
    pwm.setPWM(11, 0, LLongC);
}
if (ShowTime >= 16)  { 
 digitalWrite(OEPin, HIGH);

 Show = 0;
  Shows();
}} 
//-------------------------------------------------Zaping-----------------------
void Zaping(){
    if (ShowTime >= 1 && ShowTime <= 6) {
      digitalWrite(OEPin, LOW);
    }
     if (ShowTime >= 1 && ShowTime <= 4) {
 pwm.setPWM(0, 0, UArmC); 
 pwm.setPWM(1, 0, LArmC);
 pwm.setPWM(3, 0, DataC); 
 pwm.setPWM(4, 0, UCBC);
 pwm.setPWM(5, 0, LCBC); 
 pwm.setPWM(6, 0, L1C);
 pwm.setPWM(7, 0, L2C); 
 pwm.setPWM(8, 0, L3C);
 pwm.setPWM(9, 0, L4C);
 pwm.setPWM(10, 0, RLongC); 
 pwm.setPWM(11, 0, LLongO); 
digitalWrite(ZapZap, LOW);
delay (400);  
 digitalWrite(ZapZap, HIGH);
 delay (200);    
 digitalWrite(ZapZap, LOW);
delay (400);  
 digitalWrite(ZapZap, HIGH); 
    }
     if (ShowTime >= 5);  {
      digitalWrite(OEPin, HIGH);
       Show = 0;
      Shows();
}} 
//-------------------------------------------------Work CPU-----------------------
void WorkCPU(){
 
  
   if (ShowTime >= 1 && ShowTime <= 46) {
digitalWrite(OEPin, LOW);
   }
 if (ShowTime >= 1 && ShowTime <= 15) {
digitalWrite(CPUTipPow, LOW);
 pwm.setPWM(12, 0, CPUExtOut);
   }
 if (ShowTime >= 16 && ShowTime <= 17) {
 digitalWrite(OEPin, HIGH); 
digitalWrite(CPUTipPow, HIGH);
 }
   if (ShowTime >= 18 && ShowTime <= 20) { 
   digitalWrite(CPUTipRt, LOW);
   digitalWrite(CPUTipLt, HIGH);
  } 
    if (ShowTime >= 21 && ShowTime <= 23) { 
   digitalWrite(CPUTipRt, LOW);
   digitalWrite(CPUTipLt, LOW);
  } 
    if (ShowTime >= 24 && ShowTime <= 26) { 
   digitalWrite(CPUTipRt, LOW);
   digitalWrite(CPUTipLt, HIGH);
  } 
     if (ShowTime >= 27 && ShowTime <= 29) { 
   digitalWrite(CPUTipRt, LOW);
   digitalWrite(CPUTipLt, LOW);
  } 
       if (ShowTime >= 30) { 
   digitalWrite(CPUTipRt, HIGH);
   digitalWrite(CPUTipLt, HIGH);
  } 
if (ShowTime >= 31 && ShowTime <= 46) {
  digitalWrite(OEPin, LOW);
  digitalWrite(CPUTipPow, LOW); 
  pwm.setPWM(12, 0, CPUExtIn);
}
if (ShowTime >= 47)  {
  digitalWrite(CPUTipPow, HIGH); 
  digitalWrite(OEPin, HIGH);

 Show = 0;
  Shows();
}} 
//-------------------------------------------------Rocket Man-----------------------
void RocketMan(){
   if (ShowTime >= 1 && ShowTime <= 410) {
digitalWrite(OEPin, LOW);
 } 
 pwm.setPWM(0, 0, UArmC); 
 pwm.setPWM(1, 0, LArmC);
 pwm.setPWM(3, 0, DataC); 
 pwm.setPWM(4, 0, UCBC);
 pwm.setPWM(5, 0, LCBC); 
 pwm.setPWM(6, 0, L1C);
 pwm.setPWM(7, 0, L2C); 
 pwm.setPWM(8, 0, L3C);
 pwm.setPWM(9, 0, L4C);
 pwm.setPWM(10, 0, RLongC); 
 pwm.setPWM(11, 0, LLongC); 
  if (ShowTime >= 20 && ShowTime <= 30) {
   pwm.setPWM(0, 0, UArmM);
  } 
if (ShowTime >= 31 && ShowTime <= 40) { 
  pwm.setPWM(1, 0, LArmM);
  pwm.setPWM(0, 0, UArmC);
}
if (ShowTime >= 41 && ShowTime <= 50) { 
  pwm.setPWM(1, 0, LArmC);
  pwm.setPWM(0, 0, UArmM);
}
if (ShowTime >= 51 && ShowTime <= 60) { 
  pwm.setPWM(1, 0, LArmM);
  pwm.setPWM(0, 0, UArmC);
}
if (ShowTime >= 61 && ShowTime <= 70) { 
  pwm.setPWM(1, 0, LArmC);
  pwm.setPWM(0, 0, UArmM);
}
if (ShowTime >= 71 && ShowTime <= 80) { 
  pwm.setPWM(1, 0, LArmM);
  pwm.setPWM(0, 0, UArmC);
}
if (ShowTime >= 81 && ShowTime <= 90) { 
  pwm.setPWM(1, 0, LArmM);
  pwm.setPWM(0, 0, UArmO);
}
if (ShowTime >= 91 && ShowTime <= 100) { 
  pwm.setPWM(1, 0, LArmO);
  pwm.setPWM(0, 0, UArmM);
}
if (ShowTime >= 101 && ShowTime <= 110) { 
  pwm.setPWM(1, 0, LArmM);
  pwm.setPWM(0, 0, UArmO);
}
if (ShowTime >= 111 && ShowTime <= 120) { 
  pwm.setPWM(1, 0, LArmO);
  pwm.setPWM(0, 0, UArmM);
}
if (ShowTime >= 121 && ShowTime <= 130) { 
  pwm.setPWM(1, 0, LArmM);
  pwm.setPWM(0, 0, UArmO);
}
if (ShowTime >= 131 && ShowTime <= 140) { 
  pwm.setPWM(1, 0, LArmO);
  pwm.setPWM(0, 0, UArmM);
}
if (ShowTime >= 141 && ShowTime <= 150) { 
  pwm.setPWM(1, 0, LArmM);
  pwm.setPWM(0, 0, UArmO);
}
if (ShowTime >= 151 && ShowTime <= 160) { 
  pwm.setPWM(1, 0, LArmO);
  pwm.setPWM(0, 0, UArmM);
}
if (ShowTime >= 161 && ShowTime <= 170) { 
  pwm.setPWM(1, 0, LArmM);
  pwm.setPWM(0, 0, UArmO);
}
if (ShowTime >= 171 && ShowTime <= 370) { 
  pwm.setPWM(1, 0, LArmO);
  pwm.setPWM(0, 0, UArmO);
}
if (ShowTime >= 371 && ShowTime <= 389) { 
  pwm.setPWM(1, 0, LArmC);
  pwm.setPWM(0, 0, UArmC);
}
if (ShowTime >= 390)  { 
digitalWrite(OEPin,HIGH);
 Show = 0;
  Shows();
}}
//-------------------------------------------------Fav Things-----------------------
void FavThings(){
   if (ShowTime >= 1 && ShowTime <= 200) {
digitalWrite(OEPin, LOW);
 }  
  if (ShowTime >= 2 && ShowTime <= 10) {
   pwm.setPWM(0, 0, UArmO);
  } 
if (ShowTime >= 11 && ShowTime <= 20) { 
  pwm.setPWM(1, 0, LArmO);
  pwm.setPWM(0, 0, UArmC);
}
if (ShowTime >= 21 && ShowTime <= 30) { 
  pwm.setPWM(1, 0, LArmC);
  pwm.setPWM(0, 0, UArmO);
}
if (ShowTime >= 31 && ShowTime <= 40) { 
  pwm.setPWM(1, 0, LArmO);
  pwm.setPWM(0, 0, UArmC);
}
if (ShowTime >= 41 && ShowTime <= 50) { 
  pwm.setPWM(1, 0, LArmC);
  pwm.setPWM(0, 0, UArmO);
}
if (ShowTime >= 51 && ShowTime <= 60) { 
  pwm.setPWM(1, 0, LArmO);
  pwm.setPWM(0, 0, UArmC);
}
if (ShowTime >= 61 && ShowTime <= 70) { 
  pwm.setPWM(1, 0, LArmC);
  pwm.setPWM(0, 0, UArmO);
}
if (ShowTime >= 71 && ShowTime <= 80) { 
  pwm.setPWM(1, 0, LArmO);
  pwm.setPWM(0, 0, UArmC);
}
if (ShowTime >= 81 && ShowTime <= 90) { 
  pwm.setPWM(1, 0, LArmC);
  pwm.setPWM(0, 0, UArmC);
}
if (ShowTime >= 75 && ShowTime <= 110) { 
  pwm.setPWM(7, 0, L2O);
}
if (ShowTime >= 100 && ShowTime <= 110) { 
   pwm.setPWM(7, 0, L2C);
  pwm.setPWM(6, 0, L1O);
}
if (ShowTime >= 131 && ShowTime <= 140) { 
  pwm.setPWM(7, 0, L2C);
  pwm.setPWM(6, 0, L1O);
  pwm.setPWM(8, 0, L3O);
}
if (ShowTime >= 141 && ShowTime <= 161) { 
  pwm.setPWM(7, 0, L2C);
  pwm.setPWM(6, 0, L1O);
  pwm.setPWM(8, 0, L3O);
  pwm.setPWM(9, 0, L4O);
}
if (ShowTime >=161 && ShowTime <= 173) { 
  pwm.setPWM(7, 0, L2C);
  pwm.setPWM(6, 0, L1C);
  pwm.setPWM(8, 0, L3C);
  pwm.setPWM(9, 0, L4C);
}
if (ShowTime >= 180)  { 
digitalWrite(OEPin,HIGH);
 Show = 0;
  Shows();
}}
//_________________________________________________SHOWS________________________________
void Shows(){
  if (Show == 0){
  return;
}
switch(Show){
 case 91://Rocket Man
 Serial.print(" Rocket Man ");
  Serial.println(ShowTime);
  RocketMan();
  break;
case 92://Leia Speach
 Serial.print(" Leia Speach ");
  Serial.println(ShowTime);
  LeiaSpeach();
  break;
  case 93://R2 Awesome
 Serial.print(" R2 Awesome ");
  Serial.println(ShowTime);
//  R2Awesome();
  break;
 case 94://R2 Matcho Man
 Serial.print(" Matcho Man ");
  Serial.println(ShowTime);
 // Matcho();
  break;


   case 95://Fav Things
 Serial.print(" Fav Things ");
  Serial.println(ShowTime);
  FavThings();
  break; 
    case 96://Cantina
 Serial.print(" Cantina ");
  Serial.println(ShowTime);
 // Cantina();
  break; 
    case 97://Low Rider
 Serial.print("Low Rider ");
  Serial.println(ShowTime);
 // LowRider();
  break; 
  case 172:// WAVE 1
  Serial.print(" Wave 1 ");
  Serial.println(ShowTime);
 Wave1();
break;

  case 173:// WAVE 3
  Serial.print(" Wave 3 ");
  Serial.println(ShowTime);
 Wave3();
break;

  case 174:// Alt1
  Serial.print(" Alt 1 ");
  Serial.println(ShowTime);
 Scared();
break;
   case 176:// WAVE 2
  Serial.print(" Wave 2 ");
  Serial.println(ShowTime);
 Wave2();
break;
 case 177:// WAVE 4
  Serial.print(" Wave 4 ");
  Serial.println(ShowTime);
 Wave4();
break;
 case 178:// WAVE Alt2
  Serial.print(" Alt 2 ");
  Serial.println(ShowTime);
// Alt2();
break;
 case 192:// WAVE 1
  Serial.print(" Wave 1 ");
  Serial.println(ShowTime);
 Wave1();
break;

  case 193:// WAVE 3
  Serial.print(" Wave 3 ");
  Serial.println(ShowTime);
 Wave3();
break;

  case 194:// Alt1
  Serial.print(" Alt 1 ");
  Serial.println(ShowTime);
 Scared();
break;

  case 196:// WAVE 3
  Serial.print(" Wave 3 ");
  Serial.println(ShowTime);
 Wave3();
break;
 case 197:// WAVE 4
  Serial.print(" Wave 4 ");
  Serial.println(ShowTime);
 Wave4();
break;
 case 198:// WAVE Alt2
  Serial.print(" Alt 2 ");
  Serial.println(ShowTime);
// Alt2();
break;
  case 71:// Open Cpu
  Serial.print(" Open CPU ");
  Serial.println(ShowTime);
 OpenCPU();
break;
 case 72:// Work Cpu
  Serial.print(" Work CPU ");
  Serial.println(ShowTime);
 WorkCPU();
break;
 case 75:// Close Cpu
  Serial.print(" Close CPU ");
  Serial.println(ShowTime);
 CloseCPU();
break;

   case 73://Open Zap
 Serial.print(" Open Zap ");
  Serial.println(ShowTime);
  OpenZap();
  break;   
   case 74://Zap Zap
 Serial.print(" Zap Zap ");
  Serial.println(ShowTime);
  Zaping();
  break;  
   case 77://Close Zap
 Serial.print(" Close Zap ");
  Serial.println(ShowTime);
  CloseZap();
  break; 
 
 default:
 case 00:// No Show
 break;
}}
 

  

//////////////////////////////////////////////////////////////////////////////////////////
///*****                     Dome Panel Movement Functions                        *****///
//////////////////////////////////////////////////////////////////////////////////////////

void closeAllPanels() {
  if (ShowTime <= 3) { 
digitalWrite(OEPin, LOW); 
  }
  Serial.println("CLOSE ALL");
delay(100);
 pwm.setPWM(0, 0, UArmC);
 pwm.setPWM(1, 0, LArmC);
 pwm.setPWM(3, 0, DataC); 
 pwm.setPWM(4, 0, UCBC);
 pwm.setPWM(5, 0, LCBC); 
 pwm.setPWM(6, 0, L1C);
 pwm.setPWM(7, 0, L2C); 
 pwm.setPWM(8, 0, L3C);
 pwm.setPWM(9, 0, L4C);
 pwm.setPWM(10, 0, RLongC); 
 pwm.setPWM(11, 0, LLongC);
 delay(500);
  if (ShowTime >= 4) {
digitalWrite(OEPin, HIGH);  
}
}
void openAllPanels() {
   if (ShowTime <= 3) {
digitalWrite(OEPin, LOW); 
   }
  Serial.println("OPEN ALL");

 pwm.setPWM(0, 0, UArmO); 
 pwm.setPWM(1, 0, LArmO);
 pwm.setPWM(3, 0, DataO); 
 pwm.setPWM(4, 0, UCBO);
 pwm.setPWM(5, 0, LCBO); 
 pwm.setPWM(6, 0, L1O);
 pwm.setPWM(7, 0, L2O); 
 pwm.setPWM(8, 0, L3O);
 pwm.setPWM(9, 0, L4O);
 pwm.setPWM(10, 0, RLongO); 
 pwm.setPWM(11, 0, LLongO);
 delay(500);
  if (ShowTime >= 4) {
digitalWrite(OEPin, HIGH);  
}}

void AllPanelsOpen(){
pwm.setPWM(0, 0, UArmO); 
 pwm.setPWM(1, 0, LArmO);
 pwm.setPWM(3, 0, DataO); 
 pwm.setPWM(4, 0, UCBO);
 pwm.setPWM(5, 0, LCBO); 
 pwm.setPWM(6, 0, L1O);
 pwm.setPWM(7, 0, L2O); 
 pwm.setPWM(8, 0, L3O);
 pwm.setPWM(9, 0, L4O);
 pwm.setPWM(10, 0, RLongO); 
 pwm.setPWM(11, 0, LLongO);
 }
void AllPanelsMid() {
  pwm.setPWM(0, 0, UArmM); 
 pwm.setPWM(1, 0, LArmM);
 pwm.setPWM(3, 0, DataM); 
 pwm.setPWM(4, 0, UCBM);
 pwm.setPWM(5, 0, LCBM); 
 pwm.setPWM(6, 0, L1M);
 pwm.setPWM(7, 0, L2M); 
 pwm.setPWM(8, 0, L3M);
 pwm.setPWM(9, 0, L4M);
 pwm.setPWM(10, 0, RLongM); 
 pwm.setPWM(11, 0, LLongM);
 }
void AllPanelsClosed(){
 pwm.setPWM(0, 0, UArmC);
 pwm.setPWM(1, 0, LArmC);
 pwm.setPWM(3, 0, DataC); 
 pwm.setPWM(4, 0, UCBC);
 pwm.setPWM(5, 0, LCBC); 
 pwm.setPWM(6, 0, L1C);
 pwm.setPWM(7, 0, L2C); 
 pwm.setPWM(8, 0, L3C);
 pwm.setPWM(9, 0, L4C);
 pwm.setPWM(10, 0, RLongC); 
 pwm.setPWM(11, 0, LLongC); 
}
//-----------------------------------------Apply Incoming Message---------------------
void ApplyIncoming(){

  
//  Serial.print (IncomingInt);
//   Serial.println (CurrentNumber);
   
switch(CurrentNumber){
   case 91://Rocket Man
   Show = 91;
    Shows();
    break;
  case 92://LeiaHolo
    Show = 92;
    Shows();
    break;
    case 93://R2Awe
    Show = 93;
    Shows();
    break;
    case 94://Matcho
    Show = 94;
    Shows();
    break;
    case 95://FavThings
    Show = 95;
    Shows();
    break;
    case 96://Cantina
    Show = 96;
    Shows();
    break;
    case 97: //Low Rider
    Show = 97;
    Shows();
    break;
  case 171://Open All
  openAllPanels();
    break;
  case 172://Wave 1
    Show = 172;
    Shows();
    break;
    case 173://Wave 3
    Show = 173;
    Shows();
    break;
    case 174:// Alt1
    Show = 174;
    Shows();
    break;
    case 175://Close all
    closeAllPanels();
     break;
     case 176://Wave 2
     Show = 176;
     Shows();
     break;
     case 177:
    Show = 177;
    Shows();
     break;
     case 178:
     Show = 178;
     Shows();
     break;
    case 191://Open All
  openAllPanels();
    break;
  case 192://Wave 1
    Show = 192;
    Shows();
    break;
    case 193://Wave 3
    Show = 193;
    Shows();
    break;
    case 194:// Alt1
    Show = 194;
    Shows();
    break;
    case 195://Close all
    closeAllPanels();
     break;
     case 196://Wave 2
     Show = 196;
     Shows();
     break;
     case 197:
    Show = 197;
    Shows();
     break;
     case 198:
     Show = 198;
     Shows();
     break;
 
 case 71://Open CPU
 Show = 71;
  Shows();
 break;
  case 72://Work CPU
 Show = 72;
 Shows();
 break;   
  case 75://Close CPU
 Show = 75;
  Shows();
 break;

  case 74://Zap Zap
   Show = 74;
   Shows();
    break;
  case 73://Open Zap
  Show = 73;
  Shows();
    break;

  
  case 77://Close Zap
  Show = 77;
  Shows();
    break;   
}

}
 
  


//--------------------------------------------------We Got One-----------------
void WeGotOne(){
Serial.println("WE GOT ONE");
CurrentNumber = IncomingInt;
 ShowTime = 0;
 LastNum = CurrentNumber;
 ApplyIncoming(); 
    
}
//-------------------------------------------------Radio-------------------------------
void Radio(){
  if (rf69_manager.available()) {
    // Wait for a message addressed to us from the client
    uint8_t buf[sizeof(RadioPacket)];
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (rf69_manager.recvfromAckTimeout(buf, &len,2000, &from))
    {
      memcpy(&RadioPacket, buf, sizeof(RadioPacket));
      if (!len) return;
      buf[len] = 0;
      Incoming1 = String(RadioPacket.n1);
      Incoming2 = String(RadioPacket.n2);
      Incoming3 = String(RadioPacket.n3);
     

IncomingMsg = Incoming1 + Incoming2 + Incoming3;
   
IncomingInt = IncomingMsg.toInt();
Num3Int = Incoming3.toInt();
//Serial.print("Incoming3: ");
//Serial.println(Incoming3);
if (1 <= Num3Int && Num3Int <=8){
WeGotOne();
    }
  }
}
}

//---------------------------------------------------Debug Radio
void DebugRadio(){
     Serial.print("RSSI: ");
      Serial.println(rf69.lastRssi(), DEC);

    Serial.print(" Incoming Int ");
   Serial.println(IncomingInt);
    Serial.print(" LastNum ");
   Serial.println(LastNum);
    Serial.print(" CurrentNumber ");
   Serial.println(CurrentNumber);
     Serial.print(" CurrentShow ");
   Serial.println(Show);
}     
      
    





void loop(){ 

//const int RLongC = 280;
//const int RLongO = 450;
//digitalWrite(OEPin, LOW); 
 // pwm.setPWM(10, 0, 250); 
//  delay(1000);
 //  pwm.setPWM(10, 0, 400);
//   delay(2000);  
   CurrentMillis = millis();
  
 
    if (millis() - PreviousDisplayMillis >= DisplayInterval){
    PreviousDisplayMillis = CurrentMillis;
    DebugRadio();
    }
  
    if (millis() - PreviousRadioMillis >= RadioInterval){
    PreviousRadioMillis = CurrentMillis;
    Radio();
    ApplyIncoming();
    }

   if (millis() - PreviousLoopMillis >= LoopInterval){
    PreviousLoopMillis = CurrentMillis;
    Shows();
      }
     if (millis() - PreviousShowTimeMillis >= ShowTimeInterval){
            PreviousShowTimeMillis = CurrentMillis;
            ShowTime++;
     }
     
  delay(100);
}
int mapPulselength(double microseconds) {
  double pulselength;
  int pulse;
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  pulselength /= 4096;  // 12 bits of resolution
  pulse = microseconds/pulselength;
  return pulse;
}
