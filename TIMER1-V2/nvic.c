#include "periph.h"

// =======================================
/* This function is used to select the Priority Grouping. 
The parameter "PriorityGroup" is the Priority Grouping Number as given in the Table:
 
Priority Group Number 	Split Point 	Group Prio Bits (x) 	Sub Prio Bits (y) 	Total Group Prio 	Total SubGroup Prio
0,1,2,3 	              [xxxx.0000] 	      [7-4] 	               N/A 	               16 	            1
4 	                    [xxx.y0000] 	      [7-5] 	               [4] 	                8 	            2
5 	                    [xx.yy0000] 	      [7-6] 	              [5-4] 	              4 	            4
6 	                    [x.yyy0000] 	       [7] 	                [6-4] 	              2 	            8
7 	                    [.yyyy0000] 	       N/A 	                [7-4] 	             N/A 	            16

This function changes the PRIGROUP bits[10:8] of SCB->AIRCR (Application Interrupt & Reset Control Register) which defines how the Priority Fields in NVIC->IPRs are split. 

it Sets the priority grouping field using the required unlock sequence.
 The parameter PriorityGroup is assigned to the field SCB->AIRCR [10:8] PRIGROUP field.
 Only values from 0..7 are used.
 In case of a conflict between priority grouping and available priority bits (__NVIC_PRIO_BITS = 4), the smallest possible priority group is set.
 */
 
void setPriGroup(uint32_t PriorityGroup){
  uint32_t reg_value;
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07);  /* only values 0..7 are used */

  reg_value  =  SCB->AIRCR;     /* read old register configuration */
  reg_value &= ~((uint32_t)((0xFFFF << 16) | (7<< 8))); /* clear bits to change    */
  reg_value  =  (reg_value |((uint32_t)0x5FA << 16) |(PriorityGroupTmp << 8) );  /* Insert write key and priorty group */
  SCB->AIRCR =  reg_value;
}

//======================================================
/*  
Encodes the priority for an interrupt with the given priority group, preemptive priority value, and subpriority value.
It automatically takes care of the un-implemented bits. 
   In case of a conflict between priority grouping and available priority bits (__NVIC_PRIO_BITS = 4), the smallest possible priority group is set.
  input params:     
        PriorityGroup  Used priority group.
        PreemptPriority  Preemptive priority value (starting from 0).
        SubPriority  Subpriority value (starting from 0).
  output:  Encoded priority. Value used in the function setPriority().
 */
uint32_t encodePriority (uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority){
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07);   /* only values 0..7 are used   */
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7 - PriorityGroupTmp) > (uint32_t)(4)) ? (uint32_t)(4) : (uint32_t)(7 - PriorityGroupTmp);
  SubPriorityBits     = ((PriorityGroupTmp + (uint32_t)(4)) < (uint32_t)7) ? (uint32_t)0 : (uint32_t)((PriorityGroupTmp - 7) + (uint32_t)(4));

  return (((PreemptPriority & (uint32_t)((1 << (PreemptPriorityBits)) - 1)) << SubPriorityBits) |
           ((SubPriority & (uint32_t)((1 << (SubPriorityBits)) - 1)))
         );
}
//=====================================================
/* Sets the priority of an interrupt.
  input params:      
      - IRQn  Interrupt number.
      - priority  Priority to set.
 the parameter priority must be in accordance with Priority Grouping selected, hence encodePriority() must be used to encode priority before calling this function.
   note :   The priority cannot be set for every core interrupt.
 */
 void setPriority(uint32_t IRQn, uint32_t priority){
    NVIC->IP[((uint32_t)(int32_t)IRQn)]  = (uint8_t)((priority << 4) & (uint32_t)0xFF);
}

//========================================
/* Enable given interrupt:
   Each NVIC->ISER {0-7} holds 32 interrupts. 
   Thus take mod32 of the given interrupt to choose the ISER number (ISER[0] for IRQn 0-31, and ISER[1] for IRQn 32-63 ..)
     Then, enable the given bit on that register based on the remainder of the mod.
 */
void enableIrq(int IRQn){

	//if ( irq >= 44 )   return;
	NVIC->ISER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}
 
