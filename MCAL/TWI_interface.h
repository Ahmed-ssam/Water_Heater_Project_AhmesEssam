/* File: MCAL/TWI_interface.h */

#ifndef TWI_INTERFACE_H_
#define TWI_INTERFACE_H_

/* * My Error List.
 * Since I2C communication can fail at any step (No ACK, Bus busy),
 * I define these codes to debug exactly where the problem happened.
 */
typedef enum
{
    NoError,                     /* Everything went fine */
    StartError,                  /* Failed to send Start Condition */
    RepeatedStartError,          /* Failed to send Repeated Start */
    SlaveAddressWithWriteError,  /* I called the slave to WRITE, but it didn't answer (No ACK) */
    SlaveAddressWithReadError,   /* I called the slave to READ, but it didn't answer (No ACK) */
    MasterWriteByteError,        /* I sent data, but the slave didn't acknowledge */
    MasterReadByteError,         /* I failed to receive data properly */
} TWI_ErrStatus;

/* * Function: TWI_voidInitMaster
 * I configure the SCL speed (usually 400kHz) and my own address (if needed).
 */
void TWI_voidInitMaster(u8 Copy_u8Address);

/* * Function: TWI_SendStartCondition
 * I scream "ATTENTION!" on the bus to start the frame.
 */
TWI_ErrStatus TWI_SendStartCondition(void);

/* * Function: TWI_SendRepeatedStart
 * I use this to change direction (Write -> Read) without losing the bus control.
 */
TWI_ErrStatus TWI_SendRepeatedStart(void);

/* * Function: TWI_SendSlaveAddressWithWrite
 * I send the slave ID + Write bit (0). I expect an ACK response.
 */
TWI_ErrStatus TWI_SendSlaveAddressWithWrite(u8 Copy_u8SlaveAddress);

/* * Function: TWI_SendSlaveAddressWithRead
 * I send the slave ID + Read bit (1). I expect an ACK response.
 */
TWI_ErrStatus TWI_SendSlaveAddressWithRead(u8 Copy_u8SlaveAddress);

/* * Function: TWI_MasterWriteDataByte
 * I put the actual data byte on the bus and wait for the slave to accept it.
 */
TWI_ErrStatus TWI_MasterWriteDataByte(u8 Copy_u8DataByte);

/* * Function: TWI_MasterReadDataByte
 * Note: I use a pointer (u8*) for data because the function "return" is used for Error Status.
 * So, I save the received data inside this pointer variable.
 */
TWI_ErrStatus TWI_MasterReadDataByte(u8* Copy_pu8DataByte);

/* * Function: TWI_SendStopCondition
 * I release the bus so others can use it. No error check needed here.
 */
void TWI_SendStopCondition(void);

#endif