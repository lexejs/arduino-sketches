const int mySensValsSize = 12,
          logEveryLoops = 200,
          minRiseLimit = 20,
          minValueLimit = 200,
          normValue = 180,
          ledPin = LED_BUILTIN,
          analogInPin = A0,
          minHighValueLoops = 20;

#include <TM1637.h>

#define CLK 2 //pins definitions for TM1637 and can be changed to other ports
#define DIO 3
TM1637 tm1637(CLK, DIO);
uint8_t Clkpin;
uint8_t Datapin;
uint8_t Cmd_SetData;
uint8_t Cmd_SetAddr;
uint8_t Cmd_DispCtrl;
boolean _PointFlag;
int v1, v2, v3 = normValue, val, pVal = 0, loops = 0, highValueLoops = 0;
int mySensVals[mySensValsSize];
bool isTriggered = false, isBelowNorm = true;
int ledState = LOW, sensorValue, outputValue = 0, minVal = normValue, maxVal = normValue * 5;
// value output to the PWM (analog out)

void setup()
{
  pinMode(ledPin, OUTPUT);

  tm1637.init();
  tm1637.set(2); //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

  Serial.begin(9600);

  for (int count = 0; count < mySensValsSize; count++)
  {
    mySensVals[count] = 0;
  }
}

void loop()
{
  loops++;
  sensorValue = analogRead(analogInPin);

  if (sensorValue > maxVal)
  {
    maxVal = sensorValue;
  }

  if (sensorValue < minVal)
  {
    minVal = sensorValue;
  }

  // calc smoothing values and percentage
  v2 = v3;
  v3 = sensorValue;
  val = v2 + 0.2 * (-v2 + v3);
  //pVal = 100 * ((val - minVal) / (float)(maxVal - minVal));

  for (int count = 0; count < mySensValsSize - 1; count++)
  {
    mySensVals[count] = mySensVals[count + 1];
  }
  mySensVals[mySensValsSize - 1] = sensorValue;

  isBelowNorm = true;

  v1 = 0;
  for (int count = 0; count < mySensValsSize - 1; count++)
  {

    isBelowNorm = isBelowNorm && mySensVals[count] < normValue;

    if (mySensVals[count] != 0) // skip initialized as 0 values
      v1 = v1 + mySensVals[count + 1] - mySensVals[count];
  }

  isTriggered = false;

  isTriggered = v1 > minRiseLimit && v1 > 0 || val > minValueLimit;

  printSerial();

  if (highValueLoops > 0)
  {
    highValueLoops++;
  }
  if (isTriggered)
  {
    ledState = HIGH;
    highValueLoops++;
  }
  else if (isBelowNorm && highValueLoops > minHighValueLoops)
  {
    ledState = LOW;
    highValueLoops = 0;
  }
  digitalWrite(ledPin, ledState);

  DigitDisplayWrite(CLK, DIO, val);
  delay(500);
}

void printSerial()
{

  // print the results to the Serial Monitor:
  if (isTriggered || loops > logEveryLoops)
  {

    if (loops > logEveryLoops)
    {
      Serial.print("LOG ");
    }
    else
    {
      Serial.print("!!! TRIGGERED ");
    }
    //Serial.print("sensor = ");   Serial.print(sensorValue);
    Serial.print("\t Vs = ");
    Serial.print(val);
    //Serial.print("\t Q = ");    Serial.print(pVal);    Serial.print("%");

    //Serial.print("\t  v1 > minRiseLimit = ");    Serial.print( v1 > minRiseLimit);
    Serial.print("\t min = ");    Serial.print(minVal);
    Serial.print("\t up = ");
    Serial.print(v1);

    Serial.println("");
    loops = 0;
  }
}

void display(uint8_t BitAddr, int8_t DispData)
{
  tm1637.display(BitAddr, DispData);
}

void DigitDisplayWrite(int pinClk, int pinDta, int num)
{
  if (num > 9999 || num < 0)
    return;

  static int num_buf = 0;
  if (num == num_buf)
    return;
  num_buf = num;

  // init io
  Clkpin = pinClk;
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
