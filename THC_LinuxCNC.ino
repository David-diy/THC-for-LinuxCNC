#include <LiquidCrystal.h>

// LCD: RS=8, E=9, D4=10, D5=11, D6=12, D7=13
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

// Button pins (connect one side to digital pin, other to GND)
#define BTN_RIGHT 2
#define BTN_UP    3
#define BTN_DOWN  4
#define BTN_LEFT  5
#define BTN_SELECT 6

// Menu items
enum Menu {
  SET_VOLTAGE, SET_DEADBAND, SET_ZSPEED, SET_RESPONSE,
  SET_FILTER, SET_MINZ, SET_MAXZ, THC_ENABLE, RESTORE_DEFAULTS, SYSTEM_INFO,
  N_MENU_ITEMS
};
const char* menuLabels[N_MENU_ITEMS] = {
  "Volt Setpoint",
  "Deadband",
  "Z Speed",
  "Response Delay",
  "Filter",
  "Min Z",
  "Max Z",
  "THC Enable",
  "Restore Def",
  "System Info"
};

// Settings
float setVoltage = 120.0;
float deadband = 2.0;
float zSpeed = 5.0;
float responseDelay = 0.2;
int filter = 5;
int minZ = 0;
int maxZ = 100;
bool thcEnabled = true;

// Button logic for digital pins (active LOW)
enum Button { NONE, RIGHT, UP, DOWN, LEFT, SELECT };
Button readButton() {
  if (digitalRead(BTN_RIGHT) == LOW)  return RIGHT;
  if (digitalRead(BTN_UP)    == LOW)  return UP;
  if (digitalRead(BTN_DOWN)  == LOW)  return DOWN;
  if (digitalRead(BTN_LEFT)  == LOW)  return LEFT;
  if (digitalRead(BTN_SELECT)== LOW)  return SELECT;
  return NONE;
}

int menuIndex = 0;
bool inSubMenu = false;
bool needRedraw = true;

// Prints the value for submenu (2nd line)
void printSubValue(int idx) {
  lcd.setCursor(0,1);
  lcd.print("                "); // Clear
  lcd.setCursor(0,1);
  switch(idx) {
    case SET_VOLTAGE:   lcd.print("Set: "); lcd.print(setVoltage,1); lcd.print("V "); break;
    case SET_DEADBAND:  lcd.print("Set: "); lcd.print(deadband,1); lcd.print("V "); break;
    case SET_ZSPEED:    lcd.print("Set: "); lcd.print(zSpeed,1); lcd.print("mm/s"); break;
    case SET_RESPONSE:  lcd.print("Set: "); lcd.print(responseDelay,2); lcd.print("s "); break;
    case SET_FILTER:    lcd.print("Set: "); lcd.print(filter); break;
    case SET_MINZ:      lcd.print("Set: "); lcd.print(minZ); break;
    case SET_MAXZ:      lcd.print("Set: "); lcd.print(maxZ); break;
    case THC_ENABLE:    lcd.print("Set: "); lcd.print(thcEnabled ? "ON " : "OFF"); break;
    case RESTORE_DEFAULTS: lcd.print("Press SELECT!"); break;
    case SYSTEM_INFO:   lcd.print("FW 1.0 By David"); break;
  }
}

void drawMenu() {
  lcd.clear();
  if (!inSubMenu) {
    lcd.setCursor(0,0); lcd.print(">"); lcd.print(menuLabels[menuIndex]);
    lcd.setCursor(0,1); lcd.print("SEL=Edit  ^/v Nav");
  } else {
    lcd.setCursor(0,0); lcd.print(menuLabels[menuIndex]);
    printSubValue(menuIndex);
    if (menuIndex != RESTORE_DEFAULTS && menuIndex != SYSTEM_INFO) {
      lcd.setCursor(15,1); lcd.write((byte)0x7E); // right arrow
    }
  }
  needRedraw = false;
}

