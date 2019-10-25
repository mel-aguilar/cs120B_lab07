/*	Author: magui051
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#ifdef _SIMULATE_
#endif
#define A0 (~PINA & 0x01)
#define A1 (~PINA & 0x02)

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerISR() {
	TimerFlag = 1;
}

void TimerOff(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum states{START, INIT, INCREMENT, WAIT, DECREMENT, W10, AW10, RESET} state;


unsigned char counter = 0;
unsigned char i = 0;



void Tick_LCD() {
	switch(state){
		case START:
			state = INIT;
			break;
		case INIT:
			if(A0) {
				state = INCREMENT;
			}
			else {
				state = INIT;
			}
			break;
		case INCREMENT:
				state = W10;
			break;
		case W10:
			if(A0){
				if(A1) {
					state = RESET;
				}
				else if(i < 10){
					state = W10;
				}
				else{
					state = INCREMENT;
				}
			}
			else if (!A0) {
				state = WAIT;
			}			
			break;
		case WAIT:
			if(A0) {
				state = INCREMENT;
			}
			else if(A1) {
				state = DECREMENT;
			}
			else {
				state = WAIT;
			}
			break;
		case DECREMENT:
			if(!A1) {
				state = WAIT;
			}
			else {
				state = AW10;
			}
			break;
		case AW10:
			if(A1){
				if(A0) {
					state = RESET;
				}
				else if(i < 10){
					state = AW10;
				}
				else if(i > 10){
					state = DECREMENT;
				}
			}
			else if (!A1) {
				state = WAIT;
			}
			break;
		case RESET:
			if(!A0 && !A1) {
				state = START;
			}
			break;
	}
	switch(state){
		case START:
			counter = 0;
			i = 0;
			LCD_Cursor(1);
			LCD_WriteData(counter + '0');
			break;
		case INIT:
			break;
		case INCREMENT:
			if(counter < 9) {
				counter++;
			}
			i = 0;
			LCD_Cursor(1);
			LCD_WriteData(counter + '0');
			break;
		case W10:
			i++;
			break;
		case WAIT:
			break;
		case DECREMENT:
			if(counter > 0) {
				counter--;
			}
			i = 0;
			LCD_Cursor(1);
			LCD_WriteData(counter + '0');
			break;
		case AW10:
			i++;
			break;
		case RESET:
			break;
	}
}
int main(void) {
    /* Replace with your application code */
	DDRA = 0x00;	PORTA = 0xFF;
	DDRC = 0xFF;	PORTC = 0x00;
	DDRD = 0xFF;	PORTD = 0x00;
	
	LCD_init();
	TimerSet(100);
	TimerOn();
	
	state = START;
	counter = 0;
	LCD_Cursor(1);
	LCD_WriteData(counter + '0');
    while (1) {
		Tick_LCD();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
	return 1;
}
