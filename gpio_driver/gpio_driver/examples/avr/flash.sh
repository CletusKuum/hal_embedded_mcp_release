#!/bin/bash
PORT=${1:-/dev/ttyUSB0}

echo "Flashing AVR Example to $PORT..."
avrdude -p atmega328p -c arduino -P "$PORT" -b 115200 -U flash:w:build/gpio_example.hex:i

if [ $? -ne 0 ]; then
    echo "Flash failed! Check connections and serial port."
    exit 1
fi

echo "Flash successful!"
