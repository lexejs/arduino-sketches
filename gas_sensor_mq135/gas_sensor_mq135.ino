
#include <TM1637.h>

// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = A5; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0, minVal = 150, maxVal = 750;      // value output to the PWM (analog out)

#include "TM1637.h"
#define CLK 2//pins definitions for TM1637 and can be changed to other ports
#define DIO 3
TM1637 tm1637(CLK, DIO);
uint8_t Clkpin;
uint8_t Datapin;

uint8_t Cmd_SetData;
uint8_t Cmd_SetAddr;
uint8_t Cmd_DispCtrl;
boolean _PointFlag;
int v1 = 150, v2 = 150, v3 = 150, val = 150, pVal = 0;

const unsigned int mySensValsSize = 6;
int mySensVals[mySensValsSize];

void setup()
{
  tm1637.init();
  tm1637.set(3);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

  Serial.begin(9600);

  for (int count = 0; count < mySensValsSize; count++) {
    mySensVals[count] = 150;
  }
}

void loop() {

  sensorValue = analogRead(analogInPin);

  if (sensorValue > maxVal) {
    maxVal = sensorValue;
  }

  if (sensorValue < minVal) {
    minVal = sensorValue;
  }

  v2 = v3;
  v3 = sensorValue;
  val = v2 + 0.2 * (v2 - v3);

  pVal = 100 * ((val - minVal) / (float)(maxVal - minVal));

  for (int count = 0; count < mySensValsSize - 1; count++) {
    mySensVals[count] = mySensVals[count + 1];
  }
  
  mySensVals[mySensValsSize-1]=sensorValue;
  
  v1 = 0;
  for (int count = 0; count < mySensValsSize - 1; count++) {
    v1 = v1 + mySensVals[count + 1] - mySensVals[count];
  }

  // print the results to the Serial Monitor:
  if (sensorValue > 200 || val > 200) {
    Serial.print("sensor = ");
    Serial.print(sensorValue);

    Serial.print("\t s = ");
    Serial.print(val);

    Serial.print("\t P = ");
    Serial.print(pVal);
    Serial.print("%");

    Serial.print("\t v1 sum = ");
    Serial.println(v1);

  } else if (v1 > 10) {
    Serial.print("v1 sum = ");
    Serial.println(v1);
  }

  DigitDisplayWrite(CLK, DIO, pVal);
  delay(500);
}

void display(uint8_t BitAddr, int8_t DispData) {
  tm1637.display(BitAddr, DispData);
}

void DigitDisplayWrite(int pinClk, int pinDta, int num)
{
  if (num > 9999 || num < 0)return;

  static int num_buf = 0;
  if (num == num_buf)return;
  num_buf = num;

  // init io
  Clkpin  = pinClk;
  Datapin = pinDta;
  pinMode(Clkpin, OUTPUT);
  pinMode(Datapin, OUTPUT);

  if (num < 10)
  {
    display(3, num);
    display(2, 0x7f);
    display(1, 0x7f);
    display(0, 0x7f);
  }
  else if (num < 100)
  {
    display(3, num % 10);
    display(2, (num / 10) % 10);
    display(1, 0x7f);
    display(0, 0x7f);
  }
  else if (num < 1000)
  {
    display(3, num % 10);
    display(2, (num / 10) % 10);
    display(1, (num / 100) % 10);
    display(0, 0x7f);
  }
  else
  {
    display(3, num % 10);
    display(2, (num / 10) % 10);
    display(1, (num / 100) % 10);
    display(0, (num / 1000) % 10);
  }
}
