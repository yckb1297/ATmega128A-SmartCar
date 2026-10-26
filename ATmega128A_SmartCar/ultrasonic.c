#include "ultrasonic.h"

volatile int ultrasonic_dis1 = 0;
volatile int ultrasonic_dis2 = 0;
volatile int ultrasonic_dis3 = 0;
volatile char scm1[50];
volatile char scm2[50];
volatile char scm3[50];
volatile uint8_t left_flag = 0;	//하강 엣지 감지용 플래그
volatile uint8_t front_flag = 0;
volatile uint8_t right_flag = 0;

void init_ultrasonic(void);
void make_trigger1(void);
void make_trigger2(void);
void make_trigger3(void);
void ultrasonic_processing(void);

//P278 표 12-3
//INT4: PE4 외부 INT4 초음파 센서 상승/하강 엣지 발생시 ISR 수행
//상승엣지에서 1번, 하강엣지에서 1번, 총 2번 ISR 수행
//0x000A
ISR(INT4_vect){
	if(ECHO1_PORT & (1 << ECHO1_PIN)){	//1. 상승 엣지
		TCNT1 = 0;	//timer counter1을 0으로 reset
	}
	else {
		/*								//2. 하강 엣지
		TCNT1: timer counter 1
		0.000064sec * 10 = 0.00064sec(640us)
		640us/58(1cm 이동 소요 시간) : 11cm
		1sec: 1000000us
		*/
		ultrasonic_dis1 = (TCNT1 * 4) / 58;	//총 소요 시간 계산
		//sprintf: 지정된 형식에 따라 데이터를 문자열로 변환하여 메모리 버퍼에 저장
		sprintf((char *)scm1, "Left dis: %dcm\n", ultrasonic_dis1);	//소요 시간을 cm로 환산
		left_flag = 1;
	}
}

ISR(INT5_vect){
	if(ECHO2_PORT & (1 << ECHO2_PIN)){	//1. 상승 엣지
		TCNT1 = 0;	//timer counter1을 0으로 reset
	}
	else {								//2. 하강 엣지
		ultrasonic_dis2 = (TCNT1 * 4) / 58;	//소요 시간
		sprintf((char *)scm2, "front dis: %dcm\n", ultrasonic_dis2);	//소요 시간을 cm로 환산
		front_flag = 1;
	}
}

ISR(INT6_vect){
	if(ECHO3_PORT & (1 << ECHO3_PIN)){	//1. 상승 엣지
		TCNT1 = 0;	//timer counter1을 0으로 reset
	}
	else {								//2. 하강 엣지
		ultrasonic_dis3 = (TCNT1 * 4) / 58;	//소요 시간
		sprintf((char *)scm3, "Right dis: %dcm\n", ultrasonic_dis3);	//소요 시간을 cm로 환산
		right_flag = 1;
	}
}


void init_ultrasonic(void){
	//output mode로 설정
	TRIG1_DDR |= 1 << TRIG1_PIN;	//좌측
	TRIG2_DDR |= 1 << TRIG2_PIN;	//정면
	TRIG3_DDR |= 1 << TRIG3_PIN;	//우측
	//input mode로 설정
	ECHO1_DDR &= ~(1 << ECHO1_PIN);	
	ECHO2_DDR &= ~(1 << ECHO2_PIN);
	ECHO3_DDR &= ~(1 << ECHO3_PIN);
	
	// External Interrupt 허용
	EIMSK |= (1 << INT4) | (1 << INT5) | (1 << INT6);
	
	// 0 1: 상승/하강 엣지 둘 다 INT를 띄우도록 요청
	EICRB = (1 << ISC60) | (1 << ISC50) | (1 << ISC40);
	
	//16bit timer/counter 1번을 사용
	//16bit timer1: 16bit로 표시할 수 있는 최댓값: 65535(max, 0xffff)
	//시스템 클럭: 16Mhz을 64분주
	TCCR1B &= ~(1 << CS12 | 1 << CS11 | 1 << CS10);
	TCCR1B |= (1 << CS11 | 1 << CS10);	//64분주
	
	left_flag = 0;
	front_flag = 0;
	right_flag = 0;
	TCNT1 = 0;
	
	make_trigger1();
}

void make_trigger1(void){
	TRIG1_PORT &= ~(1 << TRIG1_PIN);	//low
	_delay_us(1);
	
	TRIG1_PORT |= (1 << TRIG1_PIN);		//high
	_delay_us(15);	
	TRIG1_PORT &= ~(1 << TRIG1_PIN);	//low
}

void make_trigger2(void){
	TRIG2_PORT &= ~(1 << TRIG2_PIN);	//low
	_delay_us(1);
	
	TRIG2_PORT |= (1 << TRIG2_PIN);		//high
	_delay_us(15);
	TRIG2_PORT &= ~(1 << TRIG2_PIN);	//low
}

void make_trigger3(void){
	TRIG3_PORT &= ~(1 << TRIG3_PIN);	//low
	_delay_us(1);
	
	TRIG3_PORT |= (1 << TRIG3_PIN);		//high
	_delay_us(15);
	TRIG3_PORT &= ~(1 << TRIG3_PIN);	//low
}

void ultrasonic_processing(void){
	if(left_flag == 1){	//1초
		left_flag = 0;
		printf("%s", scm1);
		
		_delay_ms(20);
		make_trigger2();
	}
	
	else if(front_flag == 1){	//1초
		front_flag = 0;
		printf("%s", scm2);
		
		_delay_ms(20);
		make_trigger3();
	}
	
	else if(right_flag == 1){	//1초
		right_flag = 0;
		printf("%s", scm3);
		
		_delay_ms(20);
		make_trigger1();
	}
}