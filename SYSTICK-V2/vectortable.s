
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
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word SysTick_handler
	.word 0
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
