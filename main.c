//Lab section:B21
//Name: Johnathan Murad
//Email: Jmura003@ucr.edu
//Assignment: custom Project
// Custom Lab  - Juke Box 
//I acknowledge all content contained herein, excluding template or example code,
//is my own original work.
 
#define wing 24
#define wing_1 114
#define wing_2 200
#define	C0 16.35
#define	Db0	17.32
#define	D0	18.35
#define	Eb0	19.45
#define	E0	20.60
#define	F0	21.83
#define	Gb0	23.12
#define	G0	24.50
#define	Ab0	25.96
#define	LA0	27.50
#define	Bb0	29.14
#define	B0	30.87
#define	C1	32.70
#define	Db1	34.65
#define	D1	36.71
#define	Eb1	38.89
#define	E1	41.20
#define	F1	43.65
#define	Gb1	46.25
#define	G1	49.00
#define	Ab1	51.91
#define	LA1	55.00
#define	Bb1	58.27
#define	B1	61.74
#define	C2	65.41
#define	Db2	69.30
#define	D2	73.42
#define	Eb2	77.78
#define	E2	82.41
#define	F2	87.31
#define	Gb2	92.50
#define	G2	98.00
#define	Ab2	103.83
#define	LA2	110.00
#define	Bb2	116.54
#define	B2	123.47
#define	C3	130.81
#define	Db3	138.59
#define	D3	146.83
#define	Eb3	155.56
#define	E3	164.81
#define	F3	174.61
#define	Gb3	185.00
#define	G3	196.00
#define	Ab3	207.65
#define	LA3	220.00
#define	Bb3	233.08
#define	B3	246.94
#define	C4	261.63
#define	Db4	277.18
#define	D4	293.66
#define	Eb4	311.13
#define	E4	329.63
#define	F4	349.23
#define	Gb4	369.99
#define	G4	392.00
#define	Ab4	415.30
#define	LA4	440.00
#define	Bb4	466.16
#define	B4	493.88
#define	C5	523.25
#define	Db5	554.37
#define	D5	587.33
#define	Eb5	622.25
#define	E5	659.26
#define	F5	698.46
#define	Gb5	739.99
#define	G5	783.99
#define	Ab5	830.61
#define	LA5	880.00
#define	Bb5	932.33
#define	B5	987.77
#define	C6	1046.50
#define	Db6	1108.73
#define	D6	1174.66
#define	Eb6	1244.51
#define	E6	1318.51
#define	F6	1396.91
#define	Gb6	1479.98
#define	G6	1567.98
#define	Ab6	1661.22
#define	LA6	1760.00
#define	Bb6	1864.66
#define	B6	1975.53
#define	C7	2093.00
#define	Db7	2217.46
#define	D7	2349.32
#define	Eb7	2489.02
#define	E7	2637.02
#define	F7	2793.83
#define	Gb7	2959.96
#define	G7	3135.96
#define	Ab7	3322.44
#define	LA7	3520.01
#define	Bb7	3729.31
#define	B7	3951.07
#define	C8	4186.01
#define	Db8	4434.92
#define	D8	4698.64
#define	Eb8	4978.03`
#include <avr/io.h>
#include "io.c"
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s
	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}



enum OStates{stop, onoff, wait, wait2} O_state;
unsigned char button2 = 0x00;
unsigned char tmpB = 0x00;
void offonoff(){
	button2 = ~PINA & 0x07;
	switch(O_state){
		case stop:
		tmpB = 0x00;
		if(button2 == 0x01 || button2 == 0x02 || button2 == 0x04){
			PWM_on();
			//PORTD = 0x00;
			O_state = wait;
		}
		break;
		
		case wait:
		tmpB = 0x00;
		if(button2 == 0x00) {
			O_state = onoff;
		}
		break;
		
		case onoff:
		if(tmpB == 0x02){
			O_state = stop;
		}
		if(button2 == 0x01 || button2 == 0x02 || button2 == 0x04){
			tmpB = 0x01;
			PWM_off();
			//PORTD = 0x00;
			O_state = wait2;
		}
		break;
		
		case wait2:
		tmpB = 0x01;
		PORTD = 0x02;
		set_PWM(0);
		if(button2 == 0x00){
			O_state = stop;
			
		}
		break;
	}
	
}

