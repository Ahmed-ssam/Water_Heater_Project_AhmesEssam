/*
 * File: ADC_private.h
 * Layer: MCAL
 */ 
#ifndef ADC_PRIVATE_H_
#define ADC_PRIVATE_H_




/* * ADMUX (ADC Multiplexer Selection Register) - Address 0x27
 * I use this register to select which Pin (Channel) to read from (ADC0-ADC7),
 * and to select the Voltage Reference (AVCC, Internal, etc).
 * 'volatile' tells the compiler: "Don't optimize this, hardware changes it".
 */
#define ADMUX_REG   *((volatile u8*)0x27)

/* * ADCSRA (ADC Control and Status Register A) - Address 0x26
 * I use this register to Enable the ADC, Start Conversion,
 * Check the Flag (finished?), and set the Prescaler (Speed).
 */
#define ADCSRA_REG  *((volatile u8*)0x26)

/* * Data Registers (High and Low Bytes)
 * The ADC result is 10-bit, so it's split into two 8-bit registers.
 */
#define ADCH_REG    *((volatile u8*)0x25) /* High Byte */
#define ADCL_REG    *((volatile u8*)0x24) /* Low Byte */

/* * ADC Data Register (16-bit access) - Address 0x24
 * Clever Trick: Since ADCL (0x24) and ADCH (0x25) are next to each other in memory,
 * I cast the address of ADCL to a (u16*) pointer.
 * This allows me to read the full 10-bit result in one line of code!
 */
#define ADC_DATA_REG *((volatile u16*)0x24)

#endif /* ADC_PRIVATE_H_ */