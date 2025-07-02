/*
 * Simple Arduino Torch Height Controller (THC) for CNC
 * Outputs: UP, DOWN signals (relay or logic)
 * Inputs: torch voltage (optional), manual switches for up/down
 * For integration with breakout board (parallel port)
 */

// Pin assignments (change as needed)
const int UP_PIN = 2;     // Output to BOB "Torch UP"
const int DOWN_PIN = 3;   // Output to BOB "Torch DOWN"

const int MANUAL_UP = 4;  // Optional: Manual UP button
const int MANUAL_DOWN = 5;// Optional: Manual DOWN button

// Optional: for voltage based THC
// const int VOLTAGE_PIN = A0; 

void setup() {
  pinMode(UP_PIN, OUTPUT);
  pinMode(DOWN_PIN, OUTPUT);
  pinMode(MANUAL_UP, INPUT_PULLUP);
  pinMode(MANUAL_DOWN, INPUT_PULLUP);
  // Serial.begin(9600);
}

void loop() {
  // Manual test mode
  if (digitalRead(MANUAL_UP) == LOW) {
    digitalWrite(UP_PIN, HIGH);
    digitalWrite(DOWN_PIN, LOW);
  } else if (digitalRead(MANUAL_DOWN) == LOW) {
    digitalWrite(UP_PIN, LOW);
    digitalWrite(DOWN_PIN, HIGH);
  } else {
    // Default: no movement
    digitalWrite(UP_PIN, LOW);
    digitalWrite(DOWN_PIN, LOW);
  }

  // For real THC: add torch voltage sense and logic here

  delay(20); // debounce
}