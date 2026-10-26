#include "LED.h"

void init_led(void){
	DDRG |= (1 << PG3);     // PG3 출력 설정
	PORTG &= ~(1 << PG3);   // LED 초기 상태 OFF
}

void led1_on(void){
	PORTG |= (1 << PG3);    // PG3 HIGH → LED ON
}

void led1_off(void){
	PORTG &= ~(1 << PG3);   // PG3 LOW → LED OFF
}