
default:
	mkdir -p build

	avr-gcc \
		-mmcu=atmega328p \
		-DF_CPU=16000000UL \
		-Os \
		-Wall -Wextra \
		-Isrc/include \
		src/main.c \
		src/debug.c \
		src/core.c  \
		src/queue.c	\
		src/tests.c \
		-o build/firmware.elf

	avr-objcopy -O ihex -R .eeprom build/firmware.elf build/firmware.hex

	avrdude -c usbasp -p m328p -B 125kHz \
		-U flash:w:build/firmware.hex:i
