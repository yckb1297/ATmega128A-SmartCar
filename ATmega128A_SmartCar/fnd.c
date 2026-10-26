#include "fnd.h"

extern volatile uint32_t msec_count;

extern void forward(int speed);
extern void backward(int speed);
extern void turn_left(int speed);
extern void turn_right(int speed);
extern void stop(void);

extern int forward_state;
extern int backward_state;
extern int turn_left_state;
extern int turn_right_state;
extern int stop_state;

extern int backward_cnt;
extern int turn_left_cnt;
extern int turn_right_cnt;

extern int button0_state;

void init_fnd(void);
void fnd_on(void);
void fnd_display_processing(void);
void driving_timer(int is_active);
void driving_state(int is_active);
void driving_state_count(int button_state);


uint32_t ms_count = 0;	//ms를 재는 count uint32_t: unsigned int
uint32_t sec_count = 0;	//sec를 재는 count uint32_t: unsigned int
uint8_t fnd_font[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x98, 0x7f};
uint8_t fnd_index = 0;


volatile uint8_t fnd1_buffer[4] = {0xc0, 0xc0, 0xc0, 0xc0};
volatile uint8_t fnd2_buffer[4] = {0xc0, 0xc0, 0xc0, 0xc0};
volatile uint8_t fnd_power_state = 0;	//0: fnd off, 1: fnd on
volatile uint8_t timer_run_flag = 0;	//0: 타이머 정지, 1: 타이머 작동

void init_fnd(void){
	
	FND_DATA_DDR = 0xff;	//출력 모드로 설정. 출력:1, 입력: 0
	FND1_DIGIT_DDR |= 1 << FND1_DIGIT_D1 | 1<< FND1_DIGIT_D2 | 1<<FND1_DIGIT_D3 | 1<<FND1_DIGIT_D4;
	FND2_DIGIT_DDR |= 1 << FND2_DIGIT_D1 | 1<< FND2_DIGIT_D2 | 1<<FND2_DIGIT_D3 | 1<<FND2_DIGIT_D4;
	
	FND_DATA_PORT = 0xff;	//꺼진 상태로 초기화
	
	FND1_DIGIT_PORT = 0x00;	//자릿수 선택 해제
	FND2_DIGIT_PORT &= 0x0F;
}

void fnd_on(void){
	
	static int digit_select = 0;
	
	FND1_DIGIT_PORT &= ~(1 << FND1_DIGIT_D1 | 1 << FND1_DIGIT_D2 | 1 << FND1_DIGIT_D3 | 1 << FND1_DIGIT_D4);
	FND2_DIGIT_PORT &= 0x0F;
	
	switch(digit_select){
		case 0:		//1단위(digit 4)
		FND1_DIGIT_PORT |= (1 << FND1_DIGIT_D4);
		FND2_DIGIT_PORT |= (1 << FND2_DIGIT_D4);
		FND_DATA_PORT = fnd_font[0];
		break;
		
		case 1:		//10단위
		FND1_DIGIT_PORT |= (1 << FND1_DIGIT_D3);
		FND2_DIGIT_PORT |= (1 << FND2_DIGIT_D3);
		FND_DATA_PORT = fnd_font[0];
		break;
		
		case 2:		//100단위
		FND1_DIGIT_PORT |= (1 << FND1_DIGIT_D2);
		FND2_DIGIT_PORT |= (1 << FND2_DIGIT_D2);
		FND_DATA_PORT = fnd_font[0];
		break;
		
		case 3:		//1000단위
		FND1_DIGIT_PORT |= (1 << FND1_DIGIT_D1);
		FND2_DIGIT_PORT |= (1 << FND2_DIGIT_D1);
		FND_DATA_PORT = fnd_font[0];
		break;
	}

	digit_select = (digit_select + 1) % 4;	//다음 표시할 자리수
}

