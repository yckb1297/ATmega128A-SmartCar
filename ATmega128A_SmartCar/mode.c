#include "def.h"

extern void forward(int speed);
extern void backward(int speed);
extern void turn_left(int speed);
extern void turn_right(int speed);
extern void stop(void);
extern void init_motor_driver(void);
extern void back_left(int speed);
extern void back_right(int speed);

extern volatile uint8_t bt_data;
extern volatile int ultrasonic_dis1;
extern volatile int ultrasonic_dis2;
extern volatile int ultrasonic_dis3;

void manual_mode(void);
void auto_mode(int is_driving);
void auto_mode_check(void);
void distance_check(void);



int func_state = MANUAL_MODE;

void(*pfunc[]) () = {
	manual_mode,
	auto_mode,
	auto_mode_check,
	distance_check
};

void manual_mode(void){
	switch(bt_data){
		case 'F':
		case 'f':
		forward(500);	//4us * 500 = 0.002sec(2ms)
		break;
		
		case 'B':
		case 'b':
		backward(500);
		break;
		
		case 'L':
		case 'l':
		turn_left(500);	//4us * 700 = 0.0028sec
		break;
		
		case 'R':
		case 'r':
		turn_right(500);
		break;
		
		case 'S':
		case 's':
		stop();
		break;
		
		default:
		break;
	}
	bt_data = 0;
}

int is_turning = 0;

void auto_mode(int is_driving){
	
	if(is_driving == 4){
		if (is_driving == 4) {

			// [강제 직각 좌회전 모드]
			// 정면 벽이 완전히 사라져서 30cm 이상 확보될 때까지 제자리 좌회전만 반복합니다.
			if (is_turning == 1) {
				if (ultrasonic_dis2 < 35) {
					turn_left(420); // 바퀴가 굴러가는 속도(400)로 짧게 회전
					} else {
					is_turning = 0; // 정면이 완전히 뚫리면 강제 탈출 모드 해제
				}
			}
			
			// [강제 직각 우회전 모드]
			else if (is_turning == 2) {
				if (ultrasonic_dis2 < 35) {
					turn_right(420);
					} else {
					is_turning = 0;
				}
			}
			
			// [일반 주행 모드]
			else {
				// 정면이 막혔을 때 (직각 코너 또는 벽 직면)
				if (ultrasonic_dis2 < 22) {
					
					if (ultrasonic_dis1 <= 12 && ultrasonic_dis3 <= 12) {
						backward(500);
					}
					// 정면이 막혔고 왼쪽에 조금의 여유가 있다면 직각 좌회전
					else if (ultrasonic_dis1 >= ultrasonic_dis3) {
						is_turning = 1;
					}
					// 정면이 막혔고 오른쪽에 여유가 있다면 직각 우회전
					else {
						is_turning = 2;
					}
				}
				
				// 평상시 전진 및 미세 경로 보정 (정면 거리가 22cm 이상)
				else {
					if (ultrasonic_dis1 <= 15) {
						turn_right(380);
					}
					else if (ultrasonic_dis3 <= 15) {
						turn_left(380);
					}
					else {
						forward(500);
					}
				}
			}
		}
		else if (is_driving == 0) {
			stop();
			is_turning = 0;
		}
	}
	else if (is_driving == 0) {
		stop();
		is_turning = 0;
	}
			}