
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings 
 -mcpu=cortex-m3
COPS = -Wall  -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m3

all :  blinker.bin

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.bc
	rm -f *.opt.s
	rm -f *.norm.s
	rm -f *.hex

#---------------------------------

crt0.o : crt0.s
	$(ARMGNU)-as $(AOPS) crt0.s -o crt0.o

vectortable.o : vectortable.s
	$(ARMGNU)-as $(AOPS) vectortable.s -o vectortable.o

blinker.o : blinker.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m3 -march=armv7-m -c blinker.c -o blinker.o


blinker.bin : linkerscript.ld vectortable.o crt0.o blinker.o
	$(ARMGNU)-ld -o blinker.elf -T linkerscript.ld vectortable.o crt0.o blinker.o
	$(ARMGNU)-objdump -D blinker.elf > blinker.list
	$(ARMGNU)-objcopy blinker.elf blinker.bin -O binary



