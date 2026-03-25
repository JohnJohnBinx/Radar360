#include "TM4C123.h"                    // Device header
#include <stdbool.h>
#include <stdio.h>
#include "driverlib/sysctl.h"

// HC-SR04 DEFINITIONS
#define ECHO (1U<<6)			//PB6
#define TRIGGER (1U<<4)		//PA4
#define BLUE (1U<<2)			//PF2, Blue LED

//Timer Variables & Prototypes
uint32_t HighEdge, LowEdge;
const double _16MHz_1clock = 62.5e-9;
const uint32_t MULTIPLIER = 5882;
void Timer0_init(void);
void delay_Microsecond(uint32_t time);

//Initializations
void Ports_Init(void);
uint32_t getClock;

//HC-SR04 Variables
uint32_t measureD(void);
uint32_t ddistance;					//Distance In Centimetres

//PWM Protypes & Variables
void PWM_Init(void);
void DelayFunc(int j);
bool Bounce = 0;
bool Stage1 = 1;
bool Stage2 = 0;
bool Stage3 = 0;

//UART Prototypes
void UART_Init(void);
void UART_Transmit(uint8_t Data1, uint8_t Data2, uint8_t Data3);

//Converting Angle From PWM
int ConvertAngle(int Cycle1, int Cycle2);
int Angle1;
int Angle2;


//Ports & Pins Used
//GPIOA: PA4, PA1
//GPIOB: PB6
//GPIOC: N/A
//GPIOD: N/A
//GPIOE: PE4,PE5
//GPIOF: PF2

//Designations
//PA4 HR04 Trigger
//PA1 UART Output
//PB6 HR04 ECHO
//PE4 PWM1 Signal
//PE5 PWM2 Signal
//PF2 Blue Onboard LED

int main(void){
	
	getClock = SysCtlClockGet();		//Get System Clock
	
	DelayFunc(50);						//Delay Function
	PWM_Init();
	Ports_Init();
	Timer0_init();
	UART_Init();
	
	//250 Cycles = 90 Degree, 2.77 Cycles == 1 Degree
	//4625 Middle Position
	//Has To Go Between 4875 and 4375
	
	DelayFunc(80);
while(1){
	
	while (Bounce == 0){
		while(Stage1){
			DelayFunc(12000);
			PWM0 ->_2_CMPA -= 2.77;
			ConvertAngle(PWM0 ->_2_CMPA, PWM0 ->_2_CMPB);
			UART_Transmit(Angle1, Angle2, measureD());
			if(PWM0 ->_2_CMPA < 4625){
				Stage1 = 0;
				Stage2 = 1;
			}
		}
		while(Stage2){
			DelayFunc(12000);
			PWM0 ->_2_CMPB -= 2.77;
			ConvertAngle(PWM0 ->_2_CMPA, PWM0 ->_2_CMPB);
			UART_Transmit(Angle1, Angle2, measureD());
			if(PWM0 ->_2_CMPB < 4375){
				Stage2 = 0;
				Stage3 = 1;
			}
		}
		while(Stage3){
			DelayFunc(12000);
			PWM0 ->_2_CMPA -= 2.77;
			ConvertAngle(PWM0 ->_2_CMPA, PWM0 ->_2_CMPB);
			UART_Transmit(Angle1, Angle2, measureD());
			if(PWM0 ->_2_CMPA < 4375){
				Stage3 = 0;
				Stage1 = 1;
				Bounce = 1;
		}
		}
	}
	
	while(Bounce){
		while(Stage1){
			DelayFunc(10000);
			PWM0 ->_2_CMPA += 2.77;
			ConvertAngle(PWM0 ->_2_CMPA, PWM0 ->_2_CMPB);
			UART_Transmit(Angle1, Angle2, measureD());
			if(PWM0 ->_2_CMPA > 4625){
				Stage1 = 0;
				Stage2 = 1;
			}
		}
		while(Stage2){
			DelayFunc(10000);
			PWM0 ->_2_CMPB += 2.77;
			ConvertAngle(PWM0 ->_2_CMPA, PWM0 ->_2_CMPB);
			UART_Transmit(Angle1, Angle2, measureD());
			if(PWM0 ->_2_CMPB > 4875){
				Stage2 = 0;
				Stage3 = 1;
			}
		}
		while(Stage3){
			DelayFunc(10000);
			PWM0 ->_2_CMPA += 2.77;
			ConvertAngle(PWM0 ->_2_CMPA, PWM0 ->_2_CMPB);
			UART_Transmit(Angle1, Angle2, measureD());
			if(PWM0 ->_2_CMPA > 4875){
				Stage3 = 0;
				Stage1 = 1;
				Bounce = 0;
		}
	}
}
}
}
	

