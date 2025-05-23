#include <math.h>               // Include math library so we can use trig functions and square root

// Motor driver pins
// Left motor
const int LmotorSpeed = 9;      // ENA
const int LmotorBackward = 10;  // IN1
const int LmotorForward = 11;   // IN2
// Right motor
const int RmotorSpeed = 3;      // ENB
const int RmotorBackward = 6;   // IN4
const int RmotorForward = 5;    // IN3

// Ultrasonic sensor pins
const int trigPin = 2;          // Trigger pin
const int echoPin = 12;         // Echo pin

// Line follower pins
const int LeftA0 = A1;          // Left analog pin
const int RightA0 = A0;         // Right analog pin
const int LeftD0 = 7;           // Left digital pin
const int RightD0 = 8;          // Right digital pin

// LED pin
const int LEDpin = 13;

// Variables to calculate angular motor speed required for car to stay at a constant speed of v cm/s
double stallTorque = 800.0;      // Stall torque in gf.cm
int motorSpeedmax = 244;         // Maximum motor speed in RPM
float efficiency = 0.8;          // Motor efficiency in [0.6,0.9] range                
float k = 0.6;                   // Friction coefficient in [0.3,1.0] range
float m = 500.0;                 // Car mass in grams
float v = 10;                    // Car speed in cm/s

// Logistic function variables for gradual increase/decrease of car speed
int L;                           // The PWM value that will give us the desired motor speed.
float steepness = 0.005;         // The steepness of the curve (k). The larger it is, the faster the car will reach its final speed.
int t = 3000;                    // The amount of time that the speed will increase/decrease for in milliseconds.
int timeBetween = 50;            // The time between each iteration in milliseconds.

// Variables to keep track of when to start the car
int START = 0;
int currentSpeed;


// Initialize all the functions
int logistic(int x);
int setSpeed(int slope);
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

  // Begin serial communication
  Serial.begin(115200);
}


void loop() {
  while (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    if (input.length() > 0) {
      // If the stop sign is detected
      if (input == 'STOP') {
        // Theoretically the speed of the car will no longer be its desired one so LED is turned off
        digitalWrite(LEDpin, LOW);
        // Set the initial speed of the car
        L = setSpeed(input.toInt());
        currentSpeed = L;
        // Trigger the line follower
        line_follower(currentSpeed);
        // gradually slow the car's speed by using the same logistic function but in reverse
        for (int elapsed = t; elapsed >= 0; elapsed-= timeBetween) {
          int decreasingSpeed = logistic(elapsed);
          analogWrite(LmotorSpeed, decreasingSpeed);
          analogWrite(LmotorSpeed, decreasingSpeed);
          // Trigger the line follower
          line_follower(currentSpeed);
          delay(timeBetween);
        }
      }
      else {
        // calculate the distance between the signs and the car
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        double duration = pulseIn(echoPin, HIGH);
        // distance in cm
        double distance = duration * 0.0343 / 2;

        // Check if this is the first sign detected, gradually increase the speed if it is
        if (START == 0) {
          // Set the final speed of the car
          L = setSpeed(input.toInt());
          START = 1;
          // Gradually increase the car's speed by using the logistic function
          for (int elapsed = 0; elapsed >= t; elapsed+= timeBetween) {
            currentSpeed = logistic(elapsed);
            analogWrite(LmotorSpeed, currentSpeed);
            analogWrite(LmotorSpeed, currentSpeed);
            // Trigger the line follower
            line_follower(currentSpeed);
            delay(timeBetween);
          }
          // Theoretically the speed of the car would be its desired one so the LED light is turned on
          digitalWrite(LEDpin, HIGH);
        }
        // Otherwise change the speed when the distance between the car and the sign is 0
        else {
          // keep calculating the distance again until the distance is small
          while (distance > 4.0) {
            // calculate the distance between the signs and the car
            digitalWrite(trigPin, LOW);
            delayMicroseconds(2);
            digitalWrite(trigPin, HIGH);
            delayMicroseconds(10);
            duration = pulseIn(echoPin, HIGH);
            // distance in cm
            distance = duration * 0.0343 / 2;
            // Trigger the line follower
            line_follower(currentSpeed);
          }
          // Then set the speed using the slope as an argument
          currentSpeed = setSpeed(input.toInt());
          analogWrite(LmotorSpeed, currentSpeed);
          analogWrite(LmotorSpeed, currentSpeed);
          // Trigger the line follower
          line_follower(currentSpeed);
        }
      }
    }
  }
  delay(100);
}

