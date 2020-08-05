#ARMGNU = arm-linux-gnueabi
ARMGNU = arm-none-eabi

AS = $(ARMGNU)-as --warn
CC = $(ARMGNU)-gcc -Wall  -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=cortex-m3 -mthumb

all :  vga1.bin

#---------------------------------

crt0.o : crt0.s
	$(AS) crt0.s -o crt0.o

vectortable.o : vectortable.s
	$(AS) vectortable.s -o vectortable.o

timers.o : timers.c
	$(CC) -c timers.c -o timers.o
	
dmaspi.o : dmaspi.c
	$(CC) -c dmaspi.c -o dmaspi.o
	
video.o : video.c
	$(CC) -c video.c -o video.o

gdi.o : gdi.c
	$(CC) -c gdi.c -o gdi.o

main.o : main.c
	$(CC) -c main.c -o main.o
	
vga1.bin : linkerscript.ld vectortable.o crt0.o main.o timers.o dmaspi.o video.o gdi.o
	$(ARMGNU)-ld  -T linkerscript.ld vectortable.o crt0.o main.o timers.o dmaspi.o video.o gdi.o -o vga1.elf
	$(ARMGNU)-objcopy -O binary vga1.elf vga1.bin 
	-rm -rf  *.o *.elf

#---------------------------------
# clean up
clean:
	-rm -rf *.bin *.o *.elf *.list