void DelayFunc(int j){
	int i = 0;
	while(i<j){i = i+1;}
}

void PWM_Init(void){
	
	SYSCTL -> RCGC0 |= 0x100000;		//Enable PWM Clock Gating
	SYSCTL -> RCGC2 |= 0x10;				//Enable PortE Clock Gating
	GPIOE -> AFSEL |= 0x30;					//Enable Pin PE4 And PE5 As Alternate Function
	GPIOE -> PCTL |= 0x440000;			//Enable PE4 As M0PWM4 Mode and PE5 As M0PWM5 Mode
	GPIOE -> DEN |= 0x30;						//Digitally Enable PE4 And PE5
	
	
	SYSCTL -> RCC |= (1<<20);				//Enable PWMDIV, By Default PMDIV is /64
	
	DelayFunc(50);
	PWM0 -> _2_CTL &=~ 0x01;							//Disable PWM0 Generator
	PWM0 ->_2_GENA |= 0x08C;							//High At Start, Low When LOAD = CMPA
	PWM0 ->_2_GENB |= 0x80C;							//High At Start, Low When LOAD = CMPB
	
	//PWM Signal Calculation - Formula Found On TIVAC Datasheet
	//16MHz -> 1/16e6 = 62.5e-9s
	//50Hz -> 1/50 = 0.02s
	//Number Of Cycles Per Time Period = 0.02/62.5e-9 = 320000 Cycles
	//Accounting For Predivider: 320000/64 = 5000 Cycles
	
	PWM0 ->_2_LOAD = 5000; 							//Generate A 50Hz Signal
	PWM0 ->_2_CMPA = (4875); 						//Generate A 7.5% Duty Cycle 
	PWM0 ->_2_CMPB = (4875); 						//Generate A 7.5% Duty Cycle 
	
	PWM0 -> _2_CTL |= 0x01;							//Enable PWM0 Generator
	PWM0 -> ENABLE |= 0x30;							//Enable PWM0 Module
}


void Timer0_init(void){

	SYSCTL -> RCGCTIMER |= 0x01;
	SYSCTL -> RCGCGPIO |= 0x02;
	GPIOB -> DIR &=~ ECHO;
	GPIOB -> DEN |=ECHO;
	GPIOB -> AFSEL |=ECHO;
	GPIOB -> PCTL &=~ 0x0F000000;
	GPIOB -> PCTL |=	0x07000000;
	
	TIMER0 -> CTL &=~ 1;		//Disable CTL For Timer Setup
	TIMER0 -> CFG = 4;			//Enable Timer As 16Bit
	TIMER0 -> TAMR = 0x17;	//Edge Time Mode Up Counter
	TIMER0 -> CTL |= 0x0C; 	//Set TAEVENT Bit to both Edges
	TIMER0 -> CTL |= 1;			//Enable Timer
	
	
	SYSCTL -> RCGCTIMER |= 0x02;
	TIMER1 -> CTL=0;							//Disable Timer For Configuration
	TIMER1 -> CFG = 0x04;					//Enable Timer As 16Bit
	TIMER1 -> TAMR = 0x02; 				//Periodic Mode Timer
	TIMER1 -> TAILR = 16-1; 			//One Micro-second (16 cycles)
	TIMER1 -> ICR = 0x1;
	TIMER1 -> CTL |= 0x01;
	
}


void delay_Microsecond(uint32_t time){
	
	uint32_t i;
	for(i=0; i<time;i++){
		while((TIMER1->RIS&0x1)==0){
			TIMER1->ICR =0x1;
		}
	}
}

