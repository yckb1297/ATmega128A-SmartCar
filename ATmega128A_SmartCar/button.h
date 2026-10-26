#define F_CPU 16000000UL  //16MHz
#include <avr/io.h>
#include <util/delay.h>

#define  BUTTON_DDR  DDRG
#define  BUTTON_PIN  PING

#define  BUTTON0PIN 0    

#define  BUTTON0   0   

#define  BUTTON_NUMBER 1


#define  BUTTON_PRESS  1
#define  BUTTON_RELEASE 0