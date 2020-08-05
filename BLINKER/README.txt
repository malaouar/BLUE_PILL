 basic blinker for led  on PC13  using internal 8MHz RC oscillator (HSI).
 
I used YAGARTO on WIN7 to compile the program and ST STMFlashLoader Demo Demonstrator GUI to load it.

this program must be loaded at 0x08000000  in flash so Load  it using SERIAL or SWD methodes.
DONT load with stduino bootloader cose it load programs at 0x08002000 (or 0x08005000 )
