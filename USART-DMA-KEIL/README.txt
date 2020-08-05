DMA memory to USART1 transfer using request from USART1.

This prgram prints "laouar".

hex2bin the generated "dma.hex" in "objs" folder

this program must be loaded at 0x08002000  in flash so Load it using USB stm32duino bootloader with option "-a 2" .

 dfu-util -d 1EAF:0003 -a 2 -D dma.bin -R
 
================
DMA Channel configuration procedure:

1. Set the peripheral register address in the DMA_CPARx register. The data will be
moved from/ to this address to/ from the memory after the peripheral event.
2. Set the memory address in the DMA_CMARx register. The data will be written to or
read from this memory after the peripheral event.
3. Configure the total number of data to be transferred in the DMA_CNDTRx register.
After each peripheral event, this value will be decremented.
4. Configure the channel priority using the PL[1:0] bits in the DMA_CCRx register
5. Configure data transfer direction, circular mode, peripheral & memory incremented
mode, peripheral & memory data size, and interrupt after half and/or full transfer in the
DMA_CCRx register
6. Activate the channel by setting the ENABLE bit in the DMA_CCRx register.

As soon as the channel is enabled, it can serve any DMA request from the peripheral
connected on the channel.
 
Memory-to-memory mode:
The DMA channels can also work without being triggered by a request from a peripheral.
This mode is called Memory to Memory mode.
If the MEM2MEM bit in the DMA_CCRx register is set, then the channel initiates transfers
as soon as it is enabled by software by setting the Enable bit (EN) in the DMA_CCRx register. The transfer stops once the DMA_CNDTRx register reaches zero.

===========================
When you want to send data from a region of memory to a peripheral, you put the starting memory address in the ‘source’ register (DMA_CMARx) and the peripheral’s register address in the ‘destination’ register (DMA_CPARx). The size of the memory region goes in the ‘number of bytes’ register (DMA_CNDTRx).

The ‘configuration’ (DMA_CCRx) register also needs to be set correctly. The MINC and PINC bits tell the peripheral whether it should increment the memory and peripheral addresses after each transfer, respectively. When we send data from memory to a peripheral, we want to set MINC and clear PINC so that the entire area of memory is sent to the same peripheral register.

the MEM2MEM bit sets a special ‘memory-to-memory’ mode which ignores the usual DMA request triggers to transfer data as quickly as possible.
