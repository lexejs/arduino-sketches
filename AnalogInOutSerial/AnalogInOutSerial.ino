/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin 9 to ground through 220 ohm resistor

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogInOutSerial
*/

// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A6, 

          touchPin = 4,

          buttonPin1 = 5,
          buttonPin2 = 6,
          buttonPin3 = 7,
          buttonPin4 = 8,

          powerOutPin = 9,

          delayM = 100,
          minSensorLevel = 800;

const long loopsTarget = 2000 / delayM * 120, // * secs
    loopsSticked = 800 / delayM,              // 1/2 ms
    loopsClicked = 600 / delayM;              // 1/2 ms

int sValue = 1,
    prevValue = 0,
    buttonState = 0,
    touchState = 0,
    absValue,
    sendValue,
    sensorValue = 0, // value read from the pot
    outputValue = 0,
    clickCount = 0; // value output to the PWM (analog out)

bool isTriggered = false, isChanged = false, isClicked = false, isClickChanged = false, isStickDetected = false;

long loops = 0, loopsStick = 0, loopsClick = 0;

enum action
{
  none,      //0
  triggered, //1
  sticked,   //2
  clicked1,  //3
  clicked2,  //4
  clicked3,  //5
  keepOn     //6
};

action doNextAction = none;

void setup()
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(buttonPin4, INPUT);
  pinMode(touchPin, INPUT);
}

void loop()
{

  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  touchState = digitalRead(touchPin);

  isChanged = isTriggered;
  isTriggered = digitalRead(buttonPin1) == HIGH
    || digitalRead(buttonPin2) == HIGH 
    || digitalRead(buttonPin3) == HIGH 
    || digitalRead(buttonPin4) == HIGH;
  isChanged = isChanged != isTriggered;

  isClickChanged = isClicked;
  isClicked = touchState == HIGH;
  isClickChanged = isClickChanged != isClicked;

  if (isClickChanged && isClicked == false)
  {
    loopsStick = 0;
    isStickDetected = false;
  }

  if (isStickDetected == false && isClicked && loopsStick++ > loopsSticked)
  {
    isStickDetected = true;

    Serial.println(" Sticked !!!  \t");
  }

  if (isClickChanged && isClicked)
  {
    loopsClick = 0;
    clickCount++;
  }

  // handle events below

  if (isStickDetected)
  {
    doNextAction = sticked;

    clickCount = 0;
  }

  if (clickCount > 0 && isClicked == false && loopsClick++ > loopsClicked)
  {

    Serial.print(" clickCount !!!  \t");
    Serial.println(clickCount);
    //do clickCount specific logic
    if (clickCount == 1)
      doNextAction = clicked1;
    else if (clickCount == 3)
      doNextAction = clicked3;

    clickCount = 0;
  }

  // map it to the range of the analog out:
  if (doNextAction != clicked3 && doNextAction != keepOn)
  {

    if (sensorValue > minSensorLevel && sensorValue < 900)
      outputValue = max(0, map(sensorValue, minSensorLevel, 900, 0, 255));

    else if (sensorValue > 900)
    {
      outputValue = 255 - max(0, map(sensorValue, 900, 1023, 0, 255 - 25));
    }
  }

  if (isTriggered && (doNextAction == none || doNextAction == triggered))
  {
    doNextAction = triggered;
    loops = 0;
  }
  else if (loops++ > loopsTarget && doNextAction == triggered)
  {
    outputValue = 0;
    doNextAction = none;
  }

  if (doNextAction == clicked1)
  {

    if (sValue > 0)
    {
      doNextAction = none;
      outputValue = 0;
    }
    else
    {
      if (minSensorLevel > sensorValue)
      {
        outputValue = 255;
      }
      doNextAction = keepOn;
      loops = 0;
      sValue = min(15, outputValue / 2);
    }
  }
  else if (doNextAction == clicked3)
  {
    outputValue = 255;
    if (sValue == 0)
      sValue = min(15, outputValue / 2);
  }
  else if (doNextAction == none)
  {
    outputValue = 0;
  }

  absValue = abs(sValue - outputValue);

  if (absValue > 10 && sValue > 10)
  {
    sValue = outputValue * 0.1 + sValue * 0.9;
  }
  else if (outputValue == 0 && sValue == 1)
  {
    sValue = 0;
  }
  else if (absValue > 1)
  {
    sValue += outputValue > sValue ? +1 : -1;
  }

  //sendValue = isTriggered?sValue:0;
  // change the analog out value:
  analogWrite(powerOutPin, sValue);

  if (absValue > 5 || isChanged || isClickChanged)
  {

    // print the results to the Serial Monitor:
    Serial.print("absValue = ");
    Serial.print(absValue);

    Serial.print("\t sValue = ");
    Serial.print(sValue);

    Serial.print("\t outputValue = ");
    Serial.print(outputValue);

    Serial.print("\t sensorValue = ");
    Serial.print(sensorValue);

    Serial.print("\t loops = ");
    Serial.print(loops);

    Serial.print("\t doNextAction = ");
    Serial.print(doNextAction);

    if (isTriggered)
    {
      Serial.print("\t isTriggered !");
    }

    if (isClickChanged)
    {
      Serial.print("\t isClickChanged !");
    }
    Serial.println("|");
  }

  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(delayM);
}
