
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings 
COPS = -Wall  -O2 -nostdlib -nostartfiles -ffreestanding
COPS +=  -mthumb -mcpu=cortex-m3 -march=armv7-m

all :  a.bin

clean:
	rm -f *.bin *.o *.elf *.list


#---------------------------------

crt0.o : crt0.s
	$(ARMGNU)-as $(AOPS) crt0.s -o crt0.o


spi.o : spi.c
	$(ARMGNU)-gcc $(COPS) -c spi.c -o spi.o
	
dma.o : dma.c
	$(ARMGNU)-gcc $(COPS) -c dma.c -o dma.o
	
main.o : main.c
	$(ARMGNU)-gcc $(COPS) -c main.c -o main.o	


a.bin : linkerscript.ld crt0.o main.o spi.o dma.o
	$(ARMGNU)-ld -T linkerscript.ld crt0.o main.o spi.o  dma.o -o program.elf 
	$(ARMGNU)-objcopy -O binary  program.elf a.bin
	rm -f *.o *.elf



