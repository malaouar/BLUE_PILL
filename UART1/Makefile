
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings 
 -mcpu=cortex-m3
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all :  uart.bin

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


uart.o : uart.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m3 -march=armv7-m -c uart.c -o uart.o


uart.bin : linkerscript.ld crt0.o uart.o
	$(ARMGNU)-ld -o uart.elf -T linkerscript.ld crt0.o uart.o
	$(ARMGNU)-objdump -D uart.elf > uart.list
	$(ARMGNU)-objcopy uart.elf uart.bin -O binary




