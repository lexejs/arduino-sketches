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
const int analogInPin = A6; // Analog input pin that the potentiometer is attached to
const int analogOutPin = 3,
          delayM = 100; // Analog output pin that the LED is attached to

int sensorValue = 0; // value read from the pot
int outputValue = 0; // value output to the PWM (analog out)
bool isTriggered = false, isChanged = false;

int sValue = 1,
    buttonPin = 2,
    prevValue = 0,
    buttonState = 0,
    absValue,
    sendValue;
long loopsTarget = 2000 / delayM * 10,
     loops = 0;
void setup()
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  pinMode(buttonPin, INPUT);
}

void loop()
{
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  buttonState = digitalRead(buttonPin);

  isChanged = isTriggered;
  isTriggered = buttonState == HIGH;
  isChanged = isChanged != isTriggered;

  // map it to the range of the analog out:
  outputValue = map(sensorValue, 800, 1023, 0, 255);
  if (isTriggered)
  {
    loops = 0;
  }

  if (outputValue < 0 || !isTriggered)
  {
    if (!isTriggered && loops++ > loopsTarget)
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

  if (absValue > 1 || isChanged)
  {
    // print the results to the Serial Monitor:
    Serial.print("sensor = ");
    Serial.print(sensorValue);
    Serial.print("\t sValue = ");
    Serial.print(sValue);
    Serial.print("\t output = ");
    Serial.println(outputValue);
    if (isTriggered)
    {
      Serial.println(" isTriggered !!!  ");
    }
  }

  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(delayM);
}
