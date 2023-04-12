
#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "vl53l1x_api.h"
#include "PLL.h"
#include "SysTick.h"
#include "uart.h"
#include <stdbool.h>
#include "onboardLEDs.h"




#define I2C_MCS_ACK             0x00000008  // Data Acknowledge Enable
#define I2C_MCS_DATACK          0x00000008  // Acknowledge Data
#define I2C_MCS_ADRACK          0x00000004  // Acknowledge Address
#define I2C_MCS_STOP            0x00000004  // Generate STOP
#define I2C_MCS_START           0x00000002  // Generate START
#define I2C_MCS_ERROR           0x00000002  // Error
#define I2C_MCS_RUN             0x00000001  // I2C Master Enable
#define I2C_MCS_BUSY            0x00000001  // I2C Busy
#define I2C_MCR_MFE             0x00000010  // I2C Master Function Enable

#define MAXRETRIES              5           // number of receive attempts before giving up
void I2C_Init(void){
  SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;           													// activate I2C0
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;          												// activate port B
  while((SYSCTL_PRGPIO_R&0x0002) == 0){};																		// ready?

    GPIO_PORTB_AFSEL_R |= 0x0C;           																	// 3) enable alt funct on PB2,3       0b00001100
    GPIO_PORTB_ODR_R |= 0x08;             																	// 4) enable open drain on PB3 only

    GPIO_PORTB_DEN_R |= 0x0C;             																	// 5) enable digital I/O on PB2,3
//    GPIO_PORTB_AMSEL_R &= ~0x0C;          																// 7) disable analog functionality on PB2,3

                                                                            // 6) configure PB2,3 as I2C
//  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00003300;
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00002200;    //TED
    I2C0_MCR_R = I2C_MCR_MFE;                      													// 9) master function enable
    I2C0_MTPR_R = 0b0000000000000101000000000111011;                       	// 8) configure for 100 kbps clock (added 8 clocks of glitch suppression ~50ns)
//    I2C0_MTPR_R = 0x3B;                                        						// 8) configure for 100 kbps clock
        
}

void PortN_Init(void){
SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12; //activate the clock for Port N
while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0){};//allow time for clock to stabilize
GPIO_PORTN_DIR_R=0b00000000; //Make PN1 outputs, to turn on LED's
GPIO_PORTN_DEN_R=0b00000000; //Enable PN1
return;
}

//The VL53L1X needs to be reset using XSHUT.  We will use PG0
void PortG_Init(void){
    //Use PortG0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R6;                // activate clock for Port N
    while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R6) == 0){};    // allow time for clock to stabilize
    GPIO_PORTG_DIR_R &= 0x00;                                        // make PG0 in (HiZ)
  GPIO_PORTG_AFSEL_R &= ~0x01;                                     // disable alt funct on PG0
  GPIO_PORTG_DEN_R |= 0x01;                                        // enable digital I/O on PG0
                                                                                                    // configure PG0 as GPIO
  //GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFFFF00)+0x00000000;
  GPIO_PORTG_AMSEL_R &= ~0x01;                                     // disable analog functionality on PN0

    return;
}
void PortH_Init(void){
  //Use PortM pins for output
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R7;                // activate clock for Port H
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R7) == 0){};    // allow time for clock to stabilize
  GPIO_PORTH_DIR_R |= 0xFF;                                        // make PH0 out (PN0 built-in LED1)
  GPIO_PORTH_AFSEL_R &= ~0xFF;                                     // disable alt funct on PH0
  GPIO_PORTH_DEN_R |= 0xFF;                                        // enable digital I/O on PH0
                                                                                                    // configure PN1 as GPIO
  
  GPIO_PORTH_AMSEL_R &= ~0xFF;                                     // disable analog functionality on PH0        
  return;
}

// External LED
void PortL_Init(void) {
		SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R10;                 // activate the clock for Port L

	
	
		while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R10) == 0);
	
		GPIO_PORTL_DIR_R = 0b00010000;
		GPIO_PORTL_DEN_R = 0b00010000;

	return;
	
	
}

