setup for  using 8MHz Crystal + PLL 72MHz. 
Output PLL/2 at PA8 pin (MCO)

SPI initialization in 8bit mode + transmission (send 0xAA) at MOSI (pin PA7)
SPI_BaudRatePrescaler = 0x0020 ==> SPI CLK = 72MHz/32
SCK = 2.25MHz at pin PA5


this program must be loaded at 0x08002000  in flash so Load  it using USB stm32duino bootloader with option "-a 2" .

I used YAGARTO to compile the program and DFU-UTIL to load it.

 dfu-util -d 1EAF:0003 -a 2 -D a.bin -R


