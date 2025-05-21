#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns, 2 rows

// Pin definitions
const int trigPin = 9;
const int echoPin = 10;
const int buzzerPin = 8;
const int greenLedPin = 7;
const int redLedPin = 6;
const int resetButtonPin = 5;

// Variables
long duration;
int distance;
int score = 0;
bool ballDetected = false;
int resetButtonState = 0;
int lastResetButtonState = 0;

// Sound frequencies
const int goalSound[] = {262, 294, 330, 349, 392, 440, 494, 523};
const int missSound[] = {220, 196, 175, 165, 147, 131, 123, 110};

void setup() {
  // Initialize pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Penalty Shootout");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  
  Serial.begin(9600);
}

void loop() {
  // Check reset button
  resetButtonState = digitalRead(resetButtonPin);
  if (resetButtonState != lastResetButtonState) {
    if (resetButtonState == LOW) {
      score = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Score Reset!");
      lcd.setCursor(0, 1);
      lcd.print("New Score: 0");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Penalty Shootout");
      lcd.setCursor(0, 1);
      lcd.print("Score: ");
      lcd.print(score);
    }
    delay(50);
  }
  lastResetButtonState = resetButtonState;
  
  // Ultrasonic sensor measurement
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  
  // Ball detection logic
  if (distance < 15 && distance > 5 && !ballDetected) {
    ballDetected = true;
    goalScored();
  } else if (distance > 20 && ballDetected) {
    ballDetected = false;
  }
  
  // Miss detection (when ball passes outside gate)
  if (distance > 30 && !ballDetected) {
    ballMissed();
    ballDetected = true; // Prevent multiple triggers
    delay(500); // Debounce
  }
  
  delay(50);
}

void goalScored() {
  score++;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GOAL!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  
  // Light green LED
  digitalWrite(greenLedPin, HIGH);
  digitalWrite(redLedPin, LOW);
  
  // Play goal sound
  for (int i = 0; i < 8; i++) {
    tone(buzzerPin, goalSound[i]);
    delay(100);
  }
  noTone(buzzerPin);
  
  // Turn off LED after delay
  delay(1000);
  digitalWrite(greenLedPin, LOW);
}

void ballMissed() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MISS!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  
  // Light red LED
  digitalWrite(redLedPin, HIGH);
  digitalWrite(greenLedPin, LOW);
  
  // Play miss sound
  for (int i = 0; i < 8; i++) {
    tone(buzzerPin, missSound[i]);
    delay(100);
  }
  noTone(buzzerPin);
  
  // Turn off LED after delay
  delay(1000);
  digitalWrite(redLedPin, LOW);
  
  // Restore display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Penalty Shootout");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
}
