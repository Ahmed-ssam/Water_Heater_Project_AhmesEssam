/* File: HAL/EEPROM_program.c */

/* Define CPU frequency for timing calculations */
#define F_CPU 8000000UL

/* Include standard types and hardware interfaces */
#include "../LIB/STD_TYPES.h"
#include "../MCAL/TWI_interface.h"
#include "EEPROM_interface.h"
#include <util/delay.h>

/* Define the fixed address for 24C08 EEPROM */
#define EEPROM_FIXED_ADDRESS  0x50

/* Function to initialize the EEPROM driver */
void EEPROM_voidInit(void)
{
	/* Initialize TWI (I2C) in Master mode */
	TWI_voidInitMaster(0);
}

/* Function to write a byte to a specific address */
void EEPROM_voidWriteByte(u16 Copy_u16Address, u8 Copy_u8Data)
{
	/* Calculate address packet (Fixed ID + Page bits) */
	u8 Local_u8AddressPacket = EEPROM_FIXED_ADDRESS | (Copy_u16Address >> 8);

	/* Send Start Condition to begin communication */
	TWI_SendStartCondition();
	
	/* Send Slave Address with Write request */
	TWI_SendSlaveAddressWithWrite(Local_u8AddressPacket);
	
	/* Send the memory location address (Word Address) */
	TWI_MasterWriteDataByte((u8)Copy_u16Address);
	
	/* Send the actual data to be stored */
	TWI_MasterWriteDataByte(Copy_u8Data);
	
	/* Send Stop Condition to end communication */
	TWI_SendStopCondition();
	
	/* Wait 10ms for the internal write cycle to finish */
	_delay_ms(10);
}

/* Function to read a byte from a specific address */
u8 EEPROM_u8ReadByte(u16 Copy_u16Address)
{
	/* Variable to hold the received data */
	u8 Local_u8Data = 0;
	
	/* Calculate address packet (Fixed ID + Page bits) */
	u8 Local_u8AddressPacket = EEPROM_FIXED_ADDRESS | (Copy_u16Address >> 8);

	/* Send Start Condition */
	TWI_SendStartCondition();
	
	/* Send Slave Address with Write (Dummy Write to set pointer) */
	TWI_SendSlaveAddressWithWrite(Local_u8AddressPacket);
	
	/* Send the memory location address */
	TWI_MasterWriteDataByte((u8)Copy_u16Address);
	
	/* Send Repeated Start to switch to Read mode */
	TWI_SendRepeatedStart();
	
	/* Send Slave Address with Read request */
	TWI_SendSlaveAddressWithRead(Local_u8AddressPacket);
	
	/* Read the data byte from the slave */
	TWI_MasterReadDataByte(&Local_u8Data);
	
	/* Send Stop Condition */
	TWI_SendStopCondition();
	
	/* Return the read data */
	return Local_u8Data;
}