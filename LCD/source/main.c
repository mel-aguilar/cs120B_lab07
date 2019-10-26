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
#include "io.h"
#include <avr/interrupt.h>
#include "simAVRHeader.h"
#endif
#ifdef _SIMULATE_
#endif
#define A0 (~PINA & 0x01)
#define A1 (~PINA & 0x02)

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum states{START, WAIT, LED1, LED2, LED3, RELEASE, RELEASE2, WAIT_TIME, POINTS} state;

unsigned char light = 0;
unsigned char score = 0;
unsigned char tracker = 0;
unsigned char i = 0;
const unsigned char ms300 = 2;

void tick_Num(){
	switch(state){
		case START:
			LCD_ClearScreen();
			LCD_Cursor(1);
			LCD_WriteData(score + '0');
			state = LED1;
			break;
		case WAIT:
			if(A0){
				state = RELEASE2;
			}
			else{
				state = WAIT;
			}
			break;
		case WAIT_TIME:
			if(A0){
				state = POINTS; 
				break;
			}
			else if(i < ms300){
				state = WAIT_TIME;
			}
			else{
				if(tracker == 1){
					state = LED2;
				}
				else if(tracker == 2){
					state = LED3;
				}
				else if(tracker == 3){
					state = LED1;
				}
			}
			break;
		case LED1:
			state = WAIT_TIME;
			break;
		case LED2:
			state = WAIT_TIME;
			break;
		case LED3:
			state = WAIT_TIME;
			break;
		case POINTS:
			state = RELEASE;
			break;
		case RELEASE:
			if(A0){
				state = RELEASE;
			}
			else{
				state = WAIT;
			}
			break;
		case RELEASE2:
			if(A0){
				state = RELEASE2;
			}
			else{
				if(score == 9 || score == 0){
					state = START;
				}
				else{
					state = LED1;
				}
			}
			break;
	}
	switch(state){
		case START:
			score = 5;
			break;
		case WAIT:
			break;
		case WAIT_TIME:
			i++;
			break;
		case LED1:
			light = 0x01;
			tracker = 1;
			i = 0;
			break;
		case LED2:
			light = 0x02;
			tracker = 2;
			i = 0;
			break;
		case LED3:
			light = 0x04;
			tracker = 3;
			i = 0;
			break;
		case RELEASE:
			break;
		case RELEASE2:
			break;
		case POINTS:
			if(tracker == 1 || tracker == 3){
				if(score > 0){
					score--;
				}
			}
			else if(tracker == 2){
				if(score < 9){
					score++;
				}
			}
			LCD_Cursor(1);
			LCD_WriteData(score + '0');
			if(score == 0){
				LCD_DisplayString(1, "YOU LOST        TRY AGAIN");
			}
			if(score == 9){
				LCD_DisplayString(1, "YOU WON!        YOUR SCORE: 9");
			}
			break;
	}
}

int main(void){
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	TimerSet(50);
	TimerOn();
	LCD_init();
	state = START;
	score = 5;
	LCD_Cursor(1);
	LCD_WriteData(score + '0');

	while (1){
		tick_Num();
		while(!TimerFlag){};
		TimerFlag = 0;
		PORTB = light;
	}
}
