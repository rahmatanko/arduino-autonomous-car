// Motor driver pins
// Left motor
const int LmotorSpeed = 9;      // ENA
const int LmotorBackward = 10;   // IN1
const int LmotorForward = 11;  // IN2
// Right motor
const int RmotorSpeed = 3;      // ENB
const int RmotorForward = 6;    // IN4
const int RmotorBackward = 5;   // IN3

// Ultrasonic sensor pins
const int trigPin = 2;          // Trigger pin
const int echoPin = 12;         // Echo pin

// Line follower pins
const int LeftA0 = A1;          // Left analog pin
const int RightA0 = A0;         // Right analog pin
const int LeftD0 = 7;           // Left digital pin
const int RightD0 = 8;          // Right digital pin

// initialize line follower
void line_follower();

// LED pin (if used)
const int LEDpin = 13;

void setup() {
  // LED pin
  pinMode(LEDpin, OUTPUT);

  //Line follower pins
  // Analog pins
  pinMode(LeftA0, INPUT);
  pinMode(RightA0, INPUT);
  // Digital pins
  pinMode(LeftD0, INPUT);
  pinMode(RightD0, INPUT);

  // Begin serial communication
  Serial.begin(9600);
}

void loop() {
  line_follower();
  delay(1000);

}


void line_follower() {
  // read analog sensor values
  int LeftAnalogValue = analogRead(LeftA0);
  int RightAnalogValue = analogRead(RightA0);
  // read digital sensor values
  int LeftDigitalValue = digitalRead(LeftD0);
  int RightDigitalValue = digitalRead(RightD0);


  // For testing, this way we'll see which value corresponds to black (the color of the road)
  Serial.print("LeftAnalogValue = ");
  Serial.print(LeftAnalogValue);
  Serial.print(", RightAnalogValue = ");
  Serial.println(RightAnalogValue);

  Serial.print("LeftDigitalValue = ");
  Serial.print(LeftDigitalValue);
  Serial.print(", RightDigitalValue = ");
  Serial.println(RightDigitalValue);

}
