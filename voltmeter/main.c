/*
 * voltmeter.c
 *
 * Created: 08/07/2020 5:33:20 pm
 * Author : John
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <math.h>

#include "pinDefines.h"
#include "USART.h"

#define REF_VCC 5.08
#define VOLTAGE_DIV_FACTOR 3.114

void initADC(void) {
	//ADMUX |= (0b00001111 & PORTC0);
	ADMUX |= (1 << REFS0);
	ADCSRA |= (1 << ADPS0) |(1 << ADPS1) | (1 << ADPS2);
	ADCSRA |= (1 << ADEN);
}

void setupADCSleepmode(void) {
	set_sleep_mode(SLEEP_MODE_ADC);
	ADCSRA |= (1 << ADIE);
	sei();
}

EMPTY_INTERRUPT(ADC_vect);

uint16_t oversample16x(void) {
	uint16_t oversampledValue = 0;
	uint8_t i;
	for(i = 0; i < 16; i++) {
		sleep_mode();
		oversampledValue += ADC;
	}
	return (oversampledValue >> 2);
}

void printFloat(float number) {
	number = round(number * 100) / 100;
	transmitByte('0' + number / 10);
	transmitByte('0' + number - 10 * floor(number / 10));
	transmitByte('.');
	transmitByte('0' + (number * 10) - floor(number) * 10);
	transmitByte('0' + (number * 100) - floor(number * 10) * 10);
	printString("\r\n");
}
int main(void)
{
	float voltage;
    initUSART();
	printString("\r\nDigital Voltmeter\r\n\r\n");
	initADC();
	setupADCSleepmode();
    while (1) 
    {
		voltage = oversample16x() * VOLTAGE_DIV_FACTOR * REF_VCC / 4096;
		printFloat(voltage);
		_delay_ms(500);
    }
}

