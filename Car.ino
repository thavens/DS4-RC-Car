#include <Wire.h>

#define MotorSpeedSet             0x82
#define PWMFrequenceSet           0x84
#define DirectionSet              0xaa
#define MotorSetA                 0xa1
#define MotorSetB                 0xa5
#define Nothing                   0x00
#define Stepernu                  0x1c
#define F_31372Hz                 0x04
#define Rear                     0x0f
#define Front                      0x0a
#define RearPWM 3
#define FrontPWM 5
#define RDirPin 6
#define FDirPin 7

uint8_t OldDir = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(31372);
  frequence(F_31372Hz, Rear);
  frequence(F_31372Hz, Front);
  pinMode(RearPWM,INPUT);
  pinMode(FrontPWM,INPUT);
  pinMode(RDirPin, INPUT);
  pinMode(FDirPin, INPUT);
  checkDir(0x0a);
  Serial.println("here we go");
}

void loop() {
  if (Serial.available()) {
    //dump all extra data in buffer besides most up to date
    while(Serial.available() > 3) {
      Serial.read();
    }
    uint8_t Buf[3] = {0,0,0x0a}; //Velocity r, velocity l, direction r, direction l
    Serial.readBytes(Buf, 3);

    checkDir(Buf[2]);
    MotorSpeedSetAB(Buf[0], Buf[1], Rear);
    MotorSpeedSetAB(Buf[0], Buf[1], Front);
  }
}

void checkDir(uint8_t Dir) {
  //Dir is 4 bits. 10: forward 01: reverse. 2 sets of two bits provide dir of Rear & left
  if(OldDir !=  Dir){
    uint8_t RDir = Dir >> 2;
    uint8_t FDir = Dir % 4;
    FDir = (FDir >> 1) + ((FDir % 2) << 1);
    uint8_t RFDir = (FDir << 2) + RDir;
    dirSet(RFDir, Rear);
    dirSet(RFDir, Front);
    OldDir = Dir;
  }
}

void MotorSpeedSetAB(uint8_t MotorSpeedA , uint8_t MotorSpeedB, unsigned char addy)  {
  Wire.beginTransmission(addy); // transmit to device I2CMotorDriverAdd
  Wire.write(MotorSpeedSet);                 // set pwm header 
  Wire.write(MotorSpeedA);                   // send pwma 
  Wire.write(MotorSpeedB);                   // send pwmb    
  Wire.endTransmission();                    // stop transmitting
  delay(4);
}

void frequence(unsigned char Frequence, unsigned char addy)  {    
  Wire.beginTransmission(addy); // transmit to device I2CMotorDriverAdd
  Wire.write(PWMFrequenceSet);               // set frequence header
  Wire.write(Frequence);                     //  send frequence 
  Wire.write(Nothing);                       //  need to send this byte as the third byte(no meaning)  
  Wire.endTransmission();                    // stop transmitting
  delay(4);
}

void dirSet(unsigned char _direction, unsigned char addy) {
    Wire.beginTransmission(addy); // begin transmission
    Wire.write(DirectionSet);               // Direction control header
    Wire.write(_direction);                 // send direction control information
    Wire.write(Nothing);                    // need to send this byte as the third byte(no meaning)
    Wire.endTransmission();                 // stop transmitting
    delay(4);                              // wait
}

void printHex(uint8_t num) {
  char hexCar[2];
  Serial.print("0x");
  sprintf(hexCar, "%02X", num);
  Serial.print(hexCar);
}
