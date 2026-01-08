/* File: MCAL/TWI_private.h */
#ifndef TWI_PRIVATE_H_
#define TWI_PRIVATE_H_

#define TWBR_REG    *((volatile u8*)0x20)
#define TWSR_REG    *((volatile u8*)0x21)
#define TWAR_REG    *((volatile u8*)0x22)
#define TWDR_REG    *((volatile u8*)0x23)
#define TWCR_REG    *((volatile u8*)0x56)

/* Status Codes definition */
#define TWI_START_ACK           0x08 /* Start condition transmitted */
#define TWI_REP_START_ACK       0x10 /* Repeated start transmitted */
#define TWI_SLAVE_ADD_WR_ACK    0x18 /* SLA+W transmitted, ACK received */
#define TWI_SLAVE_ADD_RD_ACK    0x40 /* SLA+R transmitted, ACK received */
#define TWI_MSTR_WR_BYTE_ACK    0x28 /* Data byte transmitted, ACK received */
#define TWI_MSTR_RD_BYTE_ACK    0x50 /* Data byte received, ACK returned */
#define TWI_MSTR_RD_BYTE_NACK   0x58 /* Data byte received, NACK returned */

#endif