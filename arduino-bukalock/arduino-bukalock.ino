/*
  Ultrasonic Sensor HC-SR04 and Arduino Tutorial

  by Dejan Nedelkovski,
  www.HowToMechatronics.com

  Modified by Renjira Naufhal Dhiaegana

  Sources:
  7-Segment https://www.instructables.com/id/Multipel-7-Segmen-di-Arduino-dengan-Chip-7447/
*/

#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <dht.h>

// Defines Pins
int val = 0;     // variable for reading the pin status
const int pinC = 2;
const int pinD = 3;
const int pinA = 4;
const int pinB = 5;
const int trigPin = 15;
const int echoPin = 7;
const int buttonPin = 8;
const int rstPin = 9;
const int ssPin = 10;
const int servoPin = 14;
int pinLed = 6;
#define dhtPin A2

dht DHT;

// Defines LED 7-Segment Number
int num_array[10][4] = {  { HIGH,LOW,LOW,LOW },     // 0
                          { LOW,HIGH,LOW,LOW },     // 1
                          { HIGH,HIGH,LOW,LOW },    // 2
                          { LOW,LOW,HIGH,LOW },     // 3
                          { HIGH,LOW,HIGH,LOW },    // 4
                          { LOW,HIGH,HIGH,LOW },    // 5
                          { HIGH,HIGH,HIGH,LOW },   // 6
                          { LOW,LOW,LOW,HIGH },     // 7
                          { HIGH,LOW,LOW,HIGH },    // 8
                          { LOW,LOW,LOW,LOW }};    // 9


// Variables
Servo myservo;  // create servo object to control a servo
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 mfrc522(ssPin, rstPin);   // Create MFRC522 instance.

// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
long duration;
int distance;
bool lock = true;
int buttonState = 0;
int trial = 3;  

void servoLock();
void servoUnlock();
void numWrite(int);
void checkPushButton();
void checkUltrasonicDistance();
void checkRFID();

void setup() {
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
  pinMode(trigPin, OUTPUT);   // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);    // Sets the echoPin as an Input
  pinMode(buttonPin, INPUT);  // Sets the buttonPin as an Input
  pinMode(13, OUTPUT);
  pinMode(pinLed, OUTPUT);
  myservo.attach(servoPin);  // attaches the servo on servoPin to the servo object
  myservo.write(180);
  Serial.begin(9600); // Starts the serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
}

void loop() {
  checkTrial();
  lcd.begin();
  analogWrite(pinLed, 50);
  printIdle();

  // 7-segment
  numWrite(trial);

  // Pushbutton
  checkPushButton();

  // Ultrasonic
  checkUltrasonicDistance();

  //RFID
  checkRFID();

  checkFirebase();
}

void checkTrial()
{
  if (trial == 0) 
  {
    printFailed();
  }
}

void servoLock()
{
  if (lock == false) {
    printIdle();
    myservo.write(180);
    analogWrite(pinLed, 50);
    delay(1500);
  }
  lock = true;
}

void servoUnlock()
{
  if (lock == true) {
    printSuccess();
    analogWrite(pinLed, 255);
    myservo.write(0);
    delay(10000);
  }
  lock = false;
  servoLock();
}

// Fungsi untuk menuliskan angka pada 7-segment LED
void numWrite(int number)
{
  if (number == 0)
  {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, LOW);
  }
  else 
  {
    digitalWrite(pinA, num_array[number-1][0]); 
    digitalWrite(pinB, num_array[number-1][1]); 
    digitalWrite(pinC, num_array[number-1][2]); 
    digitalWrite(pinD, num_array[number-1][3]);
  }
}

void checkPushButton()
{
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    servoUnlock();
  }
}

void checkUltrasonicDistance()
{
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;

  // Check distance
  if (distance < 8 && lock) {
    servoUnlock();
  }
}


void checkRFID()
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println(content.substring(1));
//  if (content.substring(1) == "76 8E BD F7") //change here the UID of the card/cards that you want to give access
//  {
//    servoUnlock();
//  }

//  else {
//    Serial.println(" Access denied");
//    printRFIDFailed();
//    delay(3000);
//  }
}

void printIdle()
{
  DHT.read11(dhtPin);
  lcd.clear();
  lcd.print("Unlock Me!");
  lcd.setCursor(0,1);
  lcd.print(String("Temperature ") + String(DHT.temperature) + String("`"));
}

void printSuccess()
{
  lcd.clear();
  lcd.print(" Unlock Success");
  lcd.setCursor(0, 1);
  lcd.print(" Welcome Home!");
  trial = 3;
  numWrite(trial);
}

void printFailed()
{
  lcd.clear();
  lcd.print(" Unlock Failed");
  lcd.setCursor(0, 1);
  lcd.print("Please wait 5 mins");
  delay(300000);
  trial = 3;
  numWrite(trial);
}

void printRFIDFailed()
{
  lcd.clear();
  lcd.print(" Unlock Failed");
  lcd.setCursor(0, 1);
  lcd.print("Please register!");
  trial -= 1;
  numWrite(trial);
}

void checkFirebase() {
  static char endMarker = '\n'; // message separator
  char receivedChar;     // read char from serial port
  int ndx = 0;          // current index of data buffer
  memset(data, 32, sizeof(data));
  while(Serial.available() > 0) {
    receivedChar = Serial.read();
    if (receivedChar == endMarker) {
      data[ndx] = '\0'; // end current message
      if(data[0] == 'o'){
        servoUnlock();
        Serial.print
      }
      return;
    }
    data[ndx] = receivedChar;
    ndx++;
    if (ndx >= DATA_MAX_SIZE) {
      break;
    }
  }
  Serial.println("error: incomplete message");
  Serial.println(data);
  memset(data, 32, sizeof(data));
}