//unsigned short wing = 24; // array size
double freq[wing] = {660, 660, 660, 510, 660, 770, 380, 510, 380, 320, 440, 480, 450, 430, 380,
660, 760, 860, 700, 760, 660, 520, 580, 480};
double length[wing] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 80, 100, 100, 100,
80, 50, 100, 80, 50, 80,   80,   80,  80};
double delay[wing] = {150, 300, 300, 100, 300, 550, 575, 450, 400, 500, 300, 330, 150, 300, 200,
200, 150, 300, 150, 350, 300, 150, 150, 500};

enum states{init, note, pause, off, wait_for_release, noMore, stop_playing} state;

unsigned short counter = 0x00;
unsigned short i = 0x00;
unsigned trigger = 0x00;
unsigned char dont_play = 0x00;
unsigned char flag = 0x00;
void supermario(){
	unsigned char button = ~PINA & 0x0F;
	flag = 0x01;
	switch(state){
		case init: // wait for user to press button to begin song
			i = 0;
			if(button == 0x01){
				PWM_on();
				LCD_DisplayString(1, "Super Mario!");
				state = note;
			}
			else {
				state = init;
			}
			break;
			
		case note: // actual play time of the note
			trigger = 0x01;
			set_PWM(freq[i]);
			if(dont_play == 0x01){
				set_PWM(0);
				PWM_off();
				state = stop_playing;
			}
			if(counter < length[i]){
				counter++;
				state = note;
				PORTD = 0x01;
				if(dont_play == 0x01){
					LCD_DisplayString(1, "paused");
					set_PWM(0);
					PWM_off();
					state = stop_playing;
				}
				if(tmpB == 0x01){
					state = noMore;
				}
				if(i >= (wing)){
					state = off;
				}
			}
			
			else{
				counter = 0;
				state = pause;
			}
			break;
			
		case pause: // small delay in between notes
			trigger = 0x02;
			set_PWM(0);
			if(tmpB == 0x01){
				state = noMore;
			}
			if(dont_play == 0x01){
				state = stop_playing;
				set_PWM(0);
				PWM_off();
			}
			if(counter < delay[i]){
				counter++;
				state = pause;
				PORTD = 0x00;
				if(dont_play == 0x01){
					LCD_DisplayString(1, "paused");
					set_PWM(0);
					state = stop_playing;
					//PWM_off();
				}
				if(tmpB == 0x01){
					state = noMore;
				}
				if(i >= (wing)){
					state = off;
				}
			}
			else {
				i++;
				counter = 0;
				if(i >= (wing)){
					state = off;
				}
				state = note;
			}
			break;
			
		case stop_playing:
			if(dont_play == 0x01){
				state = stop_playing;
			}
			if(button == 0x01 || button == 0x02 || button == 0x04){
				PWM_on();
				state = wait_for_release;
				i = 0;
				counter = 0;
			}
			else {
				if(trigger == 0x01){
					PWM_on();
					LCD_DisplayString(1, "Super Mario!");
					state = note;
				}
				else if(trigger == 0x02){
					PWM_on();
					LCD_DisplayString(1, "Super Mario!");
					state = pause;
				}
			}
			break;
						
		case off: // the end of the song should visit here
			PORTD = 0x00;
			PWM_off();
			state = wait_for_release;
			i = 0;
			counter = 0;
			tmpB = 0x02;
			break;
			
		case noMore: // when the user decides to stop the song in the middle of playing
			state = wait_for_release;
			i = 0;
			counter = 0;
			break;
		 
			
		case wait_for_release: // waits for release of button 
			if(button == 0x01 || button == 0x02 || button == 0x04){
				state = wait_for_release;
			}
			else {
				LCD_DisplayString(1, "welcome to jukebox pro!");
				state = init;
				set_PWM(0);
			}
			break;
			
		default:
			state = init;
			break;
		 
	}
}

