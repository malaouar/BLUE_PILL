
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m3
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -mcpu=cortex-m3 -march=armv7-m

all :  uart.bin

#---------------------------------

crt0.o : crt0.s
	$(ARMGNU)-as $(AOPS) crt0.s -o crt0.o


uart.o : uart.c
	$(ARMGNU)-gcc $(COPS)  -c uart.c -o uart.o


uart.bin : linkerscript.ld crt0.o uart.o
	$(ARMGNU)-ld -o uart.elf -T linkerscript.ld crt0.o uart.o
	$(ARMGNU)-objcopy uart.elf a.bin -O binary
	rm -f *.o *.elf *.list

#---------------------------------

clean:
	rm -f *.bin *.o *.bin *.elf *.list

