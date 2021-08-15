// constants won't change. Used here to set a pin number:
const int ledPin =  LED_BUILTIN;// the number of the LED pin

// Variables will change:
int ledState = LOW;             // ledState used to set the LED

unsigned const int maxDelay = 150, maxInterval = 4000;
unsigned int iOn = 0, iOff = 0, pwm = 0, interval = 0;
bool directUp = true;

void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  interval++;

  if (iOn == 0 && iOff == 0 ) {

    if (interval >= maxInterval) {
      if (directUp && pwm < maxDelay) {
        pwm++;
      } else {
        pwm--;
      }
      interval = 0;

      if (pwm == 0 || pwm == maxDelay) {
        directUp = !directUp;
      }

      Serial.print("pwm = ");
      Serial.print(pwm);
      Serial.print("\t directUp = ");
      Serial.println(directUp);
    }

    iOn = pwm;
    iOff = maxDelay - pwm;

  }

  if (iOn > 0) {
    ledState = HIGH;
    iOn--;
  } else if (iOff > 0) {
    ledState = LOW;
    iOff--;
  }

  //  if (iOn == 0 && iOff == 0 ) {
  //    directUp = !directUp;
  //  }

  digitalWrite(ledPin, ledState);

}
