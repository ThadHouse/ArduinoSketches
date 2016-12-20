/* 
* MainIncludes.h
*
* Created: 11/28/2014 4:39:47 PM
* Author: thad
*/


#ifndef __MAININCLUDES_H__
#define __MAININCLUDES_H__

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>




typedef uint8_t boolean;
typedef uint8_t byte;

#define TX_RX_LED_INIT	DDRD |= (1<<5), DDRB |= (1<<0)
#define TXLED0			PORTD |= (1<<5)
#define TXLED1			PORTD &= ~(1<<5)
#define RXLED0			PORTB |= (1<<0)
#define RXLED1			PORTB &= ~(1<<0)


#define min(a,b) ((a)<(b)?(a):(b))

#ifdef abs
#undef abs
#endif




#endif //__MAININCLUDES_H__
