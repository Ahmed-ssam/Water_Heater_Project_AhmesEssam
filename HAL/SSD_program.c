/*
 * File: SSD_program.c
 * Layer: HAL
 */ 

/* I include standard types to use u8, u16 */
#include "../LIB/STD_TYPES.h"
/* I include bit math for bitwise operations */
#include "../LIB/BIT_MATH.h"
/* I include DIO to control the hardware pins */
#include "../MCAL/DIO_interface.h"
/* I include my own header to implement the functions */
#include "SSD_interface.h"





/* * This is my "Lookup Table". 
 * It maps the decimal number (Index) to the 7-Segment pattern (Value).
 * I use Common Cathode logic (1 = LED ON).
 * Connection: Port D pins 0-6 connected to segments a-g.
 */
static u8 SSD_u8Numbers[10] = {
    0x3F, /* Index 0: I turn on bits 0-5 to draw '0' */
    0x06, /* Index 1: I turn on bits 1,2 (b,c) to draw '1' */
    0x5B, /* Index 2: Pattern for '2' */
    0x4F, /* Index 3: Pattern for '3' */
    0x66, /* Index 4: Pattern for '4' */
    0x6D, /* Index 5: Pattern for '5' */
    0x7D, /* Index 6: Pattern for '6' */
    0x07, /* Index 7: Pattern for '7' */
    0x7F, /* Index 8: Pattern for '8' (All segments on) */
    0x6F  /* Index 9: Pattern for '9' */
};

/* Function to setup the hardware pins */
void SSD_voidInit(void)
{
    /* I set the whole Port D as OUTPUT because it sends the data pattern */
    DIO_voidSetPortDirection(DIO_PORTD, DIO_PORT_OUTPUT);
    
    /* I set PC6 as OUTPUT (This controls the Right Screen / Units) */
    DIO_voidSetPinDirection(DIO_PORTC, DIO_PIN6, DIO_PIN_OUTPUT);

    /* I set PC7 as OUTPUT (This controls the Left Screen / Tens) */
    DIO_voidSetPinDirection(DIO_PORTC, DIO_PIN7, DIO_PIN_OUTPUT);
}

/* Function to display a specific number pattern */
void SSD_voidWriteDigit(u8 Copy_u8Digit)
{
    /* Safety check: I only have patterns for 0 to 9 */
    if (Copy_u8Digit < 10)
    {
        /* I fetch the pattern from my table and push it to Port D */
        DIO_voidSetPortValue(DIO_PORTD, SSD_u8Numbers[Copy_u8Digit]);
    }
}

/* Function to choose which screen is ON (Multiplexing) */
void SSD_voidEnableDigit(u8 Copy_u8DigitIndex)
{
    /* * Logic: Common Cathode means the common pin needs Ground (LOW) to work.
     * So: LOW = Enable (ON), HIGH = Disable (OFF).
     */
    
    /* If I want to enable the Right Screen (Units) */
    if (Copy_u8DigitIndex == 1)
    {
        /* I pull PC6 LOW to complete the circuit (Turn ON Right) */
        DIO_voidSetPinValue(DIO_PORTC, DIO_PIN6, DIO_PIN_LOW);
        
        /* I push PC7 HIGH to break the circuit (Turn OFF Left) */
        DIO_voidSetPinValue(DIO_PORTC, DIO_PIN7, DIO_PIN_HIGH);
    }
    /* If I want to enable the Left Screen (Tens) */
    else if (Copy_u8DigitIndex == 2)
    {
        /* I push PC6 HIGH to turn OFF Right */
        DIO_voidSetPinValue(DIO_PORTC, DIO_PIN6, DIO_PIN_HIGH); 
        
        /* I pull PC7 LOW to turn ON Left */
        DIO_voidSetPinValue(DIO_PORTC, DIO_PIN7, DIO_PIN_LOW);  
    }
}

/* Function to turn OFF everything */
void SSD_voidDisableDigits(void)
{
    /* I set both control pins HIGH to cut the power */
    /* This is used to clear the view and prevent "Ghosting" */
    
    DIO_voidSetPinValue(DIO_PORTC, DIO_PIN6, DIO_PIN_HIGH);
    DIO_voidSetPinValue(DIO_PORTC, DIO_PIN7, DIO_PIN_HIGH);
}