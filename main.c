/*
 * File: main.c
 * Application: Electric Water Heater (Display Flicker Fix)
 */ 

#define F_CPU 8000000UL
#include <util/delay.h>

#include "LIB/STD_TYPES.h"
#include "LIB/BIT_MATH.h"
#include "MCAL/DIO_interface.h"
#include "MCAL/ADC_interface.h"
#include "MCAL/TWI_interface.h"
#include "HAL/SSD_interface.h"
#include "HAL/EEPROM_interface.h"

/* --- Hardware Connections Definitions --- */
/* I define meaningful names for pins to make code readable and easy to change later */
#define BTN_ON_OFF_PORT  DIO_PORTB
#define BTN_ON_OFF_PIN   DIO_PIN0

#define BTN_UP_PORT      DIO_PORTB
#define BTN_UP_PIN       DIO_PIN1

#define BTN_DOWN_PORT    DIO_PORTB
#define BTN_DOWN_PIN     DIO_PIN2

#define HEATER_RELAY_PORT DIO_PORTB
#define HEATER_RELAY_PIN  DIO_PIN4

#define COOLER_PORT       DIO_PORTB
#define COOLER_PIN        DIO_PIN5

#define HEATING_LED_PORT  DIO_PORTB
#define HEATING_LED_PIN   DIO_PIN6

/* Constants for State Machine */
#define SYSTEM_OFF   0
#define SYSTEM_ON    1

/* * Function: APP_voidUpdateDisplay
 * Description: I use this helper function to handle the multiplexing timing.
 * Note: This function takes about 10ms to execute (5ms + 5ms).
 */
void APP_voidUpdateDisplay(u16 Copy_u16Number)
{
    /* 1. Turn off ghosts */
    SSD_voidDisableDigits();
    
    /* 2. Write Units Digit */
    SSD_voidWriteDigit(Copy_u16Number % 10);
    SSD_voidEnableDigit(1);
    _delay_ms(5); /* Small delay for the eye to catch it */
    
    /* 3. Turn off ghosts */
    SSD_voidDisableDigits();
    
    /* 4. Write Tens Digit */
    SSD_voidWriteDigit(Copy_u16Number / 10);
    SSD_voidEnableDigit(2);
    _delay_ms(5);
}

