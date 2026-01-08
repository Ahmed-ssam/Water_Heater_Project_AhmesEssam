/*
 * File: MCAL/ADC_interface.h
 * Layer: MCAL
 */ 

#ifndef ADC_INTERFACE_H_
#define ADC_INTERFACE_H_

/* * Function: ADC_voidInit
 * I setup the ADC hardware settings (like Voltage Reference and Speed/Prescaler).
 * I must call this once before using the ADC.
 */
void ADC_voidInit(void);

/* * Function: ADC_u16GetChannelReading
 * I take the Channel Number (which Pin I want to read, e.g., PA0).
 * I return u16 because the ADC resolution is 10-bit (0 to 1023), so a u8 is not enough.
 */
u16  ADC_u16GetChannelReading(u8 Copy_u8Channel);

#endif /* ADC_INTERFACE_H_ */