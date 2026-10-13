
default:
	avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os -o blink.elf blink.c

	avr-objcopy -O ihex -R .eeprom blink.elf blink.hex

	avrdude -c usbasp -p m328p -B 125kHz -U flash:w:blink.hex:i
