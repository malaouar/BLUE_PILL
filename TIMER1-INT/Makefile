
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings -mcpu=cortex-m3
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mcpu=cortex-m3 -march=armv7-m

all : tim1int.bin

#---------------------------------
crt0.o : crt0.s
	$(ARMGNU)-as $(AOPS) crt0.s -o crt0.o

main.o : main.c
	$(ARMGNU)-gcc $(COPS) -mthumb -c main.c -o main.o

tim1int.bin :       flash.ld crt0.o  main.o
	$(ARMGNU)-ld -T flash.ld crt0.o  main.o -o tim1int.elf
	$(ARMGNU)-objcopy tim1int.elf tim1int.bin -O binary
	rm -f *.elf *.o

#---------------------------------
clean:
	rm -f *.bin *.o *.elf *.list
