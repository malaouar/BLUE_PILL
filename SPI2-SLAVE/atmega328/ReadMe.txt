This is an SPI Master.
PB2 ===> NSS
PB3 ===> MOSI
PB5 ===> CLK

PB0 out put for LED, toggled after sending a char.

It sends the letters L, A, O, U, A, R one every 1s
then sends the letter M every 1s in an infinite loop

Spi configured for Max speed (CLK/2).

Internal clock 8Mhz
Powered by 3.3V