void PortM_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11; 
		while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R11) == 0);
	
		GPIO_PORTM_DIR_R = 0b00000001;
		GPIO_PORTM_DEN_R = 0b00000001;
	return;
}

void PortE_Init(void){	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;		              // activate the clock for Port E
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R4) == 0){};	        // allow time for clock to stabilize
  
	GPIO_PORTE_DIR_R = 0b00000001;
	GPIO_PORTE_DEN_R = 0b00000001;                        		// Enabled both as digital outputs
	return;
	}
	
	void PortF_Init(void){	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;		              // activate the clock for Port E
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R5) == 0){};	        // allow time for clock to stabilize
  
	GPIO_PORTF_DIR_R = 0b00010010;
	GPIO_PORTF_DEN_R = 0b00010010;                        		// Enabled both as digital outputs
	return;
	}

// Enable interrupts
void EnableInt(void)
{    __asm("    cpsie   i\n");
}

// Disable interrupts
void DisableInt(void)
{    __asm("    cpsid   i\n");
}

// Low power wait
void WaitForInt(void)
{    __asm("    wfi\n");
}

// global variable visible in Watch window of debugger
// increments at least once per button press
volatile unsigned long FallingEdges = 0;

// give clock to Port J and initalize as input GPIO

void PortJ_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R8;				// activate clock for Port J
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R8) == 0){};	// allow time for clock to stabilize
  GPIO_PORTJ_DIR_R &= ~0x02;    // (c) make PJ1 in 

  GPIO_PORTJ_DEN_R |= 0x02;     //     enable digital I/O on PJ1
	GPIO_PORTJ_PCTL_R &= ~0x000000F0; //  configure PJ1 as GPIO 
	GPIO_PORTJ_AMSEL_R &= ~0x02;	//   disable analog functionality on PJ1		
	GPIO_PORTJ_PUR_R |= 0x02;			//	enable weak pull up resistor
  GPIO_PORTJ_IS_R &= ~0x02;     // (d) PJ1 is edge-sensitive 
  GPIO_PORTJ_IBE_R &= ~0x02;    //     PJ1 is not both edges 
  GPIO_PORTJ_IEV_R &= ~0x02;    //     PJ1 falling edge event 
  GPIO_PORTJ_ICR_R = 0x02;      // (e) clear flag1
  GPIO_PORTJ_IM_R |= 0x02;      // (f) arm interrupt on PJ1
//  NVIC_PRI13_R = (NVIC_PRI13_R&0xFF00FFFF)|0x000A0000; // (g) priority 5
  NVIC_PRI12_R = (NVIC_PRI12_R&0xFF00FFFF)|0xA0000000; // (g) priority 5
  NVIC_EN1_R |= 0x00080000;              // (h) enable interrupt 67 in NVIC
  EnableInt();           				
}


// Interrupt initialization for GPIO Port J IRQ# 51
void PortJ_Interrupt_Init(void){
	
		FallingEdges = 0;       // initialize counter

	
		GPIO_PORTJ_IS_R = 0;    	// (Step 1) PJ1 is edge-sensitive 
		GPIO_PORTJ_IBE_R = 0;   	//     			PJ1 is not both edges 
		GPIO_PORTJ_IEV_R = 0;    	//     			PJ1 falling edge event 
		GPIO_PORTJ_ICR_R = 0x02;     // 					clear interrupt flag by setting proper bit in ICR register
		GPIO_PORTJ_IM_R = 0x02;    	// 					arm interrupt on PJ1 by setting proper bit in IM register
    
		NVIC_EN1_R = 0x00080000;           // (Step 2) enable interrupt 51 in NVIC
	
		NVIC_PRI12_R = 0xA0000000; 				// (Step 4) set interrupt priority 5

		EnableInt();           							// (Step 3) Enable Global Interrupt. lets go!
}





