#include "pwm.h"

extern int get_button(int button_num, int button_pin);

void init_timer1_pwm(void);
void init_motor_driver(void);
void forward(int speed);
void backward(int speed);
void turn_left(int speed);
void turn_right(int speed);
void stop(void);
void back_left(int speed);
void back_right(int speed);

int forward_state = 0;
int backward_state = 0;
int turn_left_state = 0;
int turn_right_state = 0;
int stop_state = 0;

int backward_cnt = 0;
int turn_left_cnt = 0;
int turn_right_cnt = 0;

//16bit 1번 timer/counter 사용
/*
	PWM 출력 신호
	=============
	PB5: OC1A, 왼쪽 바퀴
	PB6: OC1B, 오른족 바퀴
	BTN0: auto/manual mode 설정
	
	방향 설정
	=========
	1. 왼쪽 바퀴:
	PORTF0 -- IN1
	PORTF1 -- IN2
	2. 오른쪽 바퀴: 
	PORTF2 -- IN3
	PORTF3 -- IN4
	
	IN1/IN3		IN2/IN4
	=======		=======
	   0		   1	: 역회전
	   1		   0	: 정회전
	   1		   1	: STOP
*/

#define MOTOR_PWM_DDR DDRB
#define MOTOR_LEFT_PORT_DDR 5		//OC1A
#define MOTOR_RIGHT_PORT_DDR 6		//OC1B

#define MOTOR_DIRECTION_PORT PORTF
#define MOTOR_DIRECTION_PORT_DDR DDRF

void init_timer1_pwm(void){
	/*
	* 분주비 설정: 64
	* 16000000Hz / 64 = 250000Hz (250kHz)
	* 주기 T = 1/250000 = 0.000004sec = 4us
	* 250000Hz에서 256개 펄스 count 소요시간: 1.02ms, 127개: 0.5ms
	* 0x3ff(1023) --> 4ms
	* TCNT3: 0~255(0xff)까지 count한 후 0으로 다시 초기화.
	* Duty Cycle = (OCR1C / TOP) * 100
	*/
	TCCR1B |= 1 <<CS11 | 1 << CS10;	//분주비 64로 설정
		
	//표 15-6 모드 14: 고속 PWM 모드 사용 (P327, 표 14-5)
	TCCR1A |= 1 << WGM11;				//top --> ICR1으로 설정
	TCCR1B |= 1 << WGM13 | 1 << WGM12;
	
	//비반전 모드 top: ICR1 비교일치 값(PWM) 지정 OCR1A, OCR1B (표 15-7)
	//비교 일치 방생시, OCR1A, OCR1B의 출력핀은 LOW로 바뀌고 BOTTOM에서 HIGH로 바뀜.
	TCCR1A |= 1 << COM1A1 | 1 << COM1B1;
	
	OCR1A = 0;
	OCR1B = 0;
	
	ICR1 = 1023;	//1023 *4us ==> 4ms TOP값
}

void init_motor_driver(void){
	//1. 0으로 초기화, 출력 모드로 설정
	MOTOR_PWM_DDR &= ~(1 << 5 | 1 << 6)	;	
	MOTOR_PWM_DDR |= 1 << 5 | 1 << 6;
	MOTOR_DIRECTION_PORT_DDR &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3);
	MOTOR_DIRECTION_PORT_DDR |= 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3;
	
	//2. 모터를 전진 모드로 설정
	MOTOR_DIRECTION_PORT &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3); //초기화
	MOTOR_DIRECTION_PORT |= 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3;
	MOTOR_DIRECTION_PORT |= 1 << 0 | 1 << 2;	//전진모드	IN4 IN3 IN2 IN1: 0 1 0 1
}

void forward(int speed){
	forward_state = 1;
	backward_state = 0;
	turn_left_state = 0;
	turn_right_state = 0;
	stop_state = 0;
	
	MOTOR_DIRECTION_PORT &= 0xF0;
	MOTOR_DIRECTION_PORT |= 1 << 0 | 1 << 2;	
	
	OCR1A = OCR1B = speed;	//OCR1A : PWM left, OCR1B: PWM right
}

void backward(int speed){
	forward_state = 0;
	backward_state = 1;
	turn_left_state = 0;
	turn_right_state = 0;
	stop_state = 0;
	
	backward_cnt++;
	
	MOTOR_DIRECTION_PORT &= 0xF0;
	MOTOR_DIRECTION_PORT |= 1 << 1 | 1 << 3;	//후진모드	IN4 IN3 IN2 IN1: 1 0 1 0
	
	OCR1A = OCR1B = speed;	//OCR1A : PWM left, OCR1B: PWM right
}

void turn_left(int speed){
	forward_state = 0;
	backward_state = 0;
	turn_left_state = 1;
	turn_right_state = 0;
	stop_state = 0;
	
	turn_left_cnt++;
	
	MOTOR_DIRECTION_PORT &= 0xF0;
	MOTOR_DIRECTION_PORT |= 1 << 0 | 1 << 2;
	
	OCR1A = 0;	//OCR1A : PWM left, OCR1B: PWM right
	OCR1B = speed;
}

void back_left(int speed){

	MOTOR_DIRECTION_PORT &= 0xF0;
	MOTOR_DIRECTION_PORT |= 1 << 1 | 1 << 3;
	
	OCR1A = 0;	//OCR1A : PWM left, OCR1B: PWM right
	OCR1B = speed;
}

void turn_right(int speed){
	forward_state = 0;
	backward_state = 0;
	turn_left_state = 0;
	turn_right_state = 1;
	stop_state = 0;
	
	turn_right_cnt++;
	
	MOTOR_DIRECTION_PORT &= 0xF0;
	MOTOR_DIRECTION_PORT |= 1 << 0 | 1 << 2;
	
	OCR1A = speed;
	OCR1B = 0;	//OCR1A : PWM left, OCR1B: PWM right
}

void back_right(int speed){
	
	turn_right_cnt++;
	
	MOTOR_DIRECTION_PORT &= 0xF0;
	MOTOR_DIRECTION_PORT |= 1 << 1 | 1 << 3;
	
	OCR1A = speed;
	OCR1B = 0;	//OCR1A : PWM left, OCR1B: PWM right
}

void stop(void){
	forward_state = 0;
	backward_state = 0;
	turn_left_state = 0;
	turn_right_state = 0;
	stop_state = 1;
	
	MOTOR_DIRECTION_PORT &= 0xF0;
	MOTOR_DIRECTION_PORT |= 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3;
	
	OCR1A = OCR1B = 0;	//OCR1A : PWM left, OCR1B: PWM right
}