int main(void)
{
    /* --- System Variables --- */
    u8 Local_u8SystemState = SYSTEM_OFF;
    
    u8 Local_u8DesiredTemp = 60; /* Default set point */
    u8 Local_u8SettingMode = 0;  /* Flag: 0 = Normal, 1 = Blinking/Setting */
    u16 Local_u16TimeoutCounter = 0; /* Timer to exit setting mode automatically */
    
    /* --- Variables for Temp Averaging (Filtering) --- */
    u16 Local_u16TempReadings[10] = {0}; /* Circular buffer for last 10 readings */
    u8  Local_u8ReadIndex = 0;           /* Current position in buffer */
    u16 Local_u16TempSum = 0;
    u16 Local_u16AverageTemp = 0;        /* The final smoothed value */
    u8  Local_u8Timer100ms = 0;          /* Software prescaler for reading sensor */

    /* --- Initialization Phase --- */
    ADC_voidInit();
    SSD_voidInit();
    EEPROM_voidInit();
    
    /* Configure Input Buttons (Pull-up Resistors enabled internally if supported, or external) */
    /* Assuming internal pull-up logic or external HW pull-ups. Here initialized as Input High */
    DIO_voidSetPinDirection(BTN_ON_OFF_PORT, BTN_ON_OFF_PIN, DIO_PIN_INPUT);
    DIO_voidSetPinValue(BTN_ON_OFF_PORT, BTN_ON_OFF_PIN, DIO_PIN_HIGH); /* Activate Internal Pull-up */
    
    DIO_voidSetPinDirection(BTN_UP_PORT, BTN_UP_PIN, DIO_PIN_INPUT);
    DIO_voidSetPinValue(BTN_UP_PORT, BTN_UP_PIN, DIO_PIN_HIGH);

    DIO_voidSetPinDirection(BTN_DOWN_PORT, BTN_DOWN_PIN, DIO_PIN_INPUT);
    DIO_voidSetPinValue(BTN_DOWN_PORT, BTN_DOWN_PIN, DIO_PIN_HIGH);
    
    /* Configure Output Loads */
    DIO_voidSetPinDirection(HEATER_RELAY_PORT, HEATER_RELAY_PIN, DIO_PIN_OUTPUT);
    DIO_voidSetPinDirection(COOLER_PORT, COOLER_PIN, DIO_PIN_OUTPUT);
    DIO_voidSetPinDirection(HEATING_LED_PORT, HEATING_LED_PIN, DIO_PIN_OUTPUT);
    
    /* --- Restore Data from Memory --- */
    /* I read the last saved temperature. If it's garbage (first time run), I reset to 60 */
    Local_u8DesiredTemp = EEPROM_u8ReadByte(0x0000);
    if(Local_u8DesiredTemp > 75 || Local_u8DesiredTemp < 35) 
    {
        Local_u8DesiredTemp = 60;
        EEPROM_voidWriteByte(0x0000, 60);
    }
    
    /* --- Buffer Pre-filling --- */
    /* I fill the array with the current temp so the average doesn't start from 0 */
    u16 InitialRead = ADC_u16GetChannelReading(0);
    /* Conversion: (ADC * 5000mV) / 1024 -> /10 for LM35 (10mV/C) -> *500/1024 */
    u16 InitialTemp = (u16)(((u32)InitialRead * 500) / 1024);
    
    for(u8 i=0; i<10; i++)
    {
        Local_u16TempReadings[i] = InitialTemp;
    }
    Local_u16AverageTemp = InitialTemp;

    /* --- The Super Loop --- */
    while(1)
    {
        /* ==================== 1. ON/OFF BUTTON LOGIC ==================== */
        if(DIO_u8GetPinValue(BTN_ON_OFF_PORT, BTN_ON_OFF_PIN) == 0)
        {
            _delay_ms(50); /* Debouncing */
            if(DIO_u8GetPinValue(BTN_ON_OFF_PORT, BTN_ON_OFF_PIN) == 0)
            {
                /* Wait until release (Crucial: Keep refreshing display while waiting!) */
                while(DIO_u8GetPinValue(BTN_ON_OFF_PORT, BTN_ON_OFF_PIN) == 0)
                {
                    /* If system is ON, I must keep the SSD alive so it doesn't flicker */
                    if(Local_u8SystemState == SYSTEM_ON)
                    {
                         if(Local_u8SettingMode == 1)
                            APP_voidUpdateDisplay(Local_u8DesiredTemp);
                         else
                            APP_voidUpdateDisplay(Local_u16AverageTemp);
                    }
                }
                
                /* Toggle System State */
                if(Local_u8SystemState == SYSTEM_OFF)
                    Local_u8SystemState = SYSTEM_ON;
                else
                    Local_u8SystemState = SYSTEM_OFF;
            }
        }

        /* ==================== 2. SYSTEM ON LOGIC ==================== */
        if(Local_u8SystemState == SYSTEM_ON)
        {
            /* ---------------- A. Temperature Sensing (Every ~100ms) ---------------- */
            Local_u8Timer100ms++;
            if(Local_u8Timer100ms >= 10) /* Since loop is ~10ms, 10 counts = 100ms */
            {
                Local_u8Timer100ms = 0;
                
                /* Read ADC and Convert */
                u16 AdcVal = ADC_u16GetChannelReading(0);
                u16 CurrentTemp = (u16)(((u32)AdcVal * 500) / 1024);
                
                /* Update Moving Average Buffer */
                Local_u16TempReadings[Local_u8ReadIndex] = CurrentTemp;
                Local_u8ReadIndex++;
                if(Local_u8ReadIndex >= 10) Local_u8ReadIndex = 0;
                
                /* Calculate Average */
                Local_u16TempSum = 0;
                for(u8 i=0; i<10; i++) Local_u16TempSum += Local_u16TempReadings[i];
                Local_u16AverageTemp = Local_u16TempSum / 10;
            }

            /* ---------------- B. UP BUTTON ---------------- */
            if(DIO_u8GetPinValue(BTN_UP_PORT, BTN_UP_PIN) == 0)
            {
                _delay_ms(50);
                if(DIO_u8GetPinValue(BTN_UP_PORT, BTN_UP_PIN) == 0)
                {
                    /* First press? Enter Setting Mode. Already in? Increase Temp */
                    if(Local_u8SettingMode == 0)
                    {
                        Local_u8SettingMode = 1;
                        Local_u16TimeoutCounter = 0;
                    }
                    else
                    {
                        if(Local_u8DesiredTemp < 75)
                        {
                            Local_u8DesiredTemp += 5;
                            Local_u16TimeoutCounter = 0; /* Reset exit timer */
                        }
                    }
                    
                    /* Wait for release while refreshing display */
                    while(DIO_u8GetPinValue(BTN_UP_PORT, BTN_UP_PIN) == 0)
                    {
                        APP_voidUpdateDisplay(Local_u8DesiredTemp);
                    }
                }
            }
            
            /* ---------------- C. DOWN BUTTON ---------------- */
            else if(DIO_u8GetPinValue(BTN_DOWN_PORT, BTN_DOWN_PIN) == 0)
            {
                _delay_ms(50);
                if(DIO_u8GetPinValue(BTN_DOWN_PORT, BTN_DOWN_PIN) == 0)
                {
                    if(Local_u8SettingMode == 0)
                    {
                        Local_u8SettingMode = 1;
                        Local_u16TimeoutCounter = 0;
                    }
                    else
                    {
                        if(Local_u8DesiredTemp > 35)
                        {
                            Local_u8DesiredTemp -= 5;
                            Local_u16TimeoutCounter = 0;
                        }
                    }
                    
                    /* Wait for release while refreshing display */
                    while(DIO_u8GetPinValue(BTN_DOWN_PORT, BTN_DOWN_PIN) == 0)
                    {
                        APP_voidUpdateDisplay(Local_u8DesiredTemp);
                    }
                }
            }
            
            /* ---------------- D. Display & Control Logic ---------------- */
            if(Local_u8SettingMode == 1)
            {
                /* Safety: Turn off heater/cooler while setting temp */
                DIO_voidSetPinValue(HEATER_RELAY_PORT, HEATER_RELAY_PIN, DIO_PIN_LOW);
                DIO_voidSetPinValue(HEATING_LED_PORT, HEATING_LED_PIN, DIO_PIN_LOW);
                DIO_voidSetPinValue(COOLER_PORT, COOLER_PIN, DIO_PIN_LOW);

                /* Auto-exit setting mode if no button pressed for ~2 seconds */
                Local_u16TimeoutCounter++;
                if(Local_u16TimeoutCounter > 200) 
                {
                    /* Save new temp to EEPROM only when user finishes (saves write cycles) */
                    EEPROM_voidWriteByte(0x0000, Local_u8DesiredTemp);
                    Local_u8SettingMode = 0;
                    Local_u16TimeoutCounter = 0;
                }
                
                /* Blinking Effect: 10 cycles ON, 10 cycles OFF */
                if((Local_u16TimeoutCounter % 20) < 10)
                {
                    APP_voidUpdateDisplay(Local_u8DesiredTemp);
                }
                else
                {
                    SSD_voidDisableDigits();
                    _delay_ms(10); /* Keep timing consistent even when screen is off */
                }
            }
            else /* Normal Operation Mode */
            {
                /* Hysteresis Logic (+/- 5 degrees) to protect relay */
                if(Local_u16AverageTemp < (Local_u8DesiredTemp - 5))
                {
                    /* Too Cold -> Heater ON, LED Blink(Simulated here as ON), Cooler OFF */
                    DIO_voidSetPinValue(HEATER_RELAY_PORT, HEATER_RELAY_PIN, DIO_PIN_HIGH);
                    DIO_voidSetPinValue(HEATING_LED_PORT, HEATING_LED_PIN, DIO_PIN_HIGH);
                    DIO_voidSetPinValue(COOLER_PORT, COOLER_PIN, DIO_PIN_LOW);
                }
                else if(Local_u16AverageTemp > (Local_u8DesiredTemp + 5))
                {
                    /* Too Hot -> Heater OFF, LED OFF, Cooler ON */
                    DIO_voidSetPinValue(HEATER_RELAY_PORT, HEATER_RELAY_PIN, DIO_PIN_LOW);
                    DIO_voidSetPinValue(HEATING_LED_PORT, HEATING_LED_PIN, DIO_PIN_LOW);
                    DIO_voidSetPinValue(COOLER_PORT, COOLER_PIN, DIO_PIN_HIGH);
                }
                else
                {
                    /* In range -> All OFF (Maintain state or just drift) */
                    /* Note: A better approach is to keep previous state, but this logic is safe */
                    DIO_voidSetPinValue(HEATER_RELAY_PORT, HEATER_RELAY_PIN, DIO_PIN_LOW);
                    DIO_voidSetPinValue(HEATING_LED_PORT, HEATING_LED_PIN, DIO_PIN_LOW);
                    DIO_voidSetPinValue(COOLER_PORT, COOLER_PIN, DIO_PIN_LOW);
                }
                
                /* Show Current Temperature */
                APP_voidUpdateDisplay(Local_u16AverageTemp);
            }
        }
        else /* SYSTEM_OFF State */
        {
            /* Turn off everything */
            SSD_voidDisableDigits();
            DIO_voidSetPinValue(HEATER_RELAY_PORT, HEATER_RELAY_PIN, DIO_PIN_LOW);
            DIO_voidSetPinValue(COOLER_PORT, COOLER_PIN, DIO_PIN_LOW);
            DIO_voidSetPinValue(HEATING_LED_PORT, HEATING_LED_PIN, DIO_PIN_LOW);
        }
    }
}