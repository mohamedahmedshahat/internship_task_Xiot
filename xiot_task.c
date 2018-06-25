/*this is the main File of my application (LED-switch) */
#include "tm4c123gh6pm.h"
#include "ADC.h"
#include "uart.h"
void switch_init(void);
void EnableInterrupts(void);  // Enable interrupts
void Xiot_function(volatile unsigned long);
void switch_init(){
SYSCTL_RCGC2_R |= 0x00000020; // (a) activate clock for port F
GPIO_PORTF_DIR_R &= ~0x10; // (c) make PF4 in (built-in button)
GPIO_PORTF_AFSEL_R &= ~0x10; // disable alt funct on PF4
GPIO_PORTF_DEN_R |= 0x10; // enable digital I/O on PF4
GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
GPIO_PORTF_AMSEL_R &= ~0x10; // disable analog functionality on PF4
GPIO_PORTF_PUR_R |= 0x10; // enable weak pull-up on PF4
GPIO_PORTF_IS_R &= ~0x10; // (d) PF4 is edge-sensitive
GPIO_PORTF_IBE_R &= ~0x10; // PF4 is not both edges
GPIO_PORTF_IEV_R &= ~0x10; // PF4 falling edge event
GPIO_PORTF_ICR_R = 0x10; // (e) clear flag4
GPIO_PORTF_IM_R |= 0x10; // (f) arm interrupt on PF4
NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
NVIC_EN0_R = 0x40000000; // (h) enable interrupt 30 in NVIC
EnableInterrupts(); // (i) Enable global Interrupt flag (I)


 }
void LED_init(void){
SYSCTL_RCGC2_R |= 0x00000002; // (a) activate clock for port B
GPIO_PORTB_DIR_R &= ~0x10; // (c) make PB4 in (built-in button)
GPIO_PORTB_AFSEL_R &= ~0x10; // disable alt funct on PB4
GPIO_PORTB_DEN_R |= 0x10; // enable digital I/O on PB4
GPIO_PORTB_PCTL_R &= ~0x000F0000; // configure PFB as GPIO
GPIO_PORTB_AMSEL_R &= ~0x10; // disable analog functionality on PB4
GPIO_PORTB_DATA_R &= ~0x10;//default state for LED is Low

}
void SysTick_Init(void){ // priority 6
NVIC_ST_CTRL_R = 0; // disable SysTick during setup
NVIC_ST_RELOAD_R = -1;// reload value
NVIC_ST_CURRENT_R = 0; // any write to current clears it
NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0xc0000000;
NVIC_ST_CTRL_R = 0x07; // enable SysTick with core clock and interrupts

}


float temp_data;//global variable to store the decimal value of analog pin from 0 to 4095
unsigned long temp_degree; //variable to be assigned with real temperature value
volatile unsigned long switch_state=0;// store the status of the switch and it's volatile cause it's value my be changed indirectly
unsigned long pre_state=0;//store the pre_status of the switch

int main(void){
	//initialization 
	pre_state=switch_state;
	switch_init();
	LED_init();
	ADC0_Init();
	UART_Init();
	
	



while(1){
	
Xiot_function(switch_state);//call Xiot_function multiple times
temp_degree=(temp_data/3.3)*100;//conversion equation of temp sensor with assumption 		
UART_OutString((unsigned char*)"temperature is");

}
return 0;
}
/*
input:switch status
output:LED(low,High) and serialize it's state to PC
*/
void Xiot_function(volatile unsigned long current_input_status){
if(current_input_status %2==0 && pre_state!=current_input_status){
	GPIO_PORTB_DATA_R &= ~0x10; //LED is low
  UART_OutString((unsigned char*)"Switch pressed");//send switch state
  UART_OutString((unsigned char*)"LED OFF");//send LED status
}
else if(current_input_status %2!=0 && pre_state!=current_input_status){
	     GPIO_PORTB_DATA_R |=0x10; //LED is High
       UART_OutString((unsigned char*)"Switch pressed");//send switch state
       UART_OutString((unsigned char*)"LED ON");//send LED status
}
pre_state=current_input_status;//store pre_status of switch to find out whether switch is pressed or not

}
void GPIOPortF_Handler(void){
	switch_state+=1%10;//count number of pressed switch
}

void SysTick_Handler(void){
	
	temp_data=(ADC0_In()/4096)*3.3;// this is analog voltage at 25 C
	
}




