/* Name: main.c
 * Author: Francisco de Villalobos
 * Copyright: Francisco de Villalobos
 * License: Francisco de Villalobos
 */


#include "saast.h"
#include <avr/io.h>
#include "Functions.h"
#include "m_usb.h"
#include "m_bus.h"
#include "m_rf.h"
#include "Settings.h"
#include <avr/interrupt.h>


#define dir_1        7
#define dir_2        1
#define desvio       10
#define AVfilter     5
#define AVfilter_2   5
#define focus_turns  5
#define MINcm_Value  900
#define MAXcm_Value  100

int in_key;
int perc_1 = 10;
int perc_2 = 30;
int Vcap;
int Cap_Charge;
int Cap_Set = 20000;
int distance;
char n;
char dir;
char filter;
char filter_2;
char turns = 0;
char flag_found = 0;
char cap_ready;
unsigned int conta;
unsigned int LED_1;
unsigned int LED_2;
unsigned int LED_3;
unsigned int LED_1_fin;
unsigned int LED_2_fin;
unsigned int LED_3_fin;
unsigned int LED_3_old;
unsigned int LED_3_MAX[3];



int main(void)
{
/******************************************************************************************/
/*****************                    Initialization                      *****************/
/******************************************************************************************/
    
    init();                             // Run Initialization Code
    m_bus_init();
	m_usb_init();                       // Initialize the USB connection.
    sei();                              // Enable Interrupts
    
    set(DDRD,dir_1);        // Motor 1 Direction PIN
    set(DDRF,dir_2);        // Motor 2 Direction PIN
    set(DDRC,7);            // Charge Capacitor
    set(DDRE,6);            // Discharge Capacitor
    clear(DDRB,0);          // Encoder 1, Quadrature 1
    clear(DDRB,1);          // Encoder 1, Quadrature 2
    clear(DDRB,2);          // Encoder 2, Quadrature 1
    clear(DDRB,3);          // Encoder 2, Quadrature 2
    
	//Encoder Definition
	set(PCICR,PCIE0);		// Enable PIN-Change Interrupts
	set(PCMSK0,PCINT0);		// Demask B0
	set(PCMSK0,PCINT1);		// Demask B1
	set(PCMSK0,PCINT2);		// Demask B2
	set(PCMSK0,PCINT3);		// Demask B3
    mx_encoder_zero(1);     // Reset Encoder 1
    mx_encoder_zero(2);     // Reset Encoder 2
    LED_3_MAX[0] = 1023;

    while(1){

/******************************************************************************************/
/*****************                     ADC Conversion                     *****************/
/******************************************************************************************/
// Checks the value of the conversion and stores it in the correct variable
        
        if (check(ADCSRA,ADIF))
        {
            if (n==0) {
                filter += 1;
                filter_2 +=1;
            }
            switch(n){
                case 0:
                    LED_1 += ADC;
                    n += 1;
                    break;
                case 1:
                    LED_2 += ADC;
                    n += 1;
                    break;
                case 2:
                    LED_3 += ADC;
                    n += 1;
                    break;
                case 3:
                    Vcap = ADC;
                    Cap_Charge = Vcap * 54;
                    n = 0;
                    break;
            }
            
// Averages the readings in an amount of times to smooth readings.
            if (filter == AVfilter) {
                LED_1_fin = LED_1 / filter;
                LED_1 = 0;
                LED_2_fin = LED_2 / filter;
                LED_2 = 0;
                filter = 0;
            }
            if (filter_2 == AVfilter_2) {
                LED_3_old = LED_3_fin;
                LED_3_fin = LED_3 / filter_2;
                if ((LED_3_fin<LED_3_MAX[0]) && (turns>=2)) {
                    LED_3_MAX[0] = LED_3_fin;
                    LED_3_MAX[1] = mx_encoder(1);
                    LED_3_MAX[2] = mx_encoder(2);
                }
                LED_3 = 0;
                filter_2 = 0;
            }
            clear(ADCSRA,ADEN);     // Disable Conversion
            set(ADCSRA,ADIF);       //Set the flag so that it starts converting again
        }
 
        
// Multiplexes the ADC conversion between 3 different channels.
        if (check(ADCSRA, ADEN)==0) {
            switch(n){
                case 0:                 //Port B4 as ADC
                    set(ADCSRB,MUX5);
                    clear(ADMUX,MUX2);
                    set(ADMUX,MUX1);
                    set(ADMUX,MUX0);
                    break;
                case 1:                 //Port D4 as ADC
                    set(ADCSRB,MUX5);
                    clear(ADMUX,MUX2);
                    clear(ADMUX,MUX1);
                    clear(ADMUX,MUX0);
                    break;
                case 2:                 //Port D6 as ADC
                    set(ADCSRB,MUX5);
                    clear(ADMUX,MUX2);
                    clear(ADMUX,MUX1);
                    set(ADMUX,MUX0);
                    break;
                case 3:                 //Port B6 as ADC
                    set(ADCSRB,MUX5);
                    set(ADMUX,MUX2);
                    clear(ADMUX,MUX1);
                    set(ADMUX,MUX0);
                    break;
            }
            //Enable ADC Subsystem
            set(ADCSRA,ADEN);
            //Start Conversion (Only 1 time needed to start because of Auto-Conversion)
            set(ADCSRA,ADSC);
        }
        
/******************************************************************************************/
/*****************                   Rotation Direction                   *****************/
/******************************************************************************************/
// Changes Direction According to the brightness of the LEDs.
        if ((LED_1_fin>LED_2_fin + desvio) && (flag_found==0) && (conta>50)) {
            if (dir == 1) {
                turns +=1;
            }
            set(PORTD, dir_1);
            set(PORTF, dir_2);
            dir = 0;
            m_green(ON);
            conta = 0;
            flag_found = 0;
        }
        if ((LED_2_fin>LED_1_fin + desvio) && (flag_found==0) && (conta>50)) {
            if (dir == 0) {
                turns +=1;
            }
            clear(PORTD, dir_1);
            clear(PORTF, dir_2);
            dir = 1;
            m_green(OFF);
            conta = 0;
            flag_found = 0;
        }

        // Timer for time between turns
        if (check(TIFR0, OCF0A)){
            conta += 1;
            set(TIFR0,OCF0A);
        }
        
/******************************************************************************************/
/*****************                     Focus Detection                    *****************/
/******************************************************************************************/
// Stops Motors When After Some turns, it finds the maximum.

        if (turns>=focus_turns) {
            if (mx_encoder(1) == LED_3_MAX[1] || LED_3_fin < (LED_3_MAX[0] + 4)) {
                OCR3A = 0xFF;
                OCR1C = 0xFF;
                flag_found = 1;
                cap_ready = 0;
            }
        }
        
        if (flag_found == 1) {
            distance = 30 * (1 - (float)(LED_3_MAX[0]-MINcm_Value)/(MAXcm_Value-MINcm_Value)) + 160 * (LED_3_MAX[0]-MINcm_Value)/(MAXcm_Value-MINcm_Value);
            Cap_Set = 16 * (1 - (float)(distance - 29)/(162-29)) + 26 * (distance - 29)/(162-29) * 1000;   // Interpolation in measurements.
            //
            if (Cap_Charge < Cap_Set) {
                set(PORTC, 7);
            }
            else {
                clear(PORTC, 7);
                cap_ready = 1;
            }
        }
        
        if (flag_found == 1 && cap_ready == 1) {
            set(PORTE, 6);
            cap_ready = 0;
        }
/******************************************************************************************/
/*****************                      USB Interface                     *****************/
/******************************************************************************************/
     
        
        
// Displays information to USB in case it is connected.
        if ((m_usb_isconnected())) {

            
/*
            m_usb_tx_string("LED_1: ");
            m_usb_tx_int(LED_1_fin);
            m_usb_tx_string("   LED_2: ");
            m_usb_tx_int(LED_2_fin);
            m_usb_tx_string("   LED_3: ");
            m_usb_tx_int(LED_3_fin);
            
            m_usb_tx_string("   LED_3_MAX: ");
            m_usb_tx_int(LED_3_MAX[0]);
            m_usb_tx_string("   Found: ");
            m_usb_tx_int(flag_found);
            m_usb_tx_string("   MAX Enc 1: ");
            m_usb_tx_int(LED_3_MAX[1]);
            m_usb_tx_string("   Enc 1: ");
            m_usb_tx_int(mx_encoder(1));
            m_usb_tx_string("   Enc 2: ");
            m_usb_tx_int(mx_encoder(2));
           m_usb_tx_string("   Dir: ");
            m_usb_tx_int(dir);
            m_usb_tx_string("   Turns: ");
            m_usb_tx_int(turns);
            m_usb_tx_string("   Conta: ");
            m_usb_tx_int(conta);
 */
            //m_usb_tx_string("          \r");
 
        }
 
        // Uses the keyboard inputs to do things.
        if(m_usb_rx_available())
        {
            in_key = m_usb_rx_char();
            m_usb_tx_uint(in_key);          // Echo key
            m_usb_tx_string(": ");

            switch(in_key){
                case key_a:
                    //m_gpio_out(E6, TOGGLE);
                    set(PORTE, 6);
                    m_usb_tx_string(" E6 ON");
                    break;
                case key_b:
                    //m_gpio_out(C7, TOGGLE);
                    set(PORTC, 7);
                    m_usb_tx_string(" C7 ON");
                    break;
                case key_c:
                    clear(PORTE, 6);
                    m_usb_tx_string(" E6 OFF");
                    break;
                case key_d:
                    clear(PORTC, 7);
                    m_usb_tx_string(" C7 OFF");
                    break;
                case key_e:
                    
                    break;
                case key_f:
                    
                    break;
                case key_g:
                    
                    break;
                case key_h:
                    
                    break;
                case key_i:
                    
                    break;
                case key_j:
                    
                    break;
                case key_k:
                    
                    break;
                case key_l:
                    
                    break;
                case key_m:
                    
                    break;
                case key_n:
                    
                    break;
                case key_o:
                    
                    break;
                case key_p:
                    
                    break;
                case key_q:
                    
                    break;
                case key_r:
                    
                    break;
                case key_s:
                    
                    break;
                case key_t:
                    
                    break;
                case key_u:
                    
                    break;
                case key_v:
                    
                    break;
                case key_w:
                    
                    break;
                case key_x:
                    
                    break;
                case key_y:
                    
                    break;
                case key_z:
                    
                    break;
                default:
                    m_usb_tx_string("Invalid Input");
                    break;
            }                                           // End Switch Case.
            m_usb_tx_string(" Capacitor Charge: ");
            m_usb_tx_int(Cap_Charge);
            m_usb_tx_string("      \r");
        }
        
        // Display information for the SCREEN.
       /*
        //m_usb_tx_string("\r");      // Carriage Return
        //m_usb_tx_string("\n");      // Enter
        //m_usb_tx_string("\f");      // Form Feed
        //m_usb_tx_string("\f");      // Tab
        //m_usb_tx_string("\v");      // Vertical Tab
        */
        
/******************************************************************************************/
        
    }
    return 0;                       /* never reached */

}


/*
ISR(INT2_vect){
    
}
*/
