system clock =  8Mhz crystal

This program blinks  PC13 LED @ 0.5Hz using the systick interrupt.
and  Outputs 50Hz PWM signal at PB4 (Timer3 chanal1 output) after remapping it from PA6.
to remap we must enable AFIO clock then disable JTAG and SW first.
Remaping TIM3 CH1 from PA6 to PB4 is done by configuring AFIO_MAPR registre.

The processor goes in sleep mode waiting for systick interrupt.

when systick interrupt occurs the processor handels it. inside the handler PC13 is toggled and duty cycle is incremented and the new value is writen in TIMER3 CCR1 register.

 * PWM setup steps:
 1. Enable TIMx clock from RCC
 2. Enable Alternate function clock from RCC
 3. Enable the relevant GPIO clock from RCC
 4. Make the GPIO pin an alternate-function push-pull output
    (Each TIMx_CHy is connected to a GPIO pin. You can map them to different pins if supported)
    5. Remap the pins if needed from AFIO->MAPR
 6. Set prescaler from PSC
 7. Set period from ARR
 8. Set duty cycle from CCRx
 9. Set the operation MODE from CCMRx register (PWM mode 1 or 2) for the relevant Channel
 10. Enable relevant channel output and polarity from CCER
 11. Enable Master output from BDTR (break and dead-time register) if needed
 If needed:
    12. Enable capture-compare interrupt from DIER
    13. Enable interrupt from NVIC
 14. Enable TIMx module from CR1
 
===========================================================
this program must be loaded at 0x08002000  in flash so Load it using USB stm32duino bootloader with option "-a 2" .

I used YAGARTO to compile the program and DFU-UTIL to load it.

 dfu-util -d 1EAF:0003 -a 2 -D pwm.bin -R
 
