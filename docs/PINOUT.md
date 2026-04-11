# EtherKraken Pinout

The following tables denote the pinouts of all ports and peripherals on
the EtherKraken v1 Rev. B board.  
The Raspberry Pi Zero 2W daughterboard uses the regular BCM2835 GPIO pinout.

### Status LEDs

| Name      | Description  | Connects to         |
|-----------|--------------|---------------------|
| `ST_LED0` | Status LED 0 | MCP23017 GPB4/Pin 5 |
| `ST_LED1` | Status LED 1 | MCP23017 GPB5/Pin 6 |
| `ST_LED2` | Status LED 2 | MCP23017 GPB6/Pin 7 |
| `ST_LED3` | Status LED 3 | MCP23017 GPB7/Pin 8 |

### PWR Port

| Pin | Name   | Description    | Connects to |
|-----|--------|----------------|-------------|
| 1   | `GND`  |                | GND         |
| 3   | `VCC5` | 5V main supply | VCC5        |
| 5   | `GND`  |                | GND         |
| 2   | `GND`  |                | GND         |
| 4   | `VCC5` | 5V main supply | VCC5        |
| 6   | `GND`  |                | GND         |

### EXT Port

| Pin | Name         | Description                   | Connects to          |
|-----|--------------|-------------------------------|----------------------|
| 1   | `I2C_SCL`    | I²C clock                     | RP3A0 GPIO 3/Pin 5   |
| 3   | `I2C_SDA`    | I²C data                      | RP3A0 GPIO 2/Pin 3   |
| 5   | `GND`        |                               | GND                  |
| 7   | `GND`        |                               | GND                  |
| 9   | `SPI1_CE1`   | SPI1 chip enable 1            | RP3A0 GPIO 17/Pin 11 |
| 11  | `SPI1_CE2`   | SPI1 chip enable 2            | RP3A0 GPIO 16/Pin 36 |
| 13  | `SPI1_MISO`  | SPI1 data from slave          | RP3A0 GPIO 19/Pin 35 |
| 15  | `SPI1_MOSI`  | SPI1 data to slave            | RP3A0 GPIO 20/Pin 38 |
| 17  | `SPI1_SCLK`  | SPI1 clock                    | RP3A0 GPIO 21/Pin 40 |
| 19  | `GND`        |                               | GND                  |
| 2   | `UART_TX`    | UART transmit                 | RP3A0 GPIO 14/Pin 8  |
| 4   | `UART_RX`    | UART receive                  | RP3A0 GPIO 15/Pin 10 |
| 6   | `GND`        |                               | GND                  |
| 8   | `CO_RST`     | Co-processor reset            | RP3A0 GPIO 22/Pin 15 |
| 10  | `CO_BOOT`    | Co-processor boot mode        | RP3A0 GPIO 23/Pin 16 |
| 12  | `CO_SINT`    | Co-processor serial interrupt | RP3A0 GPIO 24/Pin 18 |
| 14  | `AUX0`       | Auxillary GPIO 0              | RP3A0 GPIO 25/Pin 22 |
| 16  | `AUX1`       | Auxillary GPIO 1              | RP3A0 GPIO 26/Pin 37 |
| 18  | `AUX_PWR_EN` | Auxillary power enable (3.3V) | RP3A0 GPIO 27/Pin 13 |
| 20  | `GND`        |                               | GND                  |

### IO0 Port

| Pin | Name    | Description      | Connects to          |
|-----|---------|------------------|----------------------|
| 1   | `GND`   |                  | GND                  |
| 3   | `IO_A0` | Primary GPIO 0   | STM32 PA0/Pin 10     |
| 5   | `IO_A1` | Primary GPIO 1   | STM32 PA1/Pin 11     |
| 7   | `IO_A2` | Primary GPIO 2   | STM32 PA2/Pin 12     |
| 9   | `IO_A3` | Primary GPIO 3   | STM32 PA3/Pin 13     |
| 11  | `IO_A4` | Primary GPIO 4   | STM32 PA4/Pin 14     |
| 13  | `IO_A5` | Primary GPIO 5   | STM32 PA5/Pin 15     |
| 15  | `IO_A6` | Primary GPIO 6   | STM32 PA6/Pin 16     |
| 17  | `IO_A7` | Primary GPIO 7   | STM32 PA7/Pin 17     |
| 19  | `GND`   |                  | GND                  |
| 2   | `GND`   |                  | GND                  |
| 4   | `IO_B0` | Secondary GPIO 0 | MCP23017 GPA0/Pin 21 |
| 6   | `IO_B1` | Secondary GPIO 1 | MCP23017 GPA0/Pin 22 |
| 8   | `IO_B2` | Secondary GPIO 2 | MCP23017 GPA0/Pin 23 |
| 10  | `IO_B3` | Secondary GPIO 3 | MCP23017 GPA0/Pin 24 |
| 12  | `IO_B4` | Secondary GPIO 4 | MCP23017 GPA0/Pin 25 |
| 14  | `IO_B5` | Secondary GPIO 5 | MCP23017 GPA0/Pin 26 |
| 16  | `IO_A8` | Primary GPIO 8   | STM32 PA8/Pin 29     |
| 18  | `IO_A9` | Primary GPIO 9   | STM32 PB0/Pin 18     |
| 20  | `GND`   |                  | GND                  |

### IO1 Port

| Pin | Name     | Description       | Connects to          |
|-----|----------|-------------------|----------------------|
| 1   | `GND`    |                   | GND                  |
| 3   | `IO_A10` | Primary GPIO 10   | STM32 PB1/Pin 19     |
| 5   | `IO_A11` | Primary GPIO 11   | STM32 PB2/Pin 20     |
| 7   | `IO_A12` | Primary GPIO 12   | STM32 PB3/Pin 39     |
| 9   | `IO_A13` | Primary GPIO 13   | STM32 PA13/Pin 34    |
| 11  | `IO_A14` | Primary GPIO 14   | STM32 PA14/Pin 37    |
| 13  | `IO_A15` | Primary GPIO 15   | STM32 PA15/Pin 38    |
| 15  | `IO_A16` | Primary GPIO 16   | STM32 PB4/Pin 40     |
| 17  | `IO_A17` | Primary GPIO 17   | STM32 PB5/Pin 41     |
| 19  | `GND`    |                   | GND                  |
| 2   | `GND`    |                   | GND                  |
| 4   | `IO_B6`  | Secondary GPIO 6  | MCP23017 GPA6/Pin 27 |
| 6   | `IO_B7`  | Secondary GPIO 7  | MCP23017 GPA7/Pin 28 |
| 8   | `IO_B8`  | Secondary GPIO 8  | MCP23017 GPB0/Pin 1  |
| 10  | `IO_B9`  | Secondary GPIO 9  | MCP23017 GPB1/Pin 2  |
| 12  | `IO_B10` | Secondary GPIO 10 | MCP23017 GPB2/Pin 3  |
| 14  | `IO_B11` | Secondary GPIO 11 | MCP23017 GPB3/Pin 4  |
| 16  | `IO_A18` | Primary GPIO 18   | STM32 PB6/Pin 42     |
| 18  | `IO_A19` | Primary GPIO 19   | STM32 PB7/Pin 43     |
| 20  | `GND`    |                   | GND                  |