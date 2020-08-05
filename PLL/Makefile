
ARMGNU = arm-none-eabi
#ARMGNU = arm-linux-gnueabi

AOPS = --warn --fatal-warnings 
 -mcpu=cortex-m3
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding 
 -mcpu=cortex-m0

all :  pll.bin

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


pll.o : pll.c
	$(ARMGNU)-gcc $(COPS) -mthumb -mcpu=cortex-m3 -march=armv7-m -c pll.c -o pll.o


pll.bin : linkerscript.ld crt0.o pll.o
	$(ARMGNU)-ld -o pll.elf -T linkerscript.ld crt0.o pll.o
	$(ARMGNU)-objdump -D pll.elf > pll.list
	$(ARMGNU)-objcopy pll.elf pll.bin -O binary




