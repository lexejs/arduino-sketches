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
const int analogInPin = A6, // Analog input pin that the potentiometer is attached to
    analogOutPin = 3,
          delayM = 100,
          touchPin = 4,
          buttonPin = 2;

const long loopsTarget = 2000 / delayM * 10, // 10 secs
    loopsSticked = 800 / delayM,             // 400 ms
    loopsClicked = 1000 / delayM;            // 500 ms

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

void setup()
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  pinMode(buttonPin, INPUT);
  pinMode(touchPin, INPUT);
}

void loop()
{
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  touchState = digitalRead(touchPin);
  buttonState = digitalRead(buttonPin);

  isChanged = isTriggered;
  isTriggered = buttonState == HIGH;
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

  if (isStickDetected)
  {
    clickCount = 0;
  }

  if (clickCount > 0 && isClicked == false && loopsClick++ > loopsClicked)
  {

    Serial.print(" clickCount !!!  \t");
    Serial.println(clickCount);
    //do clickCount specific logic
    clickCount = 0;
  }

  // map it to the range of the analog out:
  outputValue = max(0, map(sensorValue, 800, 1023, 0, 255));

  if (isTriggered)
  {
    loops = 0;
  }
  else if (loops++ > loopsTarget)
  {
    outputValue = 0;
  }

  prevValue = sValue;

  absValue = abs(sValue - outputValue);

  if (absValue > 25 && sValue > 25)
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
  analogWrite(analogOutPin, sValue);

  if (isTriggered)
  {
  }
  else
  {
  }

  if (absValue > 1 || isChanged || isClickChanged)
  {
    if (isTriggered)
    {
      Serial.print(" isTriggered !!!  \t");
    }
    if (isClickChanged)
    {
      Serial.print(" isClickChanged !!!  \t");
    }
    // print the results to the Serial Monitor:
    Serial.print("sensor = ");
    Serial.print(sensorValue);
    Serial.print("\t sValue = ");
    Serial.print(sValue);
    Serial.print("\t output = ");
    Serial.println(outputValue);
  }

  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(delayM);
}
