/*
*************************************************************************************************************************
                          Name: Atif Ehab Atif
                          gmail: atifeheb200@gmai.com
                          project: Elevator control access
*************************************************************************************************************************
*/

#include <SPI.h>
#include <MFRC522.h>

//Assign symbolic names to specific pin numbers
#define M1_IN1 4
#define M1_IN2 5
#define M1_EN 3
#define LimitSwitch1 7
#define LimitSwitch2 6
#define RST_PIN 9
#define SS_PIN 10 
#define BUZZER 2

bool MotorState = false;

MFRC522 RFID(SS_PIN, RST_PIN);

void setup()
{
  Serial.begin(9600);
  //Setting the pins mode.
  pinMode(M1_IN1, OUTPUT);
  pinMode(M1_IN2, OUTPUT);
  pinMode(M1_EN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LimitSwitch1, INPUT);
  pinMode(LimitSwitch2, INPUT);
  SPI.begin();                    //To initialize the spi communication.
  RFID.PCD_Init();                // To initialize tge MFRC522 sensor
}

void loop()
{
  String checkResult = check();
  if(check() == "accepted" && digitalRead(LimitSwitch1) == HIGH)       // That line is for checking if the clinet have right card/Tag or not and checking if the elevator at ground or not.
  {
    while(1)                 // This loop for continuously checking on limit switch at the top to know if elevator is already on the top or not and never leave until motor stop.
    {
      if(digitalRead(LimitSwitch2) == HIGH)
      {
        Motor_stop();
        return;
      }
      Motor_up();
    }
  }
  else if(check() == "accepted" && digitalRead(LimitSwitch2) == HIGH)       // That line is for checking if the clinet have right card/Tag or not and checking if the elevator at ground or not.
  {
    while(1)                 // This loop for continuously checking on limit switch at the top to know if elevator is already on the bottom or not and never leave until motor stop.
    {
      if(digitalRead(LimitSwitch1) == HIGH)
      {
        Motor_stop();
        return;
      }
      Motor_down();
    }
  }
  else if(check() == "denied")      // This check if the client have wrong card/tag then buzzer will start tunning for 5 sec.
  {
    tone(BUZZER, 5000);
    delay(5000);
    noTone(BUZZER);
  }
}

String check()
{
  if (MotorState == false)
  {
    if ( ! RFID.PICC_IsNewCardPresent())
      return "no card";
    if ( ! RFID .PICC_ReadCardSerial())
      return "no card";
    Serial.print("UID tag :");
    String Tag= "";
    for (byte i = 0; i < RFID.uid.size; i++)   //This loop is for writing the tag name and convert it inot hexdecimal.
    {
      Serial.print(RFID.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(RFID.uid.uidByte[i], HEX);
      Tag.concat(String(RFID.uid.uidByte[i] < 0x10 ? " 0" : " "));
      Tag.concat(String(RFID.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Message : ");
    Tag.toUpperCase();
    if (Tag.substring(1) == "A0 29 E9 50")            //Change here the UID of the card/cards that you want to give access.
    {
      Serial.println("Authorized access");
      return "accepted";                              //If client put the right card/tag this function will return accepted which make the elevator work.
    }
    else
    {
      Serial.println("Authorized denied");
      return "denied";                               // If client put the wrong card/tag this function will return denied which make the elevator doesn't work & and buzzer start tunning.
    }
  }
}

void Motor_up()                       //This function is to make motor run in direction that lift the elevator up.
{
  analogWrite(M1_EN, 100);            //This pin is to control the speed of motor in during moving up.
  digitalWrite(M1_IN1, HIGH);
  digitalWrite(M1_IN2, LOW);
  MotorState = true;
}

void Motor_down()                     //This function is to make motor run in direction that make the elevator goes down.
{
  analogWrite(M1_EN, 25);             //This pin is to control the speed of motor in during moving down.
  digitalWrite(M1_IN1, LOW);
  digitalWrite(M1_IN2, HIGH);
  MotorState = true;
}
void Motor_stop()                     //This function is to make the motor stop that make the elevator ready for next check.
{
  digitalWrite(M1_IN1, LOW);
  digitalWrite(M1_IN2, LOW);
  MotorState = false;
}