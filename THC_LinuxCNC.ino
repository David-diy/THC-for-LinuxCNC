/*
 * Arduino-based Torch Height Controller (THC) for Plasma CNC
 * - Designed for LinuxCNC/QtPlasmaC with parallel port breakout board
 * - Voltage-based height control, Arc OK input, Up/Down outputs
 * - Manual/Auto mode with switch
 * - Configurable via code (setpoint, hysteresis)
 */

// === Pin assignments ===
const int UP_PIN       = 2;  // Output to BOB "Torch UP"
const int DOWN_PIN     = 3;  // Output to BOB "Torch DOWN"
const int ARC_OK_IN    = 4;  // Input from plasma cutter "Arc OK"
const int ARC_OK_OUT   = 5;  // Output to BOB "Arc OK"
const int MANUAL_UP    = 6;  // Manual UP button (momentary to GND)
const int MANUAL_DOWN  = 7;  // Manual DOWN button (momentary to GND)
const int AUTO_SWITCH  = 8;  // AUTO/MANUAL mode switch (toggle to GND = AUTO)
const int VOLTAGE_PIN  = A0; // Torch voltage divider input

// === Configurable parameters ===
const float V_SETPOINT = 120.0;   // Setpoint voltage (adjust to your system)
const float V_HYST     = 2.5;     // Hysteresis around setpoint (volts)
const float VOLTAGE_DIVIDER_RATIO = 50.0; // Ratio of divider (ex: 50:1)
const float ADC_REF    = 5.0;     // Reference voltage for Arduino ADC
const int   ADC_RES    = 1023;    // 10-bit ADC

// === Internal variables ===
float torch_voltage = 0.0;
bool arc_ok = false;
bool auto_mode = false;

// === Helper: read torch voltage ===
float readTorchVoltage() {
  int raw = analogRead(VOLTAGE_PIN);
  float voltage = (raw * ADC_REF / ADC_RES) * VOLTAGE_DIVIDER_RATIO;
  return voltage;
}

void setup() {
  pinMode(UP_PIN, OUTPUT);
  pinMode(DOWN_PIN, OUTPUT);
  pinMode(ARC_OK_OUT, OUTPUT);
  pinMode(MANUAL_UP, INPUT_PULLUP);
  pinMode(MANUAL_DOWN, INPUT_PULLUP);
  pinMode(AUTO_SWITCH, INPUT_PULLUP);
  pinMode(ARC_OK_IN, INPUT_PULLUP);
  // Serial.begin(9600); // Uncomment for debugging
}

void loop() {
  auto_mode = (digitalRead(AUTO_SWITCH) == LOW); // LOW = AUTO mode
  arc_ok = (digitalRead(ARC_OK_IN) == LOW);      // Adjust if your Arc OK is active high

  digitalWrite(ARC_OK_OUT, arc_ok ? HIGH : LOW); // Pass Arc OK to LinuxCNC

  if (!arc_ok) {
    // Arc not OK: disable up/down
    digitalWrite(UP_PIN, LOW);
    digitalWrite(DOWN_PIN, LOW);
    return;
  }

  if (auto_mode) {
    torch_voltage = readTorchVoltage();

    if (torch_voltage < (V_SETPOINT - V_HYST)) {
      // Torch too low, go UP
      digitalWrite(UP_PIN, HIGH);
      digitalWrite(DOWN_PIN, LOW);
    } else if (torch_voltage > (V_SETPOINT + V_HYST)) {
      // Torch too high, go DOWN
      digitalWrite(UP_PIN, LOW);
      digitalWrite(DOWN_PIN, HIGH);
    } else {
      // Within window: do nothing
      digitalWrite(UP_PIN, LOW);
      digitalWrite(DOWN_PIN, LOW);
    }
  } else {
    // Manual mode
    bool manual_up = (digitalRead(MANUAL_UP) == LOW);
    bool manual_down = (digitalRead(MANUAL_DOWN) == LOW);
    digitalWrite(UP_PIN, manual_up ? HIGH : LOW);
    digitalWrite(DOWN_PIN, manual_down ? HIGH : LOW);
  }

  delay(20); // debounce, update rate
}