void fnd_display_processing(void){
	if (fnd_power_state == 0) {
		FND1_DIGIT_PORT = 0x00;
		FND2_DIGIT_PORT &= 0x0F;
		return;
	}

	static int digit_select = 0;
	static int fnd_select = 0;
	
	FND1_DIGIT_PORT = 0x00;
	FND2_DIGIT_PORT &= 0x0F;

	// 1. FND1을 켤 차례일 때 (이번 1ms)
	if (fnd_select == 0) {
		switch(digit_select){
			case 0: 
			FND_DATA_PORT = fnd1_buffer[0]; 
			FND1_DIGIT_PORT = 0x80; 
			break;
			case 1: 
			FND_DATA_PORT = fnd1_buffer[1]; 
			FND1_DIGIT_PORT = 0x40; 
			break;
			case 2: 
			FND_DATA_PORT = fnd1_buffer[2]; 
			FND1_DIGIT_PORT = 0x20;
			break;
			case 3: 
			FND_DATA_PORT = fnd1_buffer[3]; 
			FND1_DIGIT_PORT = 0x10; 
			break;
		}
		fnd_select = 1; // 다음 1ms에는 FND2를 켜도록 변경
	}
	// 2. FND2를 켤 차례일 때 (다음 1ms)
	else {
		uint8_t current_motor = FND2_DIGIT_PORT & 0x0F;
		
		switch(digit_select){
			case 0:
			FND_DATA_PORT = fnd2_buffer[0];
			FND2_DIGIT_PORT = current_motor | 0x80; // 상위 비트만 0x80으로 제어
			break;
			case 1:
			FND_DATA_PORT = fnd2_buffer[1];
			FND2_DIGIT_PORT = current_motor | 0x40; // 상위 비트만 0x40으로 제어
			break;
			case 2:
			FND_DATA_PORT = fnd2_buffer[2];
			FND2_DIGIT_PORT = current_motor | 0x20; // 상위 비트만 0x20으로 제어
			break;
			case 3:
			FND_DATA_PORT = fnd2_buffer[3];
			FND2_DIGIT_PORT = current_motor | 0x10; // 상위 비트만 0x10으로 제어
			break;
		}
		fnd_select = 0; // 다음 1ms에는 FND1을 켜도록 변경
		digit_select = (digit_select + 1) % 4; // FND1, FND2를 둘 다 한 번씩 확인한 후에 자릿수를 넘김
	}
}

void driving_timer(int is_active){

	if(is_active == 4){				//fnd2에 직진, 후진, 좌회전, 우회전, 정지 표시
		fnd_power_state = 1;
		timer_run_flag = 1;
	}
	else if(is_active !=0){
		fnd_power_state = 1;
		timer_run_flag = 0;
	}
	else {
		fnd_power_state = 1;
		
		if (forward_state == 1 || backward_state == 1 || turn_left_state == 1 || turn_right_state == 1) {
			if (stop_state == 1) {
				timer_run_flag = 0; // 정지 신호가 켜져 있으면 멈춤
			}
			else {
				timer_run_flag = 1; // 실제로 바퀴가 구르면 시계 시작
			}
		}
		else {
			timer_run_flag = 0; // 움직임 플래그가 전부 0이면 멈춤
		}
	}

	fnd1_buffer[0] = fnd_font[sec_count % 10];
	fnd1_buffer[1] = fnd_font[(sec_count / 10) % 6];
	fnd1_buffer[2] = fnd_font[(sec_count / 60) % 10];
	fnd1_buffer[3] = fnd_font[(sec_count / 600) % 6];
}