void GPIOJ_IRQHandler(void){
  FallingEdges = FallingEdges + 1;	// Increase the global counter variable ;Observe in Debug Watch Window
	GPIO_PORTJ_ICR_R = 0x02;     					// acknowledge flag by setting proper bit in ICR register
}



void VL53L1X_XSHUT(void){
    GPIO_PORTG_DIR_R |= 0x01;                                        // make PG0 out
    GPIO_PORTG_DATA_R &= 0b11111110;                                 //PG0 = 0
    SysTick_Wait10ms(10);
    GPIO_PORTG_DIR_R &= ~0x01;                                            // make PG0 input (HiZ)
    
}
void CW(){
    for(int i=0; i<32; i++){
        GPIO_PORTH_DATA_R = 0b00001100;
        SysTick_Wait10ms(4);
        GPIO_PORTH_DATA_R = 0b00000110;
        SysTick_Wait10ms(4);
        GPIO_PORTH_DATA_R = 0b00000011;
        SysTick_Wait10ms(4);
        GPIO_PORTH_DATA_R = 0b00001001;
        SysTick_Wait10ms(4);
			
    }
	}
void CCW(){
    for(int i=0; i<512; i++){
         GPIO_PORTH_DATA_R = 0b00001001;
        SysTick_Wait10ms(5);
        GPIO_PORTH_DATA_R = 0b00000011;
        SysTick_Wait10ms(5);
        GPIO_PORTH_DATA_R = 0b00000110;
        SysTick_Wait10ms(5);
        GPIO_PORTH_DATA_R = 0b00001100;
        SysTick_Wait10ms(5);
				
    }
	}

	
	void LED_State(int state) 
	{	
		if (state == 1) //Red
			{
			  GPIO_PORTL_DATA_R = 0b00010000;
				GPIO_PORTM_DATA_R = 0b00000000;
				GPIO_PORTE_DATA_R = 0b00000000;
				GPIO_PORTF_DATA_R = 0b00000000;
		  } 
			
			else if (state == 0) //Green
			{
			  GPIO_PORTM_DATA_R = 0b00000001;
				GPIO_PORTL_DATA_R = 0b00000000;
				GPIO_PORTE_DATA_R = 0b00000000;
				GPIO_PORTF_DATA_R = 0b00000000;
		  }
			
			else if (state == 2) //White
			{
				GPIO_PORTE_DATA_R = 0b00000001;
			  GPIO_PORTM_DATA_R = 0b00000000;
				GPIO_PORTL_DATA_R = 0b00000000;
				GPIO_PORTF_DATA_R = 0b00000000;
		  }
			
			else if (state == 3) // Orange
			{
				GPIO_PORTF_DATA_R = 0b00000010;
				GPIO_PORTE_DATA_R = 0b00000000;
			  GPIO_PORTM_DATA_R = 0b00000000;
				GPIO_PORTL_DATA_R = 0b00000000;
		  }
			
}

//*********************************************************************************************************
//*********************************************************************************************************
//***********					!!MAIN CODE!!				*****************************************************************
//*********************************************************************************************************
//*********************************************************************************************************
uint16_t	dev = 0x29;			//address of the ToF sensor as an I2C slave peripheral
int status=0;

