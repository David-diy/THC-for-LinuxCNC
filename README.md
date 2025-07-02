# Arduino THC for Plasma CNC (LinuxCNC / QtPlasmaC Compatible)

## Features

- **Voltage-based torch height control** (setpoint & hysteresis)
- **Arc OK logic**: disables up/down if arc lost, passes through to LinuxCNC
- **Manual/Auto mode** switch
- **Opto-isolated relay outputs** for direct connection to parallel port breakout board (BOB)
- **Planned upgrades**: calibration, EEPROM, digital display support

---

## Pinout & Wiring

| Arduino Pin | Function      | Connect To (BOB/Plasma)             |
|-------------|---------------|--------------------------------------|
| D2          | UP Output     | BOB "Torch UP" input (via relay)     |
| D3          | DOWN Output   | BOB "Torch DOWN" input (via relay)   |
| D4          | Arc OK Input  | Plasma "Arc OK" relay (active LOW)   |
| D5          | Arc OK Output | BOB "Arc OK" input (via relay)       |
| D6          | Manual UP Btn | Momentary button to GND              |
| D7          | Manual DN Btn | Momentary button to GND              |
| D8          | Auto/Manual   | Toggle switch to GND (LOW = AUTO)    |
| A0          | Voltage In    | Torch voltage divider output         |
| GND         | GND           | BOB & Plasma GND (Common ground)     |

- Use **opto-isolated relay modules** for outputs to your BOB.
- Use the Everlast 80S's **50:1 divided voltage output** for A0.
- Add a **1kΩ resistor and 0.1 uF ceramic capacitor** between A0 and GND as an RC filter.
- Add a **5V TVS diode** from A0 to GND at the Arduino for surge protection.

---

## LinuxCNC / QtPlasmaC Integration

- Wire Arduino relay outputs to BOB inputs, then BOB to LPT port input pins.
- Use the provided [qtplasmac-thc.hal](qtplasmac-thc.hal) file (edit pin numbers as needed).
- QtPlasmaC handles antidive/THC inhibit automatically, no Arduino changes needed.

---

## Example HAL Connections

```hal
# QtPlasmaC / LinuxCNC HAL for Arduino-based THC
# (Edit parport pin numbers to match your BOB/LPT wiring)

net plasmac-thc-up     plasmac.thc-up     parport.0.pin-11-in
net plasmac-thc-down   plasmac.thc-down   parport.0.pin-12-in
net plasmac-arc-ok     plasmac.arc-ok-in  parport.0.pin-13-in
```

---

## Schematic

See [thc-arduino-schematic.svg](thc-arduino-schematic.svg)

---

## Planned Upgrades

- **Calibration**: Setpoint/hysteresis editable via serial or buttons, stored in EEPROM
- **Digital display**: Show live voltage, setpoint, mode, error, etc.
- **Serial voltage logging**: Send live voltage to PC for logging/advanced display

---

## Safety Notice

- **Never connect plasma high voltage directly to Arduino!**
- Always use shielded cables and opto-isolators for signals.
- Add surge and noise protection as described above.

---