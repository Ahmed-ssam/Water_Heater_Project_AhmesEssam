/*
 * File: ADC_program.c
 * Layer: MCAL
 */ 

#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"
#include "ADC_private.h"
#include "ADC_interface.h"





/* Function: Initialize the ADC */
void ADC_voidInit(void)
{
    /* 1. Reference Voltage Selection: AVCC */
    /* I choose AVCC (5V) because my sensor (LM35) outputs voltage relative to 5V.
     * REFS1=0, REFS0=1 means AVCC mode.
     */
    SET_BIT(ADMUX_REG, 6); // Set REFS0
    CLR_BIT(ADMUX_REG, 7); // Clear REFS1
    
    /* 2. Data Adjustment: Right Adjust */
    /* I chose Right Adjust (ADLAR=0) because I want to read the full 10-bit value (0-1023) 
     * in a normal integer variable. Left adjust is only for 8-bit precision.
     */
    CLR_BIT(ADMUX_REG, 5); // ADLAR = 0
    
    /* 3. Clock Prescaler Selection: Division Factor 128 */
    /* My CPU is fast (8MHz), but the ADC is slow (needs 50kHz-200kHz).
     * Calculation: 8000000 / 128 = 62.5kHz. This is perfect (safe range).
     */
    SET_BIT(ADCSRA_REG, 0); // ADPS0 = 1
    SET_BIT(ADCSRA_REG, 1); // ADPS1 = 1
    SET_BIT(ADCSRA_REG, 2); // ADPS2 = 1
    
    /* 4. Enable ADC Peripheral */
    /* I turn on the main power switch for the ADC module. */
    SET_BIT(ADCSRA_REG, 7); // ADEN = 1
}

/* Function: Read Analog Value from a specific pin */
u16 ADC_u16GetChannelReading(u8 Copy_u8Channel)
{
    /* 1. Select Channel */
    /* Safety Step: I must clear the old channel bits (first 5 bits) before writing new ones.
     * 0xE0 is (1110 0000), so ANDing with it clears the bottom 5 bits.
     */
    ADMUX_REG &= 0xE0;
    
    /* Now I write the new channel number (e.g., 0 for ADC0, 1 for ADC1...) */
    ADMUX_REG |= Copy_u8Channel; 
    
    /* 2. Start Conversion */
    /* I give the order to start measuring NOW. */
    SET_BIT(ADCSRA_REG, 6); // Set ADSC (Start Conversion)
    
    /* 3. Wait for completion (Polling) */
    /* I stay in this loop doing nothing until the hardware raises the Flag (ADIF).
     * When Flag becomes 1, it means the conversion is finished.
     */
    while(GET_BIT(ADCSRA_REG, 4) == 0);
    
    /* 4. Clear Flag */
    /* Rule: To clear this interrupt flag, I must write logical '1' to it. 
     * I clear it manually to be ready for the next conversion.
     */
    SET_BIT(ADCSRA_REG, 4);
    
    /* 5. Return Result */
    /* I read the result from the 16-bit register (ADCL + ADCH combined) and return it. */
    return ADC_DATA_REG;
}