#include <math.h>              // Include math library so we can use trig functions and square root

// Motor driver pins
// Left motor
const int LmotorSpeed = 9;      // ENA
const int LmotorBackward = 10;  // IN1
const int LmotorForward = 11;   // IN2
// Right motor
const int RmotorSpeed = 3;      // ENB
const int RmotorBackward = 6;    // IN4
const int RmotorForward = 5;   // IN3

// Ultrasonic sensor pins
const int trigPin = 2;          // Trigger pin
const int echoPin = 12;         // Echo pin

// Line follower pins
const int LeftA0 = A1;          // Left analog pin
const int RightA0 = A0;         // Right analog pin
const int LeftD0 = 7;           // Left digital pin
const int RightD0 = 8;          // Right digital pin

// LED pin (if used)
const int LEDpin = 13;

// Variables to calculate angular motor speed required for car to stay at a constant speed of v cm/s
double stallTorque = 800.0;      // Stall torque in gf.cm
int motorSpeedmax = 244;         // Maximum motor speed in RPM
float efficiency = 0.8;          // Motor efficiency in [0.6,0.9] range                
float k = 0.6;                   // Friction coefficient in [0.3,1.0] range
float m = 500.0;                 // Car mass in grams
float v = 10;                    // Car speed in cm/s

// Logistic function variables for gradual increase/decrease of car speed
int L;                           // The PWM value that will give us the disired motor speed.
float steepness = 0.005;         // The steepness of the curve (k). The larger it is, the faster the car will reach its final speed.
int t = 3000;                    // The amount of time that the speed will increase/decrease for in milliseconds.
int timeBetween = 50;            // The time between each iteration in milliseconds.

// Variables to keep track of when to start the car
int START = 0;
void line_follower();

void setup() {
  
  // Motor driver pins
  // Left
  pinMode(LmotorSpeed, OUTPUT);
  pinMode(LmotorForward, OUTPUT);
  pinMode(LmotorBackward, OUTPUT);
  // Right
  pinMode(RmotorSpeed, OUTPUT);
  pinMode(RmotorForward, OUTPUT);
  pinMode(RmotorBackward, OUTPUT);

  // Ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //Line follower pins
  // Analog pins
  pinMode(LeftA0, INPUT);
  pinMode(RightA0, INPUT);
  // Digital pins
  pinMode(LeftD0, INPUT);
  pinMode(RightD0, INPUT);

  // LED pin
  pinMode(LEDpin, OUTPUT);
  digitalWrite(RmotorForward, HIGH);
  digitalWrite(RmotorBackward, LOW);
  digitalWrite(LmotorForward, HIGH);
  digitalWrite(LmotorBackward, LOW);

  // Begin serial communication
  Serial.begin(9600);
}

void loop() {
  digitalWrite(LEDpin, HIGH);
  analogWrite(LmotorSpeed, 150);
  analogWrite(RmotorSpeed, 150);
  line_follower();  
  analogWrite(LmotorSpeed, 150);
  analogWrite(RmotorSpeed, 150);
  delay(100);
}

void line_follower() {
  // read analog sensor values
  int LeftAnalogValue = analogRead(LeftA0);
  int RightAnalogValue = analogRead(RightA0);
  // read digital sensor values
  int LeftDigitalValue = digitalRead(LeftD0);
  int RightDigitalValue = digitalRead(RightD0);

  digitalWrite(LmotorSpeed, 40);
  digitalWrite(RmotorSpeed, 40);

  // While left side of motor is outside of the road, turn car to the right
  if (LeftDigitalValue != 1) {
      // Left side of motor goes forward
      digitalWrite(LmotorForward, HIGH);
      digitalWrite(LmotorBackward, LOW);
      // Right side of motor goes backwards
      digitalWrite(RmotorForward, LOW);
      digitalWrite(RmotorBackward, HIGH);
      // This makes the car turn right
      return line_follower();
  }
  else if (RightDigitalValue != 1) {
    // Right side of motor goes forward
    digitalWrite(RmotorForward, HIGH);
    digitalWrite(RmotorBackward, LOW);
    // Left side of motor goes backwards
    digitalWrite(LmotorForward, LOW);
    digitalWrite(LmotorBackward, HIGH);
    // This makes the car turn left
    return line_follower();
  }
  else {
    // If both sides of the car are on the lane, make it go forward on both sides
    digitalWrite(RmotorForward, HIGH);
    digitalWrite(RmotorBackward, LOW);
    digitalWrite(LmotorForward, HIGH);
    digitalWrite(LmotorBackward, LOW);
    return;
  }
}