int main(void) {
  uint8_t byteData, sensorState=0, myByteArray[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} , i=0;
  uint16_t wordData;
  uint16_t Distance;
  uint16_t SignalRate;
  uint16_t AmbientRate;
  uint16_t SpadNum; 
  uint8_t RangeStatus;
  uint8_t dataReady;

	//initialize
	PLL_Init();	
	SysTick_Init();
			PortL_Init();
		PortH_Init();
		PortM_Init();
	PortF_Init();
	PortE_Init();
	I2C_Init();
	UART_Init();
	PortN_Init();					// Initialize the onboard LED on port N
	PortJ_Init();											// Initialize the onboard push button on PJ1
	PortJ_Interrupt_Init();	
	
	bool running = false;
	GPIO_PORTH_DATA_R = 0b00000000;
	LED_State(3);
	
	UART_printf("Welcome Abdurahman Bade\r\n");
	int mynumber = 1;
	sprintf(printf_buffer,"Deliverable 2 Project (2DX3) %d\r\n", mynumber);
	sprintf(printf_buffer,"Let's take a look at your Model ID and Model Type %d\r\n", mynumber);
	UART_printf(printf_buffer);

  status = VL53L1_RdByte(dev, 0x010F, &byteData);					// This is the model ID.  Expected returned value is 0xEA
  myByteArray[i++] = byteData;

  status = VL53L1_RdByte(dev, 0x0110, &byteData);					// This is the module type.  Expected returned value is 0xCC
  myByteArray[i++] = byteData;
	
	status = VL53L1_RdWord(dev, 0x010F, &wordData);
	status = VL53L1X_GetSensorId(dev, &wordData);

	sprintf(printf_buffer,"Model_ID=0x%x , Module_Type=0x%x\r\n",myByteArray[0],myByteArray[1]);
	UART_printf(printf_buffer);

	// Booting ToF chip
	while(sensorState==0){
		status = VL53L1X_BootState(dev, &sensorState);
		SysTick_Wait10ms(1);
  }
	FlashAllLEDs();
	UART_printf("Booting ToF Sensor...\r\n");
	UART_printf(printf_buffer);
	UART_printf("ToF Sensor Booted Successfully!\r\n");
 	UART_printf("Please wait...\r\n");
	
	status = VL53L1X_ClearInterrupt(dev); 
	
  status = VL53L1X_SensorInit(dev);
	Status_Check("SensorInit", status);
	

  status = VL53L1X_StartRanging(dev);   
	Status_Check("StartRanging", status);
	
	
	UART_printf("Ready to Commence!! Press the onboard pushbutton (PJ1) to start!\r\n");
	uint16_t startStop = 1, prevStartStop = 1;
	
	
	while(1){
		LED_State(3);
		//wait for interupt from button to start
		startStop = GPIO_PORTJ_DATA_R & 0x02;
		
						if ((!prevStartStop && startStop) && !running) 
				{
						UART_printf("Button pressed. Commencing rotation.....\r\n");
						running = true;
						LED_State(0);
				} 
				
				else if ((!prevStartStop && startStop) && running) 
				{
						UART_printf("Button pressed. Halting rotation.....\r\n");
						running = false;
						LED_State(1);
				}
		
		WaitForInt();
		// Booting ToF chip
		if (!running){
			LED_State(1);
		}
		else if(running){
			
			status = VL53L1X_BootState(dev, &sensorState);
			SysTick_Wait10ms(10);
		}

		status = VL53L1X_ClearInterrupt(dev); 
		
		
		status = VL53L1X_SensorInit(dev);
		Status_Check("SensorInit", status);

		


		status = VL53L1X_StartRanging(dev);   


		
		//16 scans
		for(int i = 0; i < 16; i++) {
			//wait until the ToF sensor's data is ready
			while (dataReady == 0){
						status = VL53L1X_CheckForDataReady(dev, &dataReady);
						VL53L1_WaitMs(dev, 5);
			}
			
			dataReady = 0;
			
			
			status = VL53L1X_GetRangeStatus(dev, &RangeStatus);
			status = VL53L1X_GetDistance(dev, &Distance);					
			status = VL53L1X_GetSignalRate(dev, &SignalRate);
			status = VL53L1X_GetAmbientRate(dev, &AmbientRate);
			status = VL53L1X_GetSpadNb(dev, &SpadNum);
			

			status = VL53L1X_ClearInterrupt(dev); 

			
			// Print to UART
			sprintf(printf_buffer,"%u\r\n", Distance);
			UART_printf(printf_buffer);
			SysTick_Wait10ms(30);
			LED_State(0);
			CW();
			FlashLED3(20);
			SysTick_Wait10ms(150);
		}
		
		LED_State(2);
		CCW();
		LED_State(2);
		
		VL53L1X_StopRanging(dev);
	}
}