void driving_state(int is_active){
	uint8_t fnd_forward[] = {0xab, 0xdc};
	uint8_t fnd_backward[] = {0x9d, 0xe3};
	uint8_t fnd_turn_left[] = {0xc6};
	uint8_t fnd_turn_right[] = {0xf0};
	uint8_t fnd_stop[] = {0xbf};
	
	if(is_active == 4 || is_active ==0){				//fnd2에 직진, 후진, 좌회전, 우회전, 정지 표시
		fnd_power_state = 1;
		timer_run_flag = 1;
		
		
		if(forward_state == 1){
			fnd2_buffer[0] = fnd_forward[sec_count%2];
			fnd2_buffer[1] = fnd_forward[sec_count%2];
			fnd2_buffer[2] = fnd_forward[sec_count%2];
			fnd2_buffer[3] = fnd_forward[sec_count%2];
		}
		else if(backward_state == 1){
			fnd2_buffer[0] = fnd_backward[sec_count%2];
			fnd2_buffer[1] = fnd_backward[sec_count%2];
			fnd2_buffer[2] = fnd_backward[sec_count%2];
			fnd2_buffer[3] = fnd_backward[sec_count%2];
		}
		else if(turn_left_state == 1){
			fnd2_buffer[0] = fnd_turn_left[sec_count%1];
			fnd2_buffer[1] = fnd_turn_left[sec_count%1];
			fnd2_buffer[2] = fnd_turn_left[sec_count%1];
			fnd2_buffer[3] = fnd_turn_left[sec_count%1];
		}
		else if(turn_right_state == 1){	
			fnd2_buffer[0] = fnd_turn_right[sec_count%1];
			fnd2_buffer[1] = fnd_turn_right[sec_count%1];
			fnd2_buffer[2] = fnd_turn_right[sec_count%1];
			fnd2_buffer[3] = fnd_turn_right[sec_count%1];
		}
		else if(stop_state == 1){
			fnd2_buffer[0] = 0xbf;
			fnd2_buffer[1] = 0xbf;
			fnd2_buffer[2] = 0xbf;
			fnd2_buffer[3] = 0xbf;
		}
	}
	else{
		fnd_power_state = 1;
		timer_run_flag = 0;
		driving_state_count(button0_state);
	}
}

void driving_state_count(int button_state){
	
	switch(button_state){
		
		case 1:
			fnd2_buffer[0] = fnd_font[backward_cnt%10];
			fnd2_buffer[1] = fnd_font[backward_cnt/10%10];
			fnd2_buffer[2] = fnd_font[backward_cnt/100%10];
			fnd2_buffer[3] = 0xc1;
		break;
		
		case 2:
			fnd2_buffer[0] = fnd_font[turn_left_cnt%10];
			fnd2_buffer[1] = fnd_font[turn_left_cnt/10%10];
			fnd2_buffer[2] = fnd_font[turn_left_cnt/100%10];
			fnd2_buffer[3] = 0xc6;
		break;
		
		case 3:
			fnd2_buffer[0] = fnd_font[turn_right_cnt%10];
			fnd2_buffer[1] = fnd_font[turn_right_cnt/10%10];
			fnd2_buffer[2] = fnd_font[turn_right_cnt/100%10];
			fnd2_buffer[3] = 0xf0;
		break;
		
		case 5:
		fnd2_buffer[0] = fnd_font[backward_cnt%10];
		fnd2_buffer[1] = fnd_font[backward_cnt/10%10];
		fnd2_buffer[2] = fnd_font[backward_cnt/100%10];
		fnd2_buffer[3] = 0xc1;
		break;
		
		case 6:
		fnd2_buffer[0] = fnd_font[turn_left_cnt%10];
		fnd2_buffer[1] = fnd_font[turn_left_cnt/10%10];
		fnd2_buffer[2] = fnd_font[turn_left_cnt/100%10];
		fnd2_buffer[3] = 0xc6;
		break;
		
		case 7:
		fnd2_buffer[0] = fnd_font[turn_right_cnt%10];
		fnd2_buffer[1] = fnd_font[turn_right_cnt/10%10];
		fnd2_buffer[2] = fnd_font[turn_right_cnt/100%10];
		fnd2_buffer[3] = 0xf0;
		break;
	}
	
}