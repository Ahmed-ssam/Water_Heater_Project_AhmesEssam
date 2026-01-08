/* File: HAL/EEPROM_interface.h */
#ifndef EEPROM_INTERFACE_H_
#define EEPROM_INTERFACE_H_

/* I initialize the I2C protocol here */
void EEPROM_voidInit(void);

/* I write a generic byte to any specific address (Used for Temp, Flags, etc.) */
void EEPROM_voidWriteByte(u16 Copy_u16Address, u8 Copy_u8Data);

/* I read a byte from a specific address */
u8   EEPROM_u8ReadByte(u16 Copy_u16Address);

#endif