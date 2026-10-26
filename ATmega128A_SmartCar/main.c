#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>	//interrupt 라이브러리. sei 등의 함수가 포함됨.
#include <stdio.h>
#include "button.h"

extern int led_main(void);
extern void init_led(void);
extern void led1_on(void);
extern void led1_off(void);

extern void init_button(void);
extern int get_button(int button_num, int button_pin);

extern void init_ultrasonic(void);
extern void ultrasonic_processing(void);

extern void init_uart0(void);
extern void UART0_transmit(uint8_t data);
extern void pc_command_processing(void);

extern void init_timer1_pwm(void);
extern void init_motor_driver(void);

extern void init_uart1(void);
extern volatile uint8_t bt_data;

extern void init_timer1_pwm(void);
extern void init_motor_driver(void);


extern void init_ultrasonic(void);
extern void ultrasonic_processing(void);

void init_timer0(void);

extern void manual_mode(void);
extern void auto_mode(int is_driving);
extern void auto_mode_check(void);
extern void distance_check(void);
extern void(*pfunc[]) ();

extern void init_fnd(void);
extern void fnd_display_processing(void);
extern void driving_timer(int is_active);
extern void driving_state(int is_active);
extern void driving_state_count(int button_state);

extern uint32_t ms_count;
extern uint32_t sec_count;
extern volatile uint8_t timer_run_flag;
extern volatile uint8_t fnd_index;

extern int backward_cnt;
extern int turn_left_cnt;
extern int turn_right_cnt;

int button0_state = 0;

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

volatile uint32_t msec_count = 0;	//volatile 최적화 방지
volatile uint32_t btnms_count = 0;
volatile int ultrasonic_check_time;

ISR(TIMER0_OVF_vect){
	
	TCNT0 = 6;	//TCNT0 6~256카운트, 250개(1ms) 카운트 후 INTERRUPT
	msec_count++;	//1ms count
	btnms_count++;
	ultrasonic_check_time++;
	
	if(timer_run_flag == 1) {
		ms_count++;
		if(ms_count >= 1000){
			ms_count = 0;
			sec_count++;
		}
	}
	fnd_display_processing();
	
}

int main(void)
{
	
	init_led();
	init_button();
	init_fnd();
	init_timer0();
	init_uart0();
	init_uart1();
	init_timer1_pwm();
	init_motor_driver();
	init_ultrasonic();
	
	stdout = &OUTPUT;	// printf가 동작 할 수 있도록 stdout을 설정.
	sei();	//전역 interrupt 허용
	
	
	
	while (1) 
    {

		if (ultrasonic_check_time >= 40) {
			ultrasonic_check_time = 0; // 주기 초기화
			
			ultrasonic_processing();
			
			driving_state(button0_state);
			driving_timer(button0_state);
			
			if (button0_state == 4) {
				auto_mode(button0_state);
			}
			else if(button0_state ==0) {
				manual_mode();
			}
		}
		
		if(get_button(BUTTON0, BUTTON0PIN)){
			button0_state = (button0_state+1) % 8;
			
			if (button0_state == 0){
				stop();
				init_motor_driver();
				ms_count = 0;
				sec_count = 0;
				backward_cnt = 0;
				turn_left_cnt = 0;
				turn_right_cnt = 0;
			}
			else if (button0_state == 1){
				init_motor_driver();
			}
			else if (button0_state == 4){
				led1_on();
				init_motor_driver();
				ms_count = 0;
				sec_count = 0;
				backward_cnt = 0;
				turn_left_cnt = 0;
				turn_right_cnt = 0;
				
			}
			else if (button0_state == 5){
				init_motor_driver();
				led1_off();
			}
		}
		
    }
}

/*
1. timer0을 초기화
 - AVR에서 8bit timer 0/2 중에서 0번을 초기화한다.
 - 임베디드에서 가장 신경을 써야 할 부분이 초기화 하는 것임.
 - 초기화가 잘못 되면 이후가 다 꼬인다.
2. 8bit 가지고 1ms를 측정하는 timer/counter를 만들고자 한다.
 2.1. 분주비 설정 (64분주).
   - 16,000,000Hz / 64 = 250,000Hz
 2.2. 1주기가 잡아먹는 시간 계산.
   - T = 1/f = 1/250,000 = 0.000004sec = 4us = 0.004ms
 2.3. 8bit로 count하는 시간 계산. (8bit timer OV(Overflow)
   - 4us * 256 = 1024us = 1.024ms
   - 0.004ms * 250 = 0.001sec(1ms)
*/

void init_timer0(void){
	
	TCNT0 = 6;	//TCNT0 6~256카운트, 250개(1ms) 카운트 후 INTERRUPT
	TCCR0 &= ~(1<<CS02 | 1<<CS01 | 1 << CS00);	//TCCR0을 초기화. 쓰레기 값이 들어가는 것을 방지.
	TCCR0 |= 1<<CS02 | 0 << CS01 | 0 << CS00; 	//분주비 64로 설정(교재 표 13-1 참고) 0b00000100
	TIMSK |= 1<<TOIE0;	//TIMER0 Overflow interrupt 활성화
}