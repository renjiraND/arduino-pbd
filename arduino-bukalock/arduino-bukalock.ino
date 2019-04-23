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
                          { LOW,LOW,LOW,HIGH }};    // 9


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
  Serial.begin(9600); // Starts the serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
}

void loop() {
  lcd.begin();
  analogWrite(pinLed, 100);
  printIdle();

  // 7-segment
  numWrite(3);

  // Pushbutton
  checkPushButton();
  
  // Ultrasonic
  checkUltrasonicDistance();

  //RFID
  checkRFID();
}

void servoLock()
{
  if(lock == false){
    printIdle();
    myservo.write(180);
    delay(1500);
  }
  lock = true;
}

void servoUnlock()
{
  if(lock == true){
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
    digitalWrite(pinA, num_array[number-1][0]); 
    digitalWrite(pinB, num_array[number-1][1]); 
    digitalWrite(pinC, num_array[number-1][2]); 
    digitalWrite(pinD, num_array[number-1][3]);
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
    distance = duration*0.034/2;

    // Check distance
    if(distance < 8 && lock){
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
    Serial.print("RFID tag scanned:");
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
       Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
       Serial.print(mfrc522.uid.uidByte[i], HEX);
       content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Message : ");
    content.toUpperCase();
    if (content.substring(1) == "76 8E BD F7") //change here the UID of the card/cards that you want to give access
    {
      Serial.println("Authorized access");
      Serial.println();
      servoUnlock();
    }
   
    else {
      Serial.println(" Access denied");
      printRFIDFailed();
      delay(3000);
    }
}

void printIdle()
{
  DHT.read11(dhtPin);
  lcd.clear();
  lcd.print("Unlock Me!");
  lcd.setCursor(0,1);
  lcd.print(String("Temperature : ") + String(DHT.temperature) + String("\u2103"));
}

void printSuccess()
{
  lcd.clear();
  lcd.print(" Unlock Success");
  lcd.setCursor(0,1);
  lcd.print(" Welcome Home!");
}

void printFailed()
{
  lcd.clear();
  lcd.print(" Unlock Failed");
  lcd.setCursor(0,1);
  lcd.print("Please wait 5 mins");
}

void printRFIDFailed()
{
  lcd.clear();
  lcd.print(" Unlock Failed");
  lcd.setCursor(0,1);
  lcd.print("Please register!");
}
