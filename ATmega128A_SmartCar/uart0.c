#include "uart0.h"

extern int func_state;
extern void (*fp[]) ();

void init_uart0(void);
void UART0_transmit(uint8_t data);

//PC로부터 1byte가 들어오면 자동적으로 ISR 진입
ISR(USART0_RX_vect){
	
	volatile uint8_t data;
	volatile static int i=0;
	
	data = UDR0;	//UDR0의 내용이 data에 복사된 후 UDR0의 내용은 빈 상태가 됨.
	
	if(data == '\n' || data == '\r'){
		if((rear+1) % QUEUE_SIZE == front % QUEUE_SIZE){	// queue full 상태일 때
			return;
		}
		rx_buff[rear][i++] = '\0';	//문장의 끝인 NULL 추가
		i = 0;	//다음 string을 저장 하기 위해서 i를 0으로 만듦
		rear = (rear + 1) % QUEUE_SIZE;	//rear값: 0~9 순회
	}
	else{
		if((rear+1) % QUEUE_SIZE == front % QUEUE_SIZE){	// queue full 상태일 때
			return;
		}
		rx_buff[rear][i++] = data;
	}
}

/*
1. 전송속도: 9600bps
2. start/stop 설정
3. RX(수신): interrupt로 설정
*/

void init_uart0(void){
	
	//1. 전송 속도: 9600bps
	UBRR0H = 0x00;
	UBRR0L = 207;	//9600bps 교재 표 8-9
	UCSR0A |= 1 << U2X0;	//2배속 설정 (sampling 8)
	// UART0를 송신, 수신이 다 가능하고 RX INT가 가능하도록 설정
	UCSR0B |= 1 <<RXEN0 | 1 << TXEN0 | 1<< RXCIE0;
}

//UART0로 1byte를 전송하는 함수
void UART0_transmit(uint8_t data){
	
	while(!(UCSR0A & 1 << UDRE0))	//data가 송신 중이면 송신이 끝날 때까지 기다림
	;	//No operation
	UDR0 = data;	//HW 전송 register에 data를 송신
	
}
