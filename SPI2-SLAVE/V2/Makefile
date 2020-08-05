#ARMGNU = arm-linux-gnueabi
ARMGNU = arm-none-eabi

AS = $(ARMGNU)-as --warn
CC = $(ARMGNU)-gcc -Wall  -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=cortex-m3 -mthumb

all :  spi.bin

#---------------------------------

crt0.o : crt0.s
	$(AS) crt0.s -o crt0.o

vtortab.o : vtortab.s
	$(AS) vtortab.s -o vtortab.o

uart.o : uart.c
	$(CC) -c uart.c -o uart.o

main.o : main.c
	$(CC) -c main.c -o main.o
	
spi.bin : linkerscript.ld vtortab.o crt0.o main.o uart.o
	$(ARMGNU)-ld  -T linkerscript.ld vtortab.o crt0.o main.o uart.o -o spi.elf
	$(ARMGNU)-objcopy -O binary spi.elf a.bin 
	-rm -rf  *.o *.elf

#---------------------------------
# clean up
clean:
	-rm -rf *.bin *.o *.elf *.list



