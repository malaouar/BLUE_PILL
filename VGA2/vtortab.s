
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
  // etc etc
	
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

//------------------
/*
    .word     __initial_sp               ; Top of Stack
    .word     Reset_Handler              ; Reset Handler
    .word     NMI_Handler                ; NMI Handler
    .word     HardFault_Handler          ; Hard Fault Handler
    .word     MemManage_Handler          ; MPU Fault Handler
    .word     BusFault_Handler           ; Bus Fault Handler
    .word     UsageFault_Handler         ; Usage Fault Handler
    .word     0                          ; Reserved
    .word     0                          ; Reserved
    .word     0                          ; Reserved
    .word     0                          ; Reserved
    .word     SVC_Handler                ; SVCall Handler
    .word     DebugMon_Handler           ; Debug Monitor Handler
    .word     0                          ; Reserved
    .word     PendSV_Handler             ; PendSV Handler
    .word     SysTick_Handler            ; SysTick Handler

; External Interrupts
    .word     WWDG_IRQHandler            ; Window Watchdog
    .word     PVD_IRQHandler             ; PVD through EXTI Line detect
    .word     TAMPER_IRQHandler          ; Tamper
    .word     RTC_IRQHandler             ; RTC
    .word     FLASH_IRQHandler           ; Flash
    .word     RCC_IRQHandler             ; RCC
    .word     EXTI0_IRQHandler           ; EXTI Line 0
    .word     EXTI1_IRQHandler           ; EXTI Line 1
    .word     EXTI2_IRQHandler           ; EXTI Line 2
    .word     EXTI3_IRQHandler           ; EXTI Line 3
    .word     EXTI4_IRQHandler           ; EXTI Line 4
    .word     DMA1_Channel1_IRQHandler   ; DMA1 Channel 1
    .word     DMA1_Channel2_IRQHandler   ; DMA1 Channel 2
    .word     DMA1_Channel3_IRQHandler   ; DMA1 Channel 3
    .word     DMA1_Channel4_IRQHandler   ; DMA1 Channel 4
    .word     DMA1_Channel5_IRQHandler   ; DMA1 Channel 5
    .word     DMA1_Channel6_IRQHandler   ; DMA1 Channel 6
    .word     DMA1_Channel7_IRQHandler   ; DMA1 Channel 7
    .word     ADC1_2_IRQHandler          ; ADC1_2
    .word     USB_HP_CAN1_TX_IRQHandler  ; USB High Priority or CAN1 TX
    .word     USB_LP_CAN1_RX0_IRQHandler ; USB Low  Priority or CAN1 RX0
    .word     CAN1_RX1_IRQHandler        ; CAN1 RX1
    .word     CAN1_SCE_IRQHandler        ; CAN1 SCE
    .word     EXTI9_5_IRQHandler         ; EXTI Line 9..5
    .word     TIM1_BRK_IRQHandler        ; TIM1 Break
    .word     TIM1_UP_IRQHandler         ; TIM1 Update
    .word     TIM1_TRG_COM_IRQHandler    ; TIM1 Trigger and Commutation
    .word     TIM1_CC_IRQHandler         ; TIM1 Capture Compare
    .word     TIM2_IRQHandler            ; TIM2
    .word     TIM3_IRQHandler            ; TIM3
    .word     TIM4_IRQHandler            ; TIM4
    .word     I2C1_EV_IRQHandler         ; I2C1 Event
    .word     I2C1_ER_IRQHandler         ; I2C1 Error
    .word     I2C2_EV_IRQHandler         ; I2C2 Event
    .word     I2C2_ER_IRQHandler         ; I2C2 Error
    .word     SPI1_IRQHandler            ; SPI1
    .word     SPI2_IRQHandler            ; SPI2
    .word     USART1_IRQHandler          ; USART1
    .word     USART2_IRQHandler          ; USART2
    .word     USART3_IRQHandler          ; USART3
    .word     EXTI15_10_IRQHandler       ; EXTI Line 15..10
    .word     RTCAlarm_IRQHandler        ; RTC Alarm through EXTI Line
    .word     USBWakeUp_IRQHandler       ; USB Wakeup from suspend
*/