#ARMGNU = arm-linux-gnueabi
ARMGNU = arm-none-eabi

AS = $(ARMGNU)-as --warn
CC = $(ARMGNU)-gcc -Wall  -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=cortex-m3 -mthumb

all :  pwm.bin

#---------------------------------

crt0.o : crt0.s
	$(AS) crt0.s -o crt0.o

vectortable.o : vectortable.s
	$(AS) vectortable.s -o vectortable.o

pwm.o : pwm.c
	$(CC) -c pwm.c -o pwm.o
	
pwm.bin : linkerscript.ld vectortable.o crt0.o pwm.o 
	$(ARMGNU)-ld -o pwm.elf -T linkerscript.ld vectortable.o crt0.o pwm.o
	$(ARMGNU)-objdump -D pwm.elf > pwm.list
	$(ARMGNU)-objcopy pwm.elf pwm.bin -O binary

#---------------------------------
# clean up
clean:
	-rm -rf *.bin *.o *.elf *.list



