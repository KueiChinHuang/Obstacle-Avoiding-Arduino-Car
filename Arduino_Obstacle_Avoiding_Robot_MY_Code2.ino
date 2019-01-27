/*          Arduino Obstacle Avoiding Robot
    with a servo motor and an ultrasonic sensor HC-SR04
              LED and buzzer
          
  Dev: Michalis Vasilakis // Date: 11/12/2015 // www.ardumotive.com 
  Use and modify by Kuei-Chin Huang // Date: 12/12/2018 */

//Libraries
#include <Servo.h>
#include "Ultrasonic.h"

//Constants
const int button = 2;           //Button pin to pin 2
const int led    = 3;           //Led pin (throught resistor) to pin 3
const int buzzer = 4;            //Buzzer pin to pin 4
const int motorLBackward = 8;     //motor Right positive (+) pin to pin 8 (PWM)
const int motorLForward = 9;      //motor Right negative (-) pin to pin 9 (PWM)
const int motorRBackward = 10;    //motor Left positive (+) pin to pin 10 (PWM)
const int motorRForward = 11;     //motor Left negative (-) pin to pin 11 (PWM)
const int carSpeed = 255;         //Set motor speed to 255

Ultrasonic ultrasonic(A1 , A0);    //Create Ultrsonic object ultrasonic(trig pin,echo pin)
Servo myservo;                  //Create Servo object to control a servo

//Variables
int distance;           //Variable to store distance from an object
int checkRight;         //Variable to store right side distance from an object
int checkLeft;          //Variable to store left side distance from an object
int function = 0;       //Variable to store function of robot: '1' running or '0' stoped. By default is stoped
int buttonState = 0;     //Variable to store the state of the button. By default '0'
int pos = 81;           //Variable to store the servo position. By default 81 degrees - sensor will 'look' forward
int flag = 0;           //Useful flag to keep the button state when button is released


void setup() {
  myservo.attach(7);             //Servo pin connected to pin 7
  myservo.write(pos);            // tell servo to go to position in variable 'pos'
  pinMode(button, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(motorLBackward, OUTPUT);
  pinMode(motorLForward, OUTPUT);
  pinMode(motorRBackward, OUTPUT);
  pinMode(motorRForward, OUTPUT);
  Serial.begin(9600); //This will enable the Arduino to send data to the Serial monitor.
}

void loop() {
  //  delay(500);   // If I want to troble shooting and need to look at monitor, I need to use this delay command
  //  Check button state
  buttonState = digitalRead(button);

  //Change main function (stoped/running) when button is pressed
  if (buttonState == LOW) {//If button is pressed once...
    delay(500);
    if ( flag == 0) {  // If button is pressed once...
      function = 1;    // change function to 1 (start to run)
      flag = 1;       // change flag variable
      Serial.println("The button is preesed.");
    }
    else if ( flag == 1) { // If button is pressed twice...
      function = 0;       // change function to 0 (stop running)
      flag = 0;           // change flag variable again
      Serial.println("The button is preesed again.");
    }
  }

  if (function == 0) {      // If button is unpressed or pressed twice then:
    myservo.write(90);      // set servo 90 degress - sensor will look forward
    stop();                // robot remain stoped
    noTone(buzzer);         // buzzer off
    digitalWrite(led, HIGH); // and led on
    Serial.println("STOOOOOP");
  }
  else if (function == 1) {          // If button is pressed then:
    Serial.println("Let's go!");
    distance = ultrasonic.Ranging(CM); // Use 'CM' for centimeters
    // Check for objects...
    if (distance > 20) {     // If distance > 20cm
      forward();            // All clear, move forward!
      noTone(buzzer);        // buzzer off
      digitalWrite(led, LOW); // and led off
      Serial.println("The distance is bigger than 20. Let's go forward.");
    }
    else if (distance <= 20) {// If distance <= 20cm
      stop();               //Object detected! Stop the robot and check left and right for the better way out!
      tone(buzzer, 5);       // play a tone
      digitalWrite(led, HIGH); // turn the led on
      Serial.println("The distance is less than 20. Let's scan the distance and decide which way to go.");

      //Start scanning...
      //Scan left side
      myservo.write(130);                //tell servo to go to position 130
      delay(1000);                      //waits 1000ms for the servo to reach the position
      checkLeft = ultrasonic.Ranging(CM);  //Take distance from the left side
      Serial.print("The distance of left side is ");
      Serial.println(checkLeft);

      //Scan right side
      myservo.write(30);                  //tell servo to go to position 30
      delay(1000);                       //waits 1000ms for the servo to reach the position
      checkRight = ultrasonic.Ranging(CM);  //Take distance from the right side
      Serial.print("The distance of right side is ");
      Serial.println(checkRight);
      //Turn sensor back
      myservo.write(80);                  // Sensor "look" forward again (80)

      //Finally, take the decision, turn left or right?
      if (checkLeft <= 20 && checkRight <= 20) { // If distance of left side and right side both are <= 20cm
        Serial.println("The distance of both side are less than 20. Let's backward.");
        backward();                           // The road is closed... go back and then left ;)
        delay(1000);                          // backward for 1000ms
        left();                               // and turn left for 200ms
        delay(200);
      }
      // If distance of left side and right side are not both <= 20cm, then check which side has longer distance
      else if (checkLeft > checkRight) {  // if left side distance > right side distance, means there is more space on right side
        Serial.println("The distance of left side is bigger than right side. Let's turn left.");
        left();                        // then turn right for 300ms
        delay(300);
      }
      else if (checkLeft < checkRight) {  // if left side distance < right side distance, means there is more space on left side
        Serial.println("The distance of right side is bigger than left side. Let's turn right.");
        right();                        // turn left for 200ms
        delay(200);
      }
    }
  }
}

void forward() {
  analogWrite(motorLBackward, 0);
  analogWrite(motorLForward, carSpeed);
  analogWrite(motorRBackward, 0);
  analogWrite(motorRForward, carSpeed * 0.7);
}

void backward() {
  analogWrite(motorLBackward, carSpeed);
  analogWrite(motorLForward, 0);
  analogWrite(motorRBackward, carSpeed * 0.7);
  analogWrite(motorRForward, 0);
}

void right() {
  analogWrite(motorLBackward, 0);
  analogWrite(motorLForward, carSpeed);
  analogWrite(motorRBackward, carSpeed * 0.7);
  analogWrite(motorRForward, 0);
}

void left() {
  analogWrite(motorLBackward, carSpeed);
  analogWrite(motorLForward, 0);
  analogWrite(motorRBackward, 0);
  analogWrite(motorRForward, carSpeed * 0.7);
}
void stop() {
  analogWrite(motorLBackward, 0);
  analogWrite(motorLForward, 0);
  analogWrite(motorRBackward, 0);
  analogWrite(motorRForward, 0);
}
