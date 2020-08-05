#ARMGNU = arm-linux-gnueabi
ARMGNU = arm-none-eabi

AS = $(ARMGNU)-as --warn
CC = $(ARMGNU)-gcc -Wall  -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=cortex-m3 -mthumb

all :  timer1.bin

#---------------------------------

crt0.o : crt0.s
	$(AS) crt0.s -o crt0.o

vectortable.o : vectortable.s
	$(AS) vectortable.s -o vectortable.o

timer1.o : timer1.c
	$(CC) -c timer1.c -o timer1.o

nvic.o : nvic.c
	$(CC) -c nvic.c -o nvic.o

uart.o : uart.c
	$(CC) -c uart.c -o uart.o
	
timer1.bin : linkerscript.ld vectortable.o crt0.o timer1.o uart.o nvic.o
	$(ARMGNU)-ld -o timer1.elf -T linkerscript.ld vectortable.o crt0.o timer1.o uart.o nvic.o
	$(ARMGNU)-objdump -D timer1.elf > timer1.list
	$(ARMGNU)-objcopy timer1.elf timer1.bin -O binary

#---------------------------------
# clean up
clean:
	-rm -rf *.bin *.o *.elf *.list



