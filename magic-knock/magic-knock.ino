#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

const int btnPin = 6;
const int ledPin = 13;      // led connected to digital pin 13
const int knockSensor = A0; // the piezo is connected to analog pin 0
const int threshold = 100;  // threshold value to decide when the detected sound is a knock or not

const int servoPin = 9;

// these variables will change:
int sensorReading = 0;      // variable to store the value read from the sensor pin
int lockState = LOW;         // variable used to store the last LED status, to toggle the light
int lastlockState = LOW;
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

int recordingMode = LOW;  // when HIGH the device will store the next 4 knocks

int knocksQty = 2;
long knocks[2];
long lastKnock = 0;
int knockIdx = 0;


void openLock() {
  Serial.println("open");
  myservo.write(178);
//  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(8);                       // waits 15ms for the servo to reach the position
//  }  
}

void closeLock() {
  Serial.println("close");
  myservo.write(45);
//  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(8);                       // waits 15ms for the servo to reach the position
//  }
}


void dumpKnock(int k, int i, float percent) {
  Serial.print("dumpKnock: ref:");
  Serial.print(knocks[i]);
  Serial.print(" idx:");
  Serial.print(i);
  Serial.print(" knock:");
  Serial.print(k);
  Serial.print(" -%:");
  Serial.print((k*(1-percent)));
  Serial.print(" +%:");
  Serial.print((k*(1+percent)));  
  Serial.println(" ");
  
}

int verifyPattern() {
  Serial.println("Checking Pattern...");
  long m;
  int k;
  int i = 0;
  int valid = 0;
//  lastDebounceTime = m;
  lastKnock = millis();
  long timeout = millis() + 10000; // 10 senconds in the future
  while( i<knocksQty ) {
    m = millis();
    if ( m > timeout ) {
      Serial.println("Timeout!!");
      return 0; // is invalid
    }
    sensorReading = analogRead(knockSensor);
    if ((m - lastDebounceTime) > debounceDelay && sensorReading >= threshold) {
      lastDebounceTime = m;
      k = m - lastKnock;
      dumpKnock(k, i, .4);
      lastKnock = m;
      if( knocks[i] > (k*.6) && knocks[i] < (k*1.4) ) {
        valid ++;
      } else {
        valid = 0;
      }
      i++;
    }
  }  
  Serial.print("Result: ");
  Serial.println((valid == i));
  return valid == i;
}

void recordPattern() {
  Serial.println("Recording...");
  long m;
  digitalWrite(ledPin, HIGH);
  lastDebounceTime = 0;
  int first = HIGH;
  while( knockIdx<knocksQty ) {
    m = millis();
    sensorReading = analogRead(knockSensor);
    if ((m - lastDebounceTime) > debounceDelay && sensorReading >= threshold) {
      Serial.println(knockIdx);

      lastDebounceTime = m;
      if(!first) {
        knocks[knockIdx] = m - lastKnock;
        knockIdx ++;      
      } else {
        first = LOW;
      }
      lastKnock = m;
    }
  }
  
  Serial.print("Pattern: ");
  for( int i=0; i<knocksQty; i++ ) {
    Serial.print(knocks[i]);
    Serial.print(", ");
  }
  digitalWrite(ledPin, LOW);
}


void setup() {
  pinMode(btnPin, INPUT);
  pinMode(ledPin, OUTPUT); // declare the ledPin as as OUTPUT
  Serial.begin(9600);       // use the serial port
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object

  recordPattern();
}

void loop() {
  
  // read the sensor and store it in the variable sensorReading:
  sensorReading = analogRead(knockSensor);
//  Serial.println(sensorReading);

  long m = millis();
  
//  if ((m - lastDebounceTime) > debounceDelay && )
  
  
  if ((millis() - lastDebounceTime) > debounceDelay && sensorReading >= threshold) {    
    lastDebounceTime = millis();
  // if the sensor reading is greater than the threshold:
    // toggle the status of the ledPin:
    // update the LED pin itself:
//    digitalWrite(ledPin, lockState);

    if( verifyPattern() ) {
      lockState = !lockState;
      if( lockState ) {
        openLock();
      } else {
        closeLock();
      }
    }
    // send the string "Knock!" back to the computer, followed by newline
//    Serial.println("Knock!");
//    Serial.println(sensorReading);
  }
//  delay(500);  // delay to avoid overloading the serial port buffer
}

