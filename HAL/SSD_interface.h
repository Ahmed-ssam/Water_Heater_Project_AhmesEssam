/*
 * File: SSD_interface.h
 * Layer: HAL
 */ 

#ifndef SSD_INTERFACE_H_
#define SSD_INTERFACE_H_

/* * Function: SSD_voidInit
 * I set the direction of pins connected to the display (Data & Control) as Outputs.
 */
void SSD_voidInit(void);

/* * Function: SSD_voidWriteDigit
 * I take a number (0-9) and put its pattern on the data port.
 */
void SSD_voidWriteDigit(u8 Copy_u8Digit);

/* * Function: SSD_voidEnableDigit
 * I choose which screen to turn ON (Multiplexing logic).
 * 1 = Right Screen (Units), 2 = Left Screen (Tens).
 */
void SSD_voidEnableDigit(u8 Copy_u8DigitIndex);

/* * Function: SSD_voidDisableDigits
 * I turn OFF both screens to prevent "Ghosting" during switching.
 */
void SSD_voidDisableDigits(void);

#endif 