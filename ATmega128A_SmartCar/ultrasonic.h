#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

//1번이 좌측, 2번이 정면, 3번이 우측

#define TRIG1_DDR	DDRA	
#define TRIG1_PORT	PORTA
#define TRIG1_PIN	0

#define TRIG2_DDR	DDRA
#define TRIG2_PORT	PORTA
#define TRIG2_PIN	1

#define TRIG3_DDR	DDRA
#define TRIG3_PORT	PORTA
#define TRIG3_PIN	2

#define ECHO1_DDR	DDRE
#define ECHO1_PORT	PINE	// external INT4 (interrupt)
#define ECHO1_PIN	4

#define ECHO2_DDR	DDRE
#define ECHO2_PORT	PINE	// external INT5 (interrupt)
#define ECHO2_PIN	5

#define ECHO3_DDR	DDRE
#define ECHO3_PORT	PINE	// external INT6 (interrupt)
#define ECHO3_PIN	6