void Ports_Init(void){
	
	SYSCTL -> RCGCGPIO |= 0x01;		//Enable Clock For Port A
	SYSCTL -> RCGCGPIO |= 0x20;		//Enable Clock For Port F
	GPIOA -> DIR |= TRIGGER;
	GPIOF -> DIR |= BLUE;
	GPIOA -> DEN |= TRIGGER;
	GPIOF -> DEN |= BLUE;
}


uint32_t measureD(void){
	
	GPIOA ->DATA &=~ TRIGGER; //Disable Trigger Pin
	delay_Microsecond(12);		//Wait Ten Micro-seconds
	GPIOA ->DATA |= TRIGGER; //Enable Trigger Pin
	delay_Microsecond(12);		//Wait Ten Micro-seconds
	GPIOA ->DATA &=~ TRIGGER; //Disable Trigger Pin
	
	TIMER0->ICR = 4;									//Clear Timer Capture Flag
	while((TIMER0->RIS&0x04)==0){};		//Wait Until Capture Occurs
	HighEdge = TIMER0 -> TAR;					//HighEdge Is Time At Which Last Edge Event Happened
	
	TIMER0 -> ICR = 4;
	while((TIMER0->RIS&0x04)==0){};
	LowEdge = TIMER0 -> TAR; 					//LowEdge Is Time At Which Current Edge Event Happened
	ddistance = LowEdge - HighEdge;
	ddistance = _16MHz_1clock * (double) MULTIPLIER * (double) ddistance;
		
	return ddistance;
	}

	
void UART_Init(void){
	SYSCTL -> RCGCUART |= 0x01;		//Enable UART0 In Run Mode
	SYSCTL -> RCGCGPIO |= 0x01;		//Enable PORTA Clock Access
	GPIOA -> DIR |=	0x02;					//Enable PA1 As Output
	GPIOA -> DEN |= 0x02;					//Digitally Enable PA1
	GPIOA -> AFSEL	|= 0x02;			//PA1	In Alternate Function Mode (For Transmitting)
	//GPIOA -> PCTL &=~ 0xFF;			//Clear All Default PCTL Registers
	GPIOA -> PCTL |= 0x02;				//PA1 Port Control Only
	
	UART0 -> CTL &=~ 0x01;				//Disable UART0 For Configuration
	UART0 -> IBRD = 0x08;					//Buad Rate Integer As 8
	UART0 -> FBRD = 0x2C;					//Buad Rate Fractional Integer As 44
	UART0 -> LCRH = 0x60;					//8 Bits WLEN Transmission
	UART0 -> LCRH |= 0x10;				//Enable FIFO
	UART0 -> CC = 0x00;						//Ensure UART Is Using System Clock
	UART0 -> CTL |= 0x01;					//Re-enable UART After Configuration
	
	//BAUD RATE CALCULATION - FORMULA FOUND OF TIVAC DATASHEET
	//115200 = 16000000/(16*BaudRate)
	//BaudRate = 16e6/(115200*16) = 8.680556
	//FBRD = integer(0.680556*64+0.5) = 44.0556 == 44
}


void UART_Transmit(uint8_t Data1, uint8_t Data2, uint8_t Data3){
	while((UART0-> FR & (1<<5)) != 0);
	UART0 -> DR = (char)Data1;		//Data To Be Transmited
	UART0 -> DR = (char)Data2;		//Data To Be Transmited
	UART0 -> DR = (char)Data3;		//Data To Be Transmited
	
}

int ConvertAngle(int Cycle1, int Cycle2){
	Angle1 = (((Cycle1-4625)/2.77)+90);		//Convert Angle From Cycles To Degrees
	Angle2 = (((Cycle2-4625)/2.77)+90); 	
	
	if (Angle1 < 0)												//Prevent Algorithm From Transmitting UART values Greater than 255 Due To Decimal Changes
		Angle1 = 0;
	else if(Angle2 < 0)
		Angle2 = 0;
	return Angle1;
	return Angle2;
}
