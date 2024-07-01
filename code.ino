#include <Energia.h>

// Ultrasonic sensor pins
int trigPin = PF_1; // TRIG pin connected to PF1
int echoPin = PF_2; // ECHO pin connected to PF2
int ledPin = RED_LED; // Inbuilt LED pin (assuming it's connected to RED_LED pin)

// Button pin
int buttonPin = PUSH1; // SW1 button connected to PUSH1

float duration_us, distance_cm;
float minDistance_cm = 10.0; // Minimum distance threshold in cm

// Define motor control pins
#define IN1 PA_2
#define IN2 PA_3
#define IN3 PA_4
#define IN4 PA_5
#define ENA PB_6
#define ENB PB_7

bool overrideActive = false;

void setup() {
  // Begin serial port
  Serial.begin(9600);

  // Configure the trigger pin to output mode
  pinMode(trigPin, OUTPUT);
  // Configure the echo pin to input mode
  pinMode(echoPin, INPUT);
  // Configure the LED pin to output mode
  pinMode(ledPin, OUTPUT);
  
  // Configure the button pin to input mode
  pinMode(buttonPin, INPUT_PULLUP); // Use internal pull-up resistor
  
  // Set motor control pins as output
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // Set motor enable pins as output
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
}

void loop() {
  // Check if the button is pressed
  if (digitalRead(buttonPin) == LOW) {
    overrideActive = !overrideActive; // Toggle the override state
    delay(300); // Debounce delay
  }

  // Generate 10-microsecond pulse to TRIG pin
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure duration of pulse from ECHO pin
  duration_us = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distance_cm = 0.017 * duration_us;

  // Print the value to Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  // Check if the override is active or the distance is less than the minimum threshold
  if (overrideActive) {
    // If override is active, run motors forward regardless of distance
    Serial.println("Override active! Running motors.");
    digitalWrite(ledPin, LOW); // Turn off the LED

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 255); // Full speed for Motor A

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 255); // Full speed for Motor B
  } else if (distance_cm < minDistance_cm) {
    // If obstacle is closer than the threshold, blink the LED and stop motors
    digitalWrite(ledPin, HIGH); // Turn on the LED
    delay(100); // Delay for visibility
    digitalWrite(ledPin, LOW); // Turn off the LED
    delay(100); // Delay for visibility
    
    // Print safety message when safe limit exceeded
    Serial.println("Safe limit exceeded!");

    // Stop both motors
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
  } else {
    // If distance is greater than or equal to the threshold, run motors forward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 255); // Full speed for Motor A

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 255); // Full speed for Motor B
  }

  delay(500); // Delay before next measurement
}