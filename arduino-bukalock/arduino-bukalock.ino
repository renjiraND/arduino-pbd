/*
* Ultrasonic Sensor HC-SR04 and Arduino Tutorial
*
* by Dejan Nedelkovski,
* www.HowToMechatronics.com
*
* Modified by Renjira Naufhal Dhiaegana
* 
* Sources:
* 7-Segment https://www.instructables.com/id/Multipel-7-Segmen-di-Arduino-dengan-Chip-7447/
*/

#include <Servo.h>

// defines pins numbers
const int trigPin = 9;
const int echoPin = 10;
const int buzz = 11;
const int pinA = 0;
const int pinB = 1;
const int pinC = 2;
const int pinD = 3;

// defines led 7-segment
int num_array[10][4] = {  { HIGH,LOW,LOW,LOW },     // 0
                          { LOW,HIGH,LOW,LOW },     // 1
                          { HIGH,HIGH,LOW,LOW },    // 2
                          { LOW,LOW,HIGH,LOW },     // 3
                          { HIGH,LOW,HIGH,LOW },    // 4
                          { LOW,HIGH,HIGH,LOW },    // 5
                          { HIGH,HIGH,HIGH,LOW },   // 6
                          { LOW,LOW,LOW,HIGH },     // 7
                          { HIGH,LOW,LOW,HIGH },    // 8
                          { LOW,LOW,LOW,LOW }};     // 9

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

// defines variables
long duration;
int distance;
bool lock = false;

void servoLock();
void servoUnlock();
void numWrite(int);

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(buzz, OUTPUT); //
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT); 
//  myservo.attach(3);  // attaches the servo on pin 3 to the servo object
//  Serial.begin(9600); // Starts the serial communication
//  servoLock();
}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Test 7-segment
  numWrite(9);
  delay(1000);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculating the distance
  distance = duration*0.034/2;
  
//  // Prints the distance on the Serial Monitor
//  Serial.print("Distance: ");
//  Serial.println(distance);
//  if(distance < 8 && lock){
//    servoUnlock();
//  }
}

void servoLock()
{
  if(lock == false){
    for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
  }
  lock = true;
}

void servoUnlock()
{
  if(lock == true){
    for (pos = 90; pos >= 0; pos -= 1) { // goes from 0 degrees to 180 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    myservo.write(90);
  }
  lock = false;
}

// this functions writes values to the sev seg pins  
void numWrite(int number) 
{
    digitalWrite(pinA, num_array[number-1][0]); 
    digitalWrite(pinB, num_array[number-1][1]); 
    digitalWrite(pinC, num_array[number-1][2]); 
    digitalWrite(pinD, num_array[number-1][3]);
}