void setup() {
  lcd.begin(16,2);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_UP,    INPUT_PULLUP);
  pinMode(BTN_DOWN,  INPUT_PULLUP);
  pinMode(BTN_LEFT,  INPUT_PULLUP);
  pinMode(BTN_SELECT,INPUT_PULLUP);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("THC Main Menu");
  lcd.setCursor(0,1);
  lcd.print("SEL=Edit  ^/v Nav");
  delay(3000);
  needRedraw = true;
}

void loop() {
  static Button lastButton = NONE;
  static unsigned long lastDebounce = 0;
  const unsigned long debounceDelay = 150; // ms

  Button b = readButton();
  unsigned long now = millis();

  // Only react on transitions: button goes from released to pressed
  if (b != NONE && lastButton == NONE && (now - lastDebounce) > debounceDelay) {
    lastDebounce = now;
    if (!inSubMenu) {
      // Main menu navigation
      if (b == UP)   { menuIndex = (menuIndex + N_MENU_ITEMS - 1) % N_MENU_ITEMS; needRedraw = true; }
      if (b == DOWN) { menuIndex = (menuIndex + 1) % N_MENU_ITEMS; needRedraw = true; }
      if (b == SELECT) { inSubMenu = true; needRedraw = true; }
    } else {
      // In submenu
      switch(menuIndex) {
        case SET_VOLTAGE:
          if (b == LEFT && setVoltage > 0.5) { setVoltage -= 0.5; needRedraw = true; }
          if (b == RIGHT && setVoltage < 300) { setVoltage += 0.5; needRedraw = true; }
          break;
        case SET_DEADBAND:
          if (b == LEFT && deadband > 0.1) { deadband -= 0.1; needRedraw = true; }
          if (b == RIGHT && deadband < 10.0) { deadband += 0.1; needRedraw = true; }
          break;
        case SET_ZSPEED:
          if (b == LEFT && zSpeed > 0.5) { zSpeed -= 0.5; needRedraw = true; }
          if (b == RIGHT && zSpeed < 50.0) { zSpeed += 0.5; needRedraw = true; }
          break;
        case SET_RESPONSE:
          if (b == LEFT && responseDelay > 0.01) { responseDelay -= 0.01; needRedraw = true; }
          if (b == RIGHT && responseDelay < 1.0) { responseDelay += 0.01; needRedraw = true; }
          break;
        case SET_FILTER:
          if (b == LEFT && filter>1) { filter--; needRedraw = true; }
          if (b == RIGHT && filter<99) { filter++; needRedraw = true; }
          break;
        case SET_MINZ:
          if (b == LEFT && minZ>0) { minZ--; needRedraw = true; }
          if (b == RIGHT && minZ<maxZ-1) { minZ++; needRedraw = true; }
          break;
        case SET_MAXZ:
          if (b == LEFT && maxZ>minZ+1) { maxZ--; needRedraw = true; }
          if (b == RIGHT && maxZ<200) { maxZ++; needRedraw = true; }
          break;
        case THC_ENABLE:
          if (b == LEFT || b == RIGHT) { thcEnabled = !thcEnabled; needRedraw = true; }
          break;
        case RESTORE_DEFAULTS:
          if (b == SELECT) {
            setVoltage = 120.0; deadband = 2.0; zSpeed = 5.0; responseDelay = 0.2;
            filter = 5; minZ = 0; maxZ = 100; thcEnabled = true;
            needRedraw = true;
          }
          break;
        case SYSTEM_INFO:
          break;
      }
      // Return to main menu (except for restore/defaults which requires SELECT to act)
      if ((b == SELECT && menuIndex != RESTORE_DEFAULTS && menuIndex != SYSTEM_INFO) ||
          (b == LEFT && (menuIndex == SYSTEM_INFO || menuIndex == RESTORE_DEFAULTS))) {
        inSubMenu = false;
        needRedraw = true;
      }
    }
  }
  lastButton = b;

  if (needRedraw) drawMenu();
}