double freq_1[wing_1] =   {LA5, F5,  LA5, F5,  B5,  G5,  B5,  G5, // 1 ||  8
	C6, LA5,  C6, LA5,  D6,  G5,  D6,  G5, //2 || 8
	B4, LA4,  B4, LA4,  B4,  LA4,  C5,  B4, //3 || 8
	LA4,									   // 4 || 1
	LA4, B4, C5, LA4, B4, C5, C5, B4,		//5 || 8
	C2, C2, D2, E2, F2, F2, F2, F2,			//6 || 8
	C6, LA5, C6, B5, C6, B5, G5,			//    7 || 7
	LA5, E6, D6, C6, B5, C6, B5, G5,		//	  8 || 8
	C6, LA5, C6, B5, C6, B5, G5,			//	  9 || 7
	LA5, LA5, F5, LA5, G5, LA5, G5, C5,	//    10|| 8					//    11   4
	C6, LA5, C6, B5, C6, B5, G5,			//	  12  7
	LA5, E6, D6, C6, B5, C6, B5, G5,		//    13  8
	C6, LA5, C6, B5, C6, B5, G5,			//    14  7
	LA5, LA5, F5, LA5, G5, LA5, G5,		//    15  7
	C5, C5, E5, D5,						//    16  4
	C5, D5, E5,
	C5, C5, E5, Eb5,
	C5, E5, D5,
};
double lights_1[wing_1] = {0x01, 0x04, 0x01, 0x04, 0x02, 0x08, 0x02, 0x08,
	0x04, 0x10, 0x04, 0x10, 0x20, 0x08, 0x20, 0x08,
	0x0E, 0x07, 0x0E, 0x07, 0x0E, 0x07, 0x1C, 0x0E,
	0x07,
	0x07, 0x0E, 0x1C, 0x07, 0x0E, 0x1C, 0x1C, 0x0E,
	0x01, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x02, 0x08, 0x04, 0x08, 0x04, 0x01,
	0x08, 0x20, 0x10, 0x08, 0x02, 0x08, 0x02, 0x01,
	0x08, 0x02, 0x08, 0x04, 0x08, 0x04, 0x01,
	0x06, 0x06, 0x02, 0x06, 0x04, 0x06, 0x04, 0x01,
	0x08, 0x02, 0x08, 0x04, 0x08, 0x04, 0x01,
	0x08, 0x20, 0x10, 0x08, 0x02, 0x08, 0x02, 0x01,
	0x08, 0x02, 0x08, 0x04, 0x08, 0x04, 0x01,
	0x06, 0x06, 0x02, 0x06, 0x04, 0x06, 0x04,
	0x01, 0x01, 0x06, 0x03,
	0x01, 0x02, 0x04,
	0x01, 0x01, 0x04, 0x03,
0x01, 0x04, 0x3F};

double length_1[wing_1] = {100, 100, 100, 100, 100, 100 ,100, 100,
	100, 100, 100, 100, 100, 100 ,100, 100,
	600, 600, 600, 600, 400, 400 ,600, 600,
	2000,
	600, 600, 400, 600, 600, 400, 600, 1800,
	200, 200, 200, 200, 200, 200, 200, 200,
	200, 400, 200, 400, 200, 400, 1200,
	200, 200, 400, 200, 400, 200, 400, 1400,
	200, 400, 200, 400, 200, 400, 1200,
	200, 200, 400, 200, 400, 200, 400, 1400,
	200, 400, 200, 400, 200, 400, 1200,
	200, 200, 400, 200, 400, 200, 400, 1400,
	200, 400, 200, 400, 200, 400, 1200,
	200, 200, 400, 200, 400, 200, 400,
	200, 200, 200, 2400,
	200 ,200, 2600,
	200, 200, 200, 2400,
	200, 200, 3200,
};
double delay_1[wing_1] =  {1,   1,   1,   1,   1,   1,   1,   1,
	1,   1,   1,   1,   1,   1,   1,   25,
	1, 1,   1,   1,   1,   1,   1,   1,
	15,
	1, 1, 1, 1, 1, 25, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 100,
	1, 1, 1,
	1, 1, 1, 1,
1, 1, 1};

enum states_1{init_1, note_1, pause_1, off_1, wait_for_release_1, noMore_1} state_1;

unsigned short counter_1 = 0x00;
unsigned short i_1 = 0x00;

