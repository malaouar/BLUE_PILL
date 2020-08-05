
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings 
 -mcpu=cortex-m3
COPS = -Wall  -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m3

all :  timer2.bin

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

timer2.o : timer2.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m3 -march=armv7-m -c timer2.c -o timer2.o


timer2.bin : linkerscript.ld vectortable.o crt0.o timer2.o
	$(ARMGNU)-ld -o timer2.elf -T linkerscript.ld vectortable.o crt0.o timer2.o
	$(ARMGNU)-objdump -D timer2.elf > timer2.list
	$(ARMGNU)-objcopy timer2.elf timer2.bin -O binary



