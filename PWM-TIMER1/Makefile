#ARMGNU = arm-linux-gnueabi
ARMGNU = arm-none-eabi

AS = $(ARMGNU)-as --warn
CC = $(ARMGNU)-gcc -Wall  -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=cortex-m3 -mthumb

all :  a.bin

#---------------------------------

crt0.o : crt0.s
	$(AS) crt0.s -o crt0.o

vectortable.o : vectortable.s
	$(AS) vectortable.s -o vectortable.o

pwm.o : pwm.c
	$(CC) -c pwm.c -o pwm.o
	
a.bin : linkerscript.ld vectortable.o crt0.o pwm.o 
	$(ARMGNU)-ld  -T linkerscript.ld vectortable.o crt0.o pwm.o  -o program.elf
	$(ARMGNU)-objcopy -O binary program.elf a.bin 
	-rm -rf  *.o *.elf

#---------------------------------
# clean up
clean:
	-rm -rf *.bin *.o *.elf *.list