void sanic(){
	flag = 0x02;
	unsigned char button = ~PINA & 0x07;
	switch(state_1){
		case init_1: // wait for user to press button to begin song
		if(button == 0x02){
			PWM_on();
			LCD_DisplayString(1, "Green Hill Zone!");
			state_1 = note_1;
		}
		else {
			state_1 = init_1;
		}
		break;
		
		case note_1: // actual play time of the note
		set_PWM(freq_1[i_1]);
		if(counter_1 < length_1[i_1]){
			counter_1++;
			state_1 = note_1;
			PORTD = lights_1[i_1];
			if(tmpB == 0x01){
				state_1 = noMore_1;
			}
			if(i_1 >= (wing_1)){
				state_1 = off_1;
			}
		}
		
		else{
			counter_1 = 0;
			state_1 = pause_1;
		}
		break;
		
		case pause_1: // small delay in between notes
		set_PWM(0);
		if(tmpB == 0x01){
			state_1 = noMore_1;
		}
		if(counter_1 < delay_1[i_1]){
			counter_1++;
			state_1 = pause_1;
			PORTD = 0x00;
			if(tmpB == 0x01){
				state_1 = noMore_1;
			}
			if(i_1 >= (wing_1)){
				state_1 = off_1;
			}
		}
		else {
			i_1++;
			counter_1 = 0;
			if(i_1 >= (wing_1)){
				state_1 = off_1;
			}
			state_1 = note_1;
		}
		break;
		
		case off_1: // the end of the song should visit here
		PORTD = 0x00;
		PWM_off();
		state_1 = wait_for_release_1;
		i_1 = 0;
		counter_1 = 0;
		tmpB = 0x02;
		break;
		
		case noMore_1: // when the user decides to stop the song in the middle of playing
		state_1 = wait_for_release_1;
		i_1 = 0;
		counter_1 = 0;
		break;
		
		case wait_for_release_1: // waits for release of button
		if(button == 0x01 || button == 0x02 || button == 0x04){
			state_1 = wait_for_release_1;
		}
		else {
			LCD_DisplayString(1, "welcome to jukebox pro!");
			state_1 = init_1;
			set_PWM(0);
		}
		break;
		
		default:
		state = init_1;
		break;
		
	}
}

double freq_2[wing_2] =		{
	C5, LA5, G5, C5,				// m1 = 4
	C5, LA5, G5, D5, D5, C5,		// m2 = 6
	C5, LA5, G5, C5, E5,			// m3 = 5
	E5, C5, D5, E5, C5, D5, E5, C5, D5, E5, C5, D5, //m4 = 12
	C5, LA5, G5, C5,				// 5 - 4
	C5, LA5, G5, D5, E5, G5,		//6 - 6
	G5, LA5, G5, C5, E5,			//7 - 5
	E5, C5, D5, E5, C5, D5, E5, C5, D5, E5, C5, D5, //8 - 12
	C5, C5, G5, F5, E5, D5, C5, //9 - 7
	D5, D5, C5, D5, D5, B4,		//10 - 6
	D5, E5, F5, E5, D5, C5, C5, // 11 - 76
	LA4, C5, G5, F5, E5, D5, C5, // 12 -- 7
	D5, D5, C5, D5, D5,		//13 -- 5
	D5, E5, F5, E5, D5, C5,	//14 -- 6
	C5, C5, B4, C5,			// 15 -- 4
	G5, E5, D5, E5, C5, C5,	// 16 -- 6
	LA5, G5, D5, C5,			// 17 -- 108
	G5, E5, D5, E5, C5, C5,
	LA5, G5, D5, G5, G5,
	LA5, G5, E5, D5,
	D5, E5, F5, E5, D5, C5,	//129
	LA5, G5, E5, D5,
	G2, C4, D4, G4, C5, D5, G5, C5, D5, G5, C6, D6, G6, E5, //147
	C5, LA5, G5, C5,
	C5, LA5, G5, D5, D5, C5,
	C5, LA5, G5, C5, E5,
	E5, C5, D5, E5, C5, D5, E5, C5, D5, E5, C5, D5,
	C5, LA5, G5, C5,
	C5, LA5, G5, D5, E5, G5,
	G5, LA5, G5, C5, E5,
E5, C5, D5, E5, C5, D5, E5, C5, D5, E5, C5, D5,};

