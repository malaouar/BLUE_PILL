
	.syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.thumb
	
	.global vtable
	.global default_interrupt_handler
	
	/* ========= The vector table =========  */
	.type vtable, %object
	.section .vector_table,"a",%progbits
vtable:
	// --------- 0-15 --------------------
	.word 0x20005000       ;@ stack pointer at top of ram + 1 (stack full descending)
	.word reset_handler
	.word NMI_handler
	.word hard_fault_handler
	.word 0       ;@  	 MemManage
	.word 0       ;@  	 Bus Fault 
	.word 0       ;@  	 Usage Fault
	.word 0       ;@  	 Reserved 
	.word 0       ;@  	 Reserved
	.word 0       ;@  	 Reserved
	.word 0       ;@  	 Reserved
	.word 0       ;@  	 SVCall
	.word 0       ;@  	 Reserved
	.word 0       ;@  	 Debug monitor
	.word 0       ;@  	 PendSV
	.word SysTick_handler       ;@      SysTick	
  // External interrupt handlers follow 
	.word 0       ;@   	 0: WWDG
	.word 0       ;@   	 1: PVD
	.word 0       ;@   	 2: Tamper
	.word 0       ;@   	 3: RTC
	.word 0       ;@   	 4: Flash
	.word 0       ;@   	 5: RCC
	.word 0       ;@   	 6: EXTI Line 0 
	.word 0       ;@   	 7: EXTI Line 1
	.word 0       ;@   	 8: EXTI Line 2
	.word 0       ;@   	 9: EXTI Line 3
	.word 0       ;@   	 10: EXTI Line 4
	.word 0       ;@   	 11: DMA1 Channel 1
	.word 0       ;@   	 12: DMA1 Channel 2
	.word DMA1_Channel3_IRQHandler       ;@   	 13: DMA1 Channel 3
	.word 0       ;@   	 14: DMA1 Channel 4
	.word 0       ;@   	 15: DMA1 Channel 5
	.word 0       ;@   	 16: DMA1 Channel 6
	.word 0       ;@   	 17: DMA1 Channel 7
	.word 0       ;@   	 18: ADC1 and ADC2
	.word 0       ;@   	 19: USB High Priority or CAN TX interrupts
	.word 0       ;@   	 20: USB Low Priority or CAN RX0 interrupts
	.word 0       ;@   	 21: CAN RX1 interrupt
	.word 0       ;@   	 22: CAN SCE interrupt
	.word 0       ;@   	 23: EXTI Line[9:5] interrupts
	.word 0       ;@   	 24: TIM1 Break interrupt
	.word 0       ;@   	 25: TIM1 Update interrupt
	.word 0       ;@   	 26: TIM1 Trigger and Commutation interrupts
	.word TIM1_CC_IRQHandler       ;@   	 27: TIM1 Capture Compare interrupt
	.word TIM2_IRQHandler       ;@     28: TIM2 global interrupt
	.word 0       ;@   	 29: TIM3 global interrupt
	.word TIM4_IRQHandler       ;@   	 30: TIM4 global interrupt
    .word 0       ;@   	 31: I2C1 Event
    .word 0       ;@   	 32: I2C1 Error
    .word 0       ;@   	 33: I2C2 Event
    .word 0       ;@   	 34: I2C2 Error
    .word 0       ;@   	 35: SPI1
    .word SPI2_IRQHandler            ;@   	 36: SPI2
    .word 0       ;@   	 37: USART1
    .word 0       ;@   	 38: USART2
    .word 0       ;@   	 39: USART3
    .word 0       ;@   	 40: EXTI Line 15..10
    .word 0       ;@   	 41: RTC Alarm through EXTI Line
    .word 0       ;@   	 42: USB Wakeup from suspend 
  
	
	/*  Setup weak aliases for each exception handler to the  default one. These can be updated later, or just
      overridden since they're weak refs.  The reset_handler is set up separately.
	*/
	.weak NMI_handler
	.thumb_set NMI_handler,default_interrupt_handler
	
	.weak hard_fault_handler
	.thumb_set hard_fault_handler,default_interrupt_handler
	// etc etc
	
	/*  A 'Default' interrupt handler. This is where interrupts  which are not otherwise configured will go.
	    It is an infinite loop, because...well, we weren't expecting the interrupt, so what can we do?
	*/
	.section .text.default_interrupt_handler,"ax",%progbits
default_interrupt_handler:
	B . ;@ loop forever
	.size default_interrupt_handler, .-default_interrupt_handler
