//FND 유형: common anode(WCN4-0056B7-A11)

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define FND_DATA_DDR DDRC		//세그먼트 입/출력 방향 설정 주소
#define FND_DATA_PORT PORTC		//세그먼트 입/출력 값 입력 주소

#define FND1_DIGIT_DDR DDRD		//digit 입/출력 방향
#define FND1_DIGIT_PORT PORTD	//digit 선택 값

#define FND1_DIGIT_D1 4	//DIGIT 1은 PORTC의 4번핀
#define FND1_DIGIT_D2 5
#define FND1_DIGIT_D3 6
#define FND1_DIGIT_D4 7


#define FND2_DIGIT_DDR DDRF		//digit 입/출력 방향
#define FND2_DIGIT_PORT PORTF	//digit 선택 값

#define FND2_DIGIT_D1 4	//DIGIT 1은 PORTC의 4번핀
#define FND2_DIGIT_D2 5
#define FND2_DIGIT_D3 6
#define FND2_DIGIT_D4 7