double light_show[wing_2] = {0x01, 0x08, 0x04,0x01,
							 0x01, 0x08, 0x04, 0x02, 0x02, 0x01,
							 0x01, 0x08, 0x04, 0x01, 0x04,
							 0x04, 0x01, 0x02, 0x04, 0x01, 0x02, 0x04, 0x01, 0x02, 0x04, 0x01, 0x02, 
							 0x01, 0x08, 0x04,0x01,
							 0x01, 0x08, 0x04, 0x02, 0x03, 0x04,
							 0x04, 0x08, 0x04, 0x01, 0x04, 
							 0x04, 0x01, 0x02, 0x04, 0x01, 0x02, 0x04, 0x01, 0x02, 0x04, 0x01, 0x02, 
							 0x02, 0x02, 0x20, 0x10, 0x08, 0x04, 0x02, 
							 0x04, 0x04, 0x02, 0x04, 0x04, 0x01,
							 0x04, 0x08, 0x10, 0x08, 0x04, 0x02, 0x02, 
							 0x01, 0x02, 0x20, 0x10, 0x08, 0x04, 0x02,
							 0x04, 0x04, 0x02, 0x04, 0x04,
							 0x04, 0x08, 0x10, 0x08, 0x04, 0x02,
							 0x02, 0x02, 0x01, 0x02, 
							 0x10, 0x08, 0x04, 0x08, 0x02, 0x02,
							 0x20, 0x01, 0x04, 0x02,
							 0x10, 0x08, 0x04, 0x08,0x02, 0x02,
							 0x20, 0x10, 0x04, 0x10, 0x10,
							 0x20, 0x10,0x08, 0x04,
							 0x04, 0x08, 0x10, 0x08, 0x04, 0x02,
							 0x20, 0x10, 0x08, 0x04,
							 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x04, 0x08, 0x10, 0x20, 0x30, 0xFF, 0x10,
							 0x01, 0x08, 0x04,0x01,
							 0x01, 0x08, 0x04, 0x02, 0x02, 0x01,
							 0x01, 0x08, 0x04, 0x01, 0x04,
							 0x04, 0x01, 0x02, 0x04, 0x01, 0x02, 0x04, 0x01, 0x02, 0x04, 0x01, 0x02,
							 0x01, 0x08, 0x04,0x01,
							 0x01, 0x08, 0x04, 0x02, 0x03, 0x04,
							 0x04, 0x08, 0x04, 0x01, 0x04,
							 0x04, 0x01, 0x02, 0x04, 0x01, 0x02, 0x04, 0x01, 0x02, 0x04, 0x01, 0x02, };
double length_2[wing_2] =	
	{600, 150, 300, 1350,
	600, 150, 300, 900, 300, 300,
	600, 150, 300, 975, 300,
	183, 183, 183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
	600, 150, 300, 1350,
	600, 150, 300, 900, 300, 300,
	600, 150, 300, 975, 300,
	183, 183, 183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
	600, 300, 300, 300, 300, 300, 300,
	450, 450, 300, 450, 450, 300,
	150, 300, 450, 300, 300, 300, 2700,
	600, 150, 300, 450, 300, 300, 300,
	450, 450, 300, 450, 750,
	150, 300, 450, 300, 300, 300,
	1200, 300, 300, 300,
	450, 450, 600, 300, 300, 300,
	450, 450, 300, 300,
	450, 450, 600, 300, 300, 300,
	450, 450, 300, 300, 300,
	300, 900, 300, 300,
	300, 150, 450, 300, 300, 300,
	300, 900, 300, 300,
	150, 50, 50, 50, 50, 50, 50, 15, 15, 15, 15, 15, 600, 600,
	600, 150, 300, 1350,
	600, 150, 300, 900, 300, 300,
	600, 150, 300, 975, 300,
	183, 183, 183, 183, 183, 183, 183, 183, 183, 183, 183, 183,
	600, 150, 300, 1350,
	600, 150, 300, 900, 300, 300,
	600, 150, 300, 975, 300,
183, 183, 183, 183, 183, 183, 183, 183, 183, 183, 183, 183,};
double delay_2[wing_2] =	{1, 1, 1, 10,
	1, 1, 1, 10, 1, 1,
	1, 1, 1, 10, 10,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 10,
	1, 1, 1, 10, 1, 1,
	1, 1, 1, 10, 10,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	10, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 600,
	1,1,1,1,1,1,1,
	1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 600,
	1, 1, 1, 1, 1, 1,
	300,1, 1, 1,
	1, 1, 1, 1, 1, 1,
	1, 1, 900, 1,
	1, 1, 1, 1, 1, 1,
	1, 1, 600, 1, 600,
	1, 1, 1, 600,
	1, 1, 1, 1, 1, 600,
	1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 10,
	1, 1, 1, 10, 1, 1,
	1, 1, 1, 10, 10,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 10,
	1, 1, 1, 10, 1, 1,
	1, 1, 1, 10, 10,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,};

