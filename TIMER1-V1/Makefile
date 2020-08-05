
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings 
 -mcpu=cortex-m3
COPS = -Wall  -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m3

all :  timer1.bin

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

timer1.o : timer1.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m3 -march=armv7-m -c timer1.c -o timer1.o


timer1.bin : linkerscript.ld vectortable.o crt0.o timer1.o
	$(ARMGNU)-ld -o timer1.elf -T linkerscript.ld vectortable.o crt0.o timer1.o
	$(ARMGNU)-objdump -D timer1.elf > timer1.list
	$(ARMGNU)-objcopy timer1.elf timer1.bin -O binary



