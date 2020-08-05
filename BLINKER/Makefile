
ARMGNU = arm-none-eabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m3
COPS = -Wall  -O2 -mcpu=cortex-m3

all :  blinker.bin

crt0.o : crt0.s
	$(ARMGNU)-as $(AOPS) crt0.s -o crt0.o


blinker.o : blinker.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m3 -march=armv7-m -c blinker.c -o blinker.o


blinker.bin : linkerscript.ld crt0.o blinker.o
	$(ARMGNU)-ld -o blinker.elf -T linkerscript.ld crt0.o blinker.o
	$(ARMGNU)-objdump -D blinker.elf > blinker.list
	$(ARMGNU)-objcopy blinker.elf blinker.bin -O binary

#---------------------------------
clean:
	rm -f *.bin *.list *.o *.hex *.elf