enum states_2{init_2, note_2, pause_2, off_2, wait_for_release_2, noMore_2} state_2;

unsigned short counter_2 = 0x00;
unsigned short i_2 = 0x00;

void shelter(){
	flag = 0x03;
	unsigned char button = ~PINA & 0x07;
	switch(state_2){
		case init_2: // wait for user to press button to begin song
		if(button == 0x04){
			PWM_on();
			LCD_DisplayString(1, "Shelter!");
			state_2 = note_2;
		}
		else {
			state_2 = init_2;
		}
		break;
		
		case note_2: // actual play time of the note
		set_PWM(freq_2[i_2]);
		if(counter_2 < length_2[i_2]){
			counter_2++;
			state_2 = note_2;
			PORTD = light_show[i_2];
			if(tmpB == 0x01){
				state_2 = noMore_2;
			}
			if(i_2 >= (wing_2)){
				state_2 = off_2;
			}
		}
		
		else{
			counter_2 = 0;
			state_2 = pause_2;
		}
		break;
		
		case pause_2: // small delay in between notes
		set_PWM(0);
		if(tmpB == 0x01){
			state_2 = noMore_2;
		}
		if(counter_2 < delay_2[i_2]){
			counter_2++;
			state_2 = pause_2;
			PORTD = 0x00;
			if(tmpB == 0x01){
				state_2 = noMore_2;
			}
			if(i_2 >= (wing_2)){
				state_2 = off_2;
			}
		}
		else {
			i_2++;
			counter_2 = 0;
			if(i_2 >= (wing_2)){
				state_2 = off_2;
			}
			state_2 = note_2;
		}
		break;
		
		case off_2: // the end of the song should visit here
		PORTD = 0x00;
		PWM_off();
		state_2 = wait_for_release_2;
		i_2 = 0;
		counter_2 = 0;
		tmpB = 0x02;
		break;
		
		case noMore_2: // when the user decides to stop the song in the middle of playing
		state_2 = wait_for_release_2;
		i_2 = 0;
		counter_2 = 0;
		break;
		
		case wait_for_release_2: // waits for release of button
		if(button == 0x01 || button == 0x02 || button == 0x04){
			state_2 = wait_for_release_2;
		}
		else {
			LCD_DisplayString(1, "welcome to jukebox pro!");
			state_2 = init_2;
			set_PWM(0);
		}
		break;
		
		default:
		state_2 = init_2;
		break;
		
	}
}

enum P_states{curr, release, real_pause, release_again} P_state;
void pause_plz(){
	unsigned char button = ~PINA & 0x0F;
	switch(P_state){
		case curr:
		//PORTD = 0x00;
			if(button == 0x08){
				dont_play = 0x01;
				P_state = release;
				PORTD = 0x04;
			}
			else {
				P_state = curr;
				dont_play = 0x00;
			}
			break;
			
		case release:
			dont_play = 0x01;
			if(button == 0x08){
				P_state = release;
			}
			else {
				P_state = real_pause;
				
			}
			break;
			
		case real_pause:
			if(button == 0x01 || button == 0x02 || button == 0x04){
				P_state = release_again;
			}
			if(button == 0x08){
				dont_play = 0x00;
				//PORTD = 0x00;
				P_state = release_again;
			}
			else {
				P_state = real_pause;
			}
			break;
			
		case release_again:
			dont_play = 0x00;
			if(button == 0x08 || button == 0x01 || button == 0x02 || button == 0x04){
				P_state = release_again;
			}
			else {
				P_state = curr;
			}
			break;
			
		default:
			P_state = curr;
			break;
	}
}

int main(void)
{
    /* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	
	unsigned short timerPeriod = 1; 
	//unsigned short ET_1 = 0;
	//LCD_DisplayString(1, "Welcome to jukebox pro 1.0!");
	TimerSet(timerPeriod);
	TimerOn();
	//LCD_ClearScreen();
	state = init;
	O_state = stop;
	P_state = curr;
	LCD_init();
	//PWM_on();
	LCD_DisplayString(1, "welcome to jukebox pro!");
    while (1) 
    {	
		offonoff();
		supermario();
		sanic();
		shelter();
		pause_plz();
		while(!TimerFlag){}
		TimerFlag = 0;
		
    }
}

