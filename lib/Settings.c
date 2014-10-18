//
//  Settings.c
//  Speakers
//
//  Created by Francisco de Villalobos on 10/6/13.
//  Copyright (c) 2013 Francisco de Villalobos. All rights reserved.
//

#include <stdio.h>
#include "m_general.h"

void init(void){
    m_clockdivide(0);

/******************************************************************************************/
    // Define Timer 1 Settings
    // We are going to use Timer 1 as the PWM modulated signal to generate
    // the velocity control of the back motors (To enable Channel).
    
    // Define Prescaler as 8.
    clear(TCCR1B,CS12);
    set(TCCR1B,CS11);
    clear(TCCR1B,CS10);
    
    // Set Mode 15: UP to OCR1A, PWM mode
    set(TCCR1B,WGM13);
    set(TCCR1B,WGM12);
    set(TCCR1A,WGM11);
    set(TCCR1A,WGM10);
    
    // Toggle Mode on OC1C, which is multiplexed to B7
    set(DDRB,7);
    set(TCCR1A,COM1C1);
    set(TCCR1A,COM1C0);
    
    // Set the OCR1A and OCR1B. (Only Initial values. Will Change dynamically)
    OCR1A = 0x00FF;  //255
    OCR1C = 150;
    
    // Set Interrupt in Overflow
    // set(TIMSK1,TOIE1);
    
    
/******************************************************************************************/
    // Define Timer 3 Settings
    // Same as timer 1. Same frecuency, but we will change the Duty Cycle.
    // Front Motor Control. Velocity has to be 2.8333 times less than back motors.
    
    // Define Prescaler as 8.
    clear(TCCR3B,CS32);
    set(TCCR3B,CS31);
    clear(TCCR3B,CS30);
    
    // Set Mode 05: UP to 0x00FF.
    clear(TCCR3B,WGM33);
    set(TCCR3B,WGM32);
    clear(TCCR3A,WGM31);
    set(TCCR3A,WGM30);
    
    // Set at OCR3A, clear in rollover.
    set(DDRC,6);
    set(TCCR3A,COM3A1);
    set(TCCR3A,COM3A0);
    
    // Set the OCR3A.
    OCR3A = 100;//OCR1C * 2.83333;
    
    //Set Interrupt Enable in Overflow
    //set(TIMSK3,TOIE3);
    
 
/******************************************************************************************/
    // Timer 0 for sound duration
    
    // 1024 Prescaler
    set(TCCR0B, CS02);
    clear(TCCR0B, CS01);
    set(TCCR0B, CS00);
    
    // UP to OCR0A
    clear(TCCR0B, WGM02);
    set(TCCR0A, WGM01);
    clear(TCCR0A, WGM00);
    OCR0A = 0x004D;         //16 is 200Hz. 9B is 50 Hz. 4D is 100 Hz which is 1 centi second
    
    // No Change Pin B7
    clear(TCCR0A, COM0A1);
    clear(TCCR0A, COM0A0);

    
/******************************************************************************************/
    //ADC Initialization
    
    //Vcc Voltage Reference
    clear(ADMUX,REFS1);
    set(ADMUX,REFS0);
    
    //ADC Prescaler (Keep between 50 and 200kHz)
    // = /128
    set(ADCSRA,ADPS2);
    set(ADCSRA,ADPS1);
    set(ADCSRA,ADPS0);
    
    //Disable digital inputs for ADC use.
    set(DIDR2,ADC8D);
    set(DIDR2,ADC9D);
    set(DIDR2,ADC11D);
    set(DIDR2, ADC13D);

    //Enable Auto-Conversion
    set(ADCSRA,ADATE);
    
    //Disable ADC Subsystem.
    clear(ADCSRA,ADEN);
    //Connect B4 to the ADC Unit.
    set(ADCSRB,MUX5);
    clear(ADMUX,MUX2);
    set(ADMUX,MUX1);
    set(ADMUX,MUX0);
    
    //Enable ADC Subsystem
    set(ADCSRA,ADEN);
    
    //Start Conversion (Only 1 time needed to start because of Auto-Conversion)
    set(ADCSRA,ADSC);
    
    // ACD Variable Returns Conversion from 0 to 1023.
}