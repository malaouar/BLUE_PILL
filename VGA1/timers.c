#include "periph.h" 

void TIM1_Config(){
	uint16_t tmpcr1 = 0; 
	uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;
	uint32_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;
  
	uint16_t TimerPeriod = 2048; 		//Timer1 period
	uint16_t Channel1Pulse = 144;		// HSYNC pulse width
	uint16_t Channel2Pulse = 391; 		/* HSYNC + BACK PORCH */  
	uint16_t TIM1_CC_IRQn = 27;

	/* Select the Counter Mode */
	tmpcr1 = TIM1->CR1;  
    tmpcr1 &= (uint16_t)(~((uint16_t)(0x0010 | 0x0060))); // clear DIR and CMS
    tmpcr1 |= 0x0000; // Counter Mode Up 
    /* Set the clock division */
    tmpcr1 &= (uint16_t)(~((uint16_t)0x0300));  // clear CKD
    tmpcr1 |= (uint32_t)0; // Clock Division;
    TIM1->CR1 = tmpcr1;

  /* Set the Autoreload value */ 
  TIM1->ARR = TimerPeriod; // Period 
 
  /* Set the Prescaler value */
  TIM1->PSC = 0;    //Prescaler
    
   /* Set the Repetition Counter value */
    TIM1->RCR = 0; //Repetition Counter

  /* Generate an update event to reload the Prescaler and the Repetition counter
     values immediately */
  TIM1->EGR = (uint16_t)0x0001;   // TPSC Reload Mode Immediate;      
		
 /* Disable the Channel 1: Reset the CC1E Bit */
  TIM1->CCER &= (uint16_t)(~(uint16_t)0x0001); // CCER_CC1E
  /* Get the TIMER1 CCER register value */
  tmpccer = TIM1->CCER;
  /* Get the TIMER1 CR2 register value */
  tmpcr2 =  TIM1->CR2;
  
  /* Get the TIMER1 CCMR1 register value */
  tmpccmrx = TIM1->CCMR[0];
    
  /* Reset the Output Compare Mode Bits */
  tmpccmrx &= (uint16_t)(~((uint16_t)0x0070));  // CCMR1_OC1M
  tmpccmrx &= (uint16_t)(~((uint16_t)0x0003)); // CCMR1_CC1S

  /* Select the Output Compare Mode */
  tmpccmrx |= (uint16_t)0x0070;  // OCMode PWM2 
  
  /* Reset the Output Polarity level */
  tmpccer &= (uint16_t)(~((uint16_t)0x0002)); // TCCER_CC1P
  /* Set the Output Compare Polarity */
  tmpccer |= 0x0002;  // OC Polarity low
  
  /* Set the Output State */
  tmpccer |= (uint16_t)0x0001; //Output State enable
       
    /* Reset the Output N Polarity level */
    tmpccer &= (uint16_t)(~((uint16_t)0x0008));  //  TCCER_CC1NP
    /* Set the Output N Polarity */
    tmpccer |=   (uint16_t)0x0000; // OCN Polarity high
    
    /* Reset the Output N State */
    tmpccer &= (uint16_t)(~((uint16_t)0x0004));  // CCER_CC1NE    
    /* Set the Output N State */
    tmpccer |= 0x0004;  // Output N_State enable
    
    /* Reset the Output Compare and Output Compare N IDLE State */
    tmpcr2 &= (uint16_t)(~((uint16_t)0x0100)); //TIM_CR2_OIS1
    tmpcr2 &= (uint16_t)(~((uint16_t)0x0200));  // TIM_CR2_OIS1N
    
    /* Set the Output Idle state */
    tmpcr2 |= (uint16_t)0x0000; //TIM_OCIdleState_Reset
    /* Set the Output N Idle state */
    tmpcr2 |= (uint16_t)0x0100;  //TIM_OCIdleState_Set
 
  /* Write to TIMER1 CR2 */
  TIM1->CR2 = tmpcr2;
  
  /* Write to TIMER1 CCMR1 */
  TIM1->CCMR[0] = tmpccmrx;

  /* Set the Capture Compare Register value */
  TIM1->CCR[0] = Channel1Pulse; // pulse width
 
  /* Write to TIMER1 CCER */ 
  TIM1->CCER = tmpccer;		

//*****************  CONFIG TIMER1 CHANNEL 2  **********************
   /* Disable the Channel 2: Reset the CCER_CC2E Bit4 Capture/Compare 2 output enable */
  TIM1->CCER &= (uint16_t)(~((uint16_t)0x0010)); // CC2E = 0 => OC2 is not active (off)

  /* Get the TIMER1 CCER register value */  
  tmpccer = TIM1->CCER;
  /* Get the TIMER1 CR2 register value */  
  tmpcr2 =  TIM1->CR2;
  
  /* Get the TIMER1 CCMR1 register value */ 
  tmpccmrx = TIM1->CCMR[0];
    
  /* Reset the Output Compare mode and Capture/Compare selection Bits */
  tmpccmrx &= (uint16_t)(~((uint16_t)0x7000)); // Bits(14:12) OC2M[2:0] =000 => Output Compare 2 mode = Frozen
  tmpccmrx &= (uint16_t)(~((uint16_t)0x0300)); // Bits(9:8) CC2S[1:0] =00 => CC2 channel is configured as output
  
  /* Select the Output Compare Mode */
  tmpccmrx |= (uint16_t)(0x0020 << 8); // OCMode_Inactive
  
  /* Reset the CC2 Output Polarity level: CCER_CC2P (Bit5): CC2 output polarity */ 
  tmpccer &= (uint16_t)(~((uint16_t)0x0020)); // Bit5 = 0 => CC2 active high 
  /* Set the CC2 Output  Polarity */
  tmpccer |= (uint16_t)(0x0002 << 4); // Bit5 = 1 => CC2 active low
  
  /* Set the Output State CCER_CC2E (Bit4): CC2 output enable */
  tmpccer |= (uint16_t)(0x0001 << 4);  // Bit4 = 1 
        
    /* Reset the CCER_CC2NP (Bit7): CC2 complementary output polarity level */
    tmpccer &= (uint16_t)(~((uint16_t)0x0080)); // Bit7 = 0 => OC2N active high
    /* Set CC2NP Output Polarity */
    tmpccer |= (uint16_t)(0x0000 << 4); // Bit7 = 0 => OC2N active high
    
    /* Reset the CCER_CC2NE (Bit6): CC2N  output State */
    tmpccer &= (uint16_t)(~((uint16_t)0x0040));   //  Bit6 = 0 => OC2N is not active (Off)
 
    /* Set the Output N State */
    tmpccer |= (uint16_t)(0x0004 << 4); // Bit6 = 0 => OC2N signal is output (On)
    
    /* Reset the CR2_OIS2 (Bit10): Output Idle state 2 (OC2 output) Output Compare */
    tmpcr2 &= (uint16_t)(~((uint16_t)0x0400)); // Bit10 = 0 => OC2=0 when MOE=0
	/* Reset the CR2_OIS2N (Bit11): Output Idle state 2 (OC2N output)  Output Compare N  */	
    tmpcr2 &= (uint16_t)(~((uint16_t)0x0800)); // Bit11 = 0 => OC2N=0  when MOE=0
    
    /* Set the Output Idle state */
    tmpcr2 |= (uint16_t)(0x0000 << 2); // OIS2 (Bit10) = 0 => IdleState_Reset
    /* Set the Output N Idle state */
    tmpcr2 |= (uint16_t)(0x0100 << 2); // OIS2N (Bit11) = 1 => IdleState_Set
 
  /* Write to TIMER1 CR2 */
  TIM1->CR2 = tmpcr2;
  
  /* Write to TIMER1 CCMR1 */
  TIM1->CCMR[1] = tmpccmrx;

  /* Set the channel2 Capture Compare Register value */ 
  TIM1->CCR[1] = Channel2Pulse;
  
  /* Write to TIMER1 CCER */
  TIM1->CCER = tmpccer;

//---------------- 	/* Select TIMER1 as Master */ -----------------

  /* Reset the Master/slave mode: SMCR_MSM (Bit7)  */
  TIM1->SMCR &= (uint16_t)~((uint16_t)(uint16_t)0x0080);  // Bit7 = 0 clear MSM bit
  
  /* Set the MSM Bit */
  TIM1->SMCR |= (uint16_t)0x0080;  // MSM=1 => MasterSlaveMode Enable: synchronization timer and its slaves (through TRGO).

  /* Reset the Master mode selection bits: CR2_MMS  Bits(6:4)   */
  TIM1->CR2 &= (uint16_t)~((uint16_t)0x0070);   // bits6:4 = 000 => reset
  /* Select the TRGO source */
  TIM1->CR2 |=  (uint16_t)0x0020 ; // bits6:4 = 010 => The update event is selected as trigger output (TRGO).
									// a master timer can then be used as a prescaler for a slave timer.

// ----------	/* Interrupt TIMER1 */ ------------------------
  
    // Enable the Selected IRQ Channels 
    NVIC->ISER[TIM1_CC_IRQn >> 5] = (uint32_t)(0x01 << (TIM1_CC_IRQn & 0x1F)); 

    // Enable the Capture/Compare 2 interrupt DIER_CC2IE (Bit2) 
    TIM1->DIER |= (uint16_t)0x0004; // Bit2 = 1 => CC2 interrupt enabled

//-------------------------------------------------------
	
   /* Enable the TIM Main Output: BDTR_MOE (Bit15) */
    TIM1->BDTR |= (uint16_t)0x8000;  // Bit15 = 1 => Main Output enable
	
	/* Enable the TIM Counter CR1_CEN (Bit0)*/
    TIM1->CR1 |= (uint16_t)0x0001;  //Bit0 = 1 => Counter enable
	
}

