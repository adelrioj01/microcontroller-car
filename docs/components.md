# Bill of materials

| Quantity | Component | Purpose |
| ---: | --- | --- |
| 1 | STM32L-DISCOVERY board with STM32L152RBT6 | Main controller and ST-LINK programming interface |
| 1 | HC-05 Bluetooth serial module | 9600-baud wireless UART control |
| 1 | L298N dual H-bridge module | Bidirectional control of two DC motors |
| 1 | HC-SR04 ultrasonic sensor | Obstacle-distance measurement |
| 2 | 3–6 V TT geared DC motors | Left and right wheel drive |
| 1 | Two-wheel robot chassis with caster wheel | Mechanical platform |
| 1 | 10 kΩ linear potentiometer | Analog speed selection |
| 1 | 3.3–5 V active buzzer | Proximity warning |
| 1 | 6 V battery pack, such as 4×AA cells | Motor supply |
| 1 | USB cable | Board power, programming, and debugging |
| 1 | Breadboard | Prototyping and signal distribution |
| Several | Male-to-male and male-to-female jumper wires | Electrical connections |
| 1 each | 1 kΩ and 2 kΩ resistors | HC-SR04 echo voltage divider for 3.3 V logic |

## Signal mapping

| STM32 pin | Peripheral signal | Connected component |
| --- | --- | --- |
| PB6 | USART1 TX | HC-05 RX |
| PB7 | USART1 RX | HC-05 TX |
| PB8 | TIM4 channel 3 PWM | L298N motor enable input |
| PB9 | TIM4 channel 4 PWM | L298N motor enable input |
| PA11, PA12 | Motor direction GPIO | L298N direction inputs |
| PB10 | Ultrasonic trigger | HC-SR04 TRIG |
| PA5 | TIM2 channel 1 input capture | HC-SR04 ECHO through voltage divider |
| PA4 | ADC1 channel 4 | 10 kΩ potentiometer wiper |
| PA1 | Digital output | Active buzzer control |

All modules must share a common ground. The HC-SR04 ECHO output is nominally 5 V, so use the resistor divider before connecting it to PA5. If the buzzer or motor-driver input requires more current than a GPIO can safely supply, add an appropriate transistor driver.
