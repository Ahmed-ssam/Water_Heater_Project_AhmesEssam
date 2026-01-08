/*
 * File: TWI_program.c
 * Layer: MCAL
 */

#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"
#include "TWI_private.h"
#include "TWI_interface.h"

/* Function: Initialize I2C Master */
void TWI_voidInitMaster(u8 Copy_u8Address)
{
    /* 1. Set Bit Rate (Speed) */
    /* I want 100kbps (Standard Speed). My CPU is 8MHz.
     * Equation: SCL_Freq = CPU_Freq / (16 + 2 * TWBR * 4^Prescaler)
     * Calculation: 100000 = 8000000 / (16 + 2 * 32 * 1) -> So TWBR must be 32.
     */
    TWBR_REG = 32;

    /* 2. Set Prescaler */
    /* I choose Prescaler = 1 (No division) to keep calculations simple. */
    CLR_BIT(TWSR_REG, 0);
    CLR_BIT(TWSR_REG, 1);

    /* 3. Set My Address (Optional) */
    /* If another Master wants to talk to me, I need an address.
     * I shift it left because the LSB is reserved for the General Call bit.
     */
    if(Copy_u8Address != 0)
    {
        TWAR_REG = (Copy_u8Address << 1);
    }
    
    /* 4. Enable TWI Module */
    /* Turn on the engine! */
    SET_BIT(TWCR_REG, 2);
}

/* Function: Send Start Condition */
TWI_ErrStatus TWI_SendStartCondition(void)
{
    TWI_ErrStatus Local_Error = NoError;
    
    /* I ask hardware to Send START (TWSTA), Clear Flag (TWINT), and Enable TWI (TWEN) */
    TWCR_REG = (1<<7) | (1<<5) | (1<<2);
    
    /* I wait here until the hardware finishes the job (Flag becomes 1) */
    while(GET_BIT(TWCR_REG, 7) == 0);
    
    /* I check the Status Register: Did the hardware actually send START? */
    if((TWSR_REG & 0xF8) != TWI_START_ACK)
    {
        Local_Error = StartError;
    }
    return Local_Error;
}

/* Function: Send Repeated Start */
TWI_ErrStatus TWI_SendRepeatedStart(void)
{
    TWI_ErrStatus Local_Error = NoError;
    
    /* Same as Start, but used when I want to switch from Write to Read without losing the bus */
    TWCR_REG = (1<<7) | (1<<5) | (1<<2);
    
    /* Wait for job completion */
    while(GET_BIT(TWCR_REG, 7) == 0);
    
    /* Check Status: Did it send REPEATED START? */
    if((TWSR_REG & 0xF8) != TWI_REP_START_ACK)
    {
        Local_Error = RepeatedStartError;
    }
    return Local_Error;
}

/* Function: Call Slave to WRITE */
TWI_ErrStatus TWI_SendSlaveAddressWithWrite(u8 Copy_u8SlaveAddress)
{
    TWI_ErrStatus Local_Error = NoError;
    
    /* I prepare the packet: Address in top 7 bits + '0' at bit 0 (Write Mode) */
    TWDR_REG = (Copy_u8SlaveAddress << 1);
    CLR_BIT(TWDR_REG, 0); 
    
    /* Clear Flag to start transmitting this packet */
    TWCR_REG = (1<<7) | (1<<2); 
    
    /* Wait for transmission to end */
    while(GET_BIT(TWCR_REG, 7) == 0);
    
    /* Check Status: Did the Slave reply with ACK? */
    if((TWSR_REG & 0xF8) != TWI_SLAVE_ADD_WR_ACK)
    {
        Local_Error = SlaveAddressWithWriteError;
    }
    return Local_Error;
}

/* Function: Call Slave to READ */
TWI_ErrStatus TWI_SendSlaveAddressWithRead(u8 Copy_u8SlaveAddress)
{
    TWI_ErrStatus Local_Error = NoError;
    
    /* I prepare the packet: Address in top 7 bits + '1' at bit 0 (Read Mode) */
    TWDR_REG = (Copy_u8SlaveAddress << 1);
    SET_BIT(TWDR_REG, 0); 
    
    /* Clear Flag to start transmitting */
    TWCR_REG = (1<<7) | (1<<2);
    
    /* Wait for transmission */
    while(GET_BIT(TWCR_REG, 7) == 0);
    
    /* Check Status: Did the Slave reply with ACK? */
    if((TWSR_REG & 0xF8) != TWI_SLAVE_ADD_RD_ACK)
    {
        Local_Error = SlaveAddressWithReadError;
    }
    return Local_Error;
}

/* Function: Write Data Byte */
TWI_ErrStatus TWI_MasterWriteDataByte(u8 Copy_u8DataByte)
{
    TWI_ErrStatus Local_Error = NoError;
    
    /* Put the actual data in the Data Register */
    TWDR_REG = Copy_u8DataByte;
    
    /* Clear Flag to send it */
    TWCR_REG = (1<<7) | (1<<2);
    
    /* Wait for transmission */
    while(GET_BIT(TWCR_REG, 7) == 0);
    
    /* Check Status: Did the Slave receive it and send ACK? */
    if((TWSR_REG & 0xF8) != TWI_MSTR_WR_BYTE_ACK)
    {
        Local_Error = MasterWriteByteError;
    }
    return Local_Error;
}

/* Function: Read Data Byte */
TWI_ErrStatus TWI_MasterReadDataByte(u8* Copy_pu8DataByte)
{
    TWI_ErrStatus Local_Error = NoError;
    
    /* Important: I clear the flag WITHOUT setting the ACK bit (TWEA).
     * This means I will send NACK after receiving this byte.
     * Why? Because I usually read one byte then Stop. NACK tells Slave "I am done".
     */
    TWCR_REG = (1<<7) | (1<<2); 
    
    /* Wait until data is received into TWDR */
    while(GET_BIT(TWCR_REG, 7) == 0);
    
    /* Read the data from the register into my variable */
    *Copy_pu8DataByte = TWDR_REG;
    
    /* Note: I didn't check for errors here because I expect NACK anyway */
    return Local_Error;
}

/* Function: Send Stop Condition */
void TWI_SendStopCondition(void)
{
    /* I generate STOP condition (TWSTO) to release the bus */
    TWCR_REG = (1<<7) | (1<<4) | (1<<2);
    
    /* Note: No need to wait for flag here. The hardware releases the bus automatically. */
}