//************************************************************************

void TIM2_Config(){
	/* VSYNC (TIM2_CH2) and VSYNC_BACKPORCH (TIM2_CH3) */
	/* Channel 2 and 3 Configuration in PWM mode */
	
	uint16_t tmpsmcr = 0;  uint16_t tmpcr1 = 0;
	uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;
	uint32_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;
	uint16_t TIM2_IRQn = 28;
	
	uint16_t TimerPeriod = 625;		// Vertical lines:   625 original
	uint16_t Channel2Pulse = 2;		// Sync pulse:   2 original
	uint16_t Channel3Pulse = 24;	// Sync pulse + Back porch:   24 original

 /* Reset the SMCR_SMS (Bits2:0) Slave mode selection */
  TIM2->SMCR &= (uint16_t)~((uint16_t)0x0007); // Bits2:0 = 000 => Slave mode disabled
  /* Select the Slave Mode */
  TIM2->SMCR |= 0x0005; // Bits2:0 = 101 => Gated Mode - The counter clock is enabled when the trigger input (TRGI) is high.

  /* Get the TIM2_SMCR register value */ 
  tmpsmcr = TIM2->SMCR;
  /* Reset the SMCR_TS[2:0] (Bits6:4)  (Trigger selection)  */
  tmpsmcr &= (uint16_t)(~((uint16_t)0x0070)); // Bits6:4 = 000 => Internal Trigger 0 (ITR0)
  /* Set the Input Trigger source */
  tmpsmcr |= 0x0000;  // Bits6:4 = 000 => Internal Trigger 0 (ITR0)
  /* Write to TIM2 SMCR */
  TIM2->SMCR = tmpsmcr;
	
//------------------------------- 
	tmpcr1 = TIM2->CR1;  

    /* Select the Counter Mode */
	// reset CR1_DIR (direction) and CR1_CMS (Center-aligned mode)
    tmpcr1 &= (uint16_t)(~((uint16_t)(0x0010 | 0x0060))); // DIR bit4 = 0 => count up, CMS bits6:5 = 00
    //tmpcr1 |= (uint32_t)0x0000; // CounterMode_Up
 
    /* Set the clock division */
    tmpcr1 &= (uint16_t)(~((uint16_t)0x0300)); // reset CR1_CKD[1:0] bits9:8 (clock division)
    //tmpcr1 |= 0;

  TIM2->CR1 = tmpcr1;

  /* Set the Autoreload value */
  TIM2->ARR = TimerPeriod ; // 625 lines
 
  /* Set the Prescaler value */
  TIM2->PSC = 0;    

  /* Generate an update event to reload the Prescaler and the Repetition counter
     values immediately */
  TIM2->EGR = (uint16_t)0x0001; // EGR_UG (Bit0) = 1 PSC Reload Mode Immediate            

//------------------------  Channel 2 ----------------
     
   /* Disable the Channel 2: Reset the CCER_CC2E Bit4 */
  TIM2->CCER &= (uint16_t)(~((uint16_t)0x0010)); // Bit4 = 0 => OC2 is not active (Off)
  
  /* Get the TIM2 CCER register value */  
  tmpccer = TIM2->CCER;
  /* Get the TIM2 CR2 register value */
  tmpcr2 =  TIM2->CR2;
  
  /* Get the TIM2 CCMR1 register value */
  tmpccmrx = TIM2->CCMR[0];
    
  /* Reset the Output Compare mode: CCMR1_OC2M[2:0] (bits14:12) */
  tmpccmrx &= (uint16_t)(~((uint16_t)0x7000));  // OC2M[2:0] = 000 => OC2 mode Frozen
  /* Reset the Output Capture/Compare: CCMR1_CC2S[1:0] (bits9:8) */
  tmpccmrx &= (uint16_t)(~((uint16_t)0x0300)); // CC2S[1:0]= 00 => CC2 channel is configured as output
  
  /* Select the Output Compare Mode */
  tmpccmrx |= (uint16_t)(0x0070 << 8); // bits14:12 = 111 => PWM mode 2 
  
  /* Reset the Output Polarity level: CCER_CC2P (Bit5)  */
  tmpccer &= (uint16_t)(~((uint16_t)0x0020)); // Bit5 = 0 => OC2 active high
  /* Set the Output Compare Polarity */
  tmpccer |= (uint16_t)(0x0002 << 4); // Bit5 = 0 => OC2 active Low
  
  /* Set the Output State CCER_CC2E (Bit4) */
  tmpccer |= (uint16_t)(0x0001 << 4); // Bit4 = 1 => OC2 On     

  /* Write to TIM2 CR2 */
  TIM2->CR2 = tmpcr2;
  
  /* Write to TIM2 CCMR1 */
  TIM2->CCMR[0] = tmpccmrx;

  /* Set the Capture Compare Register value */
  TIM2->CCR[1] = Channel2Pulse; // VSYNC pulse width
  
  /* Write to TIM2 CCER */
  TIM2->CCER = tmpccer;

//-----------------------   Channel 3 ----------------------	   
 
  /* Disable the Channel 2: Reset the CCER_CC3E (Bit8) */
  TIM2->CCER &= (uint16_t)(~((uint16_t)0x0100)); // Bit8 = 0 => OC3 Off
  
  /* Get the TIM2 CCER register value */
  tmpccer = TIM2->CCER;
  /* Get the TIM2 CR2 register value */
  tmpcr2 =  TIM2->CR2;
  
  /* Get the TIM2 CCMR2 register value */
  tmpccmrx = TIM2->CCMR[1];
    
  /* Reset the Output Compare mode  CCMR2_OC3M[2:0] (Bits6:4)*/
  tmpccmrx &= (uint16_t)(~((uint16_t)0x0070)); // Bits6:4 = 000 => OC3 mode Frozen
  /* Reset the Capture/Compare selection CCMR2_CC3S[1:0] (Bits1:0) */
  tmpccmrx &= (uint16_t)(~((uint16_t)0x0003));  // Bits1:0 = 00 => CC3 channel is configured as output
  /* Select the Output Compare Mode CCMR2_OC3M (Bits6:4) */
  tmpccmrx |= 0x0020; // Bits6:4 = 010 => Set channel 3 to inactive level on match (OCMode_Inactive)
  
  /* Reset the Output Polarity level CCER_CC3P (Bit9)  */
  tmpccer &= (uint16_t)(~((uint16_t)0x0200)); // Bit9 = 0 => OC3 active high 
  /* Set the Output Compare Polarity */
  tmpccer |= (uint16_t)(0x0002 << 8); // Bit9 = 0 => OC3 active low  (OCPolarity_Low)
  
  /* Set the Output State CCER_CC3E (Bit8) */
  tmpccer |= (uint16_t)(0x0001 << 8); // Bit8 = 1 => OC3 On (OutputState_Enable)
    
  /* Write to TIM2 CR2 */
  TIM2->CR2 = tmpcr2;
  
  /* Write to TIM2 CCMR2 */
  TIM2->CCMR[1] = tmpccmrx;

  /* Set the Capture Compare Register CCR3 value */
  TIM2->CCR[2] = Channel3Pulse; // VSync pulse + Back porch
  
  /* Write to TIM2 CCER */   
  TIM2->CCER = tmpccer;

//-------------------- /* Interrupt TIM2 */ -------------------------

    // Enable the Selected IRQ Channels 
    NVIC->ISER[TIM2_IRQn >> 5] = (uint32_t)(0x01 << (TIM2_IRQn & 0x1F));	 
    // Enable the channel 3 Interrupt: DIER_CC3IE (Bit3)
    TIM2->DIER |= (uint16_t)0x0008;  //  Bit3 = 1=> CC3 interrupt enabled   


//-------------------------------------------	

    // Enable the TIM2 Counter: CR1_CEN  (bit0) 
    TIM2->CR1 |= 0x0001; // bit0 = 1 => timer enable
}


//***************************************************************
void TIMS_Init(){
	/**** Enable clocks for PORTA, TIMER1 & TIMER2********/
	RCC->APB2ENR |= (1<<2) | (1<<11);     // enable PORTA and TIMER1 clocks
	RCC->APB1ENR |= (1<<0);     // enable clock for timer 2
	// Enable alternate function clock
	RCC->APB2ENR |= (1 << 0); // Bit0 = 1 ==> AFIO  enabled


	/* Configure PORTA Pin1 (TIM2 OC2) and 8  Pin8 (TIMER1 OC1)  as alternate function output Push pull 50 MHz */
    // B=1011 ==> mode=11 (Output mode, max speed 50MHz), 
    //cnf=10 (Alternate function output Push-pull)
	PORTA->CRL |= 0x000000b0; // PA1
	PORTA->CRH |= 0x0000000b; // PA8

	// configure timers
	TIM1_Config();
	TIM2_Config();

}
	