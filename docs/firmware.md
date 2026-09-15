# Firmware reference

## Technical details

- Microcontroller: STM32L152RBT6
- Programming language: C
- IDE/toolchain: STM32CubeIDE
- Wireless module and protocol: HC-05 Bluetooth module over USART1 at 9600 baud, 8-N-1
- Motor driver: L298N dual H-bridge module
- Sensors: HC-SR04 ultrasonic sensor and 10 kΩ potentiometer
- Power source: USB for the controller and a 6 V battery pack for the motors, with a common ground

## Command map

| Input | Vehicle behavior |
| --- | --- |
| `1` | Stop |
| `2` | Forward |
| `3` | Reverse |
| `4` | Turn left |
| `5` | Turn right |
| `6` | Autonomous mode |

## Build notes

- Pin assignments: documented in `docs/hardware.md`
- Required libraries: STM32L1 HAL and CMSIS drivers included in `firmware/Drivers`
- Flashing procedure: import `firmware/` into STM32CubeIDE, build, and program through the board's integrated ST-LINK interface
- Pairing procedure: pair the phone with the HC-05 and open a Bluetooth serial terminal configured for the module
- Known limitations: open-loop motor control; fixed obstacle thresholds; no wheel encoders or closed-loop speed feedback