// function to compute the speed value according to the its value on the logistic curve
int logistic(int x) {
   return round(L / (1 + exp(-steepness * (x - t/2))));
}

// Function to set speed of motor according to slope
int setSpeed(int slope) {
  // calculate the angle from the slope
  float angle = atan(slope / 100.0);
  if (slope > 0){
    angle *= -1;
  }
  // Calculate the required power
  double requiredPower = v/100 * m/1000 * 9.81 * (sin(angle) + k * cos(angle));
  // maxMotorSpeed converted to rad/sec
  double omegaMax = motorSpeedmax * (PI / 30);
  // stallTorque converted to N.m
  double tauStall = stallTorque / 10200;

  // define a, b, c variables from the quadratic formula
  double a = 1 / omegaMax;
  int b = -1;
  double c = (requiredPower * efficiency) / tauStall;
  // define the discriminant
  double discriminant = (b * b) - (4 * a * c);
  // if the discriminant is less than zero then we will stop the car and this slope will cause the car to flip
  if (discriminant < 0.0) {
    return 0;
  }
  // initialize the numerator
  double numerator = 0;
  // calculate the numerator of the quadratic equation
  // use smaller numerator is slope is negative
  if (slope < 0.0) {
    numerator = -b - sqrt(discriminant);
    }
  // use larger numerator otherwise
  else {
    numerator = -b + sqrt(discriminant);
      }
  // calculate the denominator of quadratic equation
  double denominator = 2 * a;
  // calculate the angular motor speed in RPM
  double omega = (numerator / denominator) * (30 / PI);

  // map the speed so that it is in the range used by analogWrite()
  int motorSpeed = static_cast<int>(omega*255/motorSpeedmax);
  // if the root is negative, then make the car go backwards
  if (motorSpeed < 0) {
    digitalWrite(RmotorForward, LOW);
  	digitalWrite(RmotorBackward, HIGH);
  	digitalWrite(LmotorForward, LOW);
  	digitalWrite(LmotorBackward, HIGH);
    // if the absolute value is larger than 255, then return 255
    if (motorSpeed <= -255) {
      return 255;
    }
    return motorSpeed *= -1;
  }
  // otherwise make it go forward
  else {
    digitalWrite(RmotorForward, HIGH);
  	digitalWrite(RmotorBackward, LOW);
  	digitalWrite(LmotorForward, HIGH);
  	digitalWrite(LmotorBackward, LOW);
    // if the speed is greater than 255, then return 255
    if (motorSpeed > 255) {
      return 255;
    }
    return motorSpeed;
  }
}

// Line follower code
void line_follower(int initialSpeed) {
  // read analog sensor values
  int LeftAnalogValue = analogRead(LeftA0);
  int RightAnalogValue = analogRead(RightA0);
  // read digital sensor values
  int LeftDigitalValue = digitalRead(LeftD0);
  int RightDigitalValue = digitalRead(RightD0);

  // Make the speed of the car low so it turns slowly
  analogWrite(LmotorSpeed, 40);
  analogWrite(RmotorSpeed, 40);

  // While left side of motor is outside of the road (not on black lane), turn car to the right
  if (LeftDigitalValue != 1) {
      // Left side of motor goes forward
      digitalWrite(LmotorForward, HIGH);
      digitalWrite(LmotorBackward, LOW);
      // Right side of motor goes backwards
      digitalWrite(RmotorForward, LOW);
      digitalWrite(RmotorBackward, HIGH);
      // return the function again to check for line follower values again
      return line_follower(initialSpeed);
  }
  // While right side of motor is outside of the road (not on black lane), turn car to the left
  else if (RightDigitalValue != 1) {
    // Right side of motor goes forward
    digitalWrite(RmotorForward, HIGH);
    digitalWrite(RmotorBackward, LOW);
    // Left side of motor goes backwards
    digitalWrite(LmotorForward, LOW);
    digitalWrite(LmotorBackward, HIGH);
    // This makes the car turn left
    // return the function again to check for line follower values again
    return line_follower(initialSpeed);
  }
  else {
    // If both sides of the car are on the lane, make it go forward on both sides
    digitalWrite(RmotorForward, HIGH);
    digitalWrite(RmotorBackward, LOW);
    digitalWrite(LmotorForward, HIGH);
    digitalWrite(LmotorBackward, LOW);
    // Reset the speed of the car
    analogWrite(LmotorSpeed, initialSpeed);
    analogWrite(RmotorSpeed, initialSpeed);
    // the recursion ends when both line follower values indicate being on a black lane
    return;
  }
}
