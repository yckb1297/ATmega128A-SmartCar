 #define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define MANUAL_MODE 0
#define AUTO_MODE 1
#define AUTO_MODE_CHECK 2
#define DISTANCE_CHECK 3