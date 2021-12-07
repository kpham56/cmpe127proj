/*header files for TM4C123 device*/
#include "TM4C123GH6PM.h"
#include <stdio.h>


#define two_second 1000000 // two second delay from micro second delay function 
#define PA2 0x04 // left wheel
#define PA3 0x08 // right wheel
#define PA23 0xC0 // both wheels
#define PA4 0x10 // servo
/*Function prototype for microsecond delay function and servo rotation*/
void Delay_MicroSecond(int time); // generates delay in microseconds
void Servo_0_Degree(int pin);  // 3% duty cycle of 50Hz pulse
void Servo_90_Degree(int pin); // 7% duty cycle of 50Hz pulse
void Servo_180_Degree(int pin);// 12% duty cycle of 50Hz pulse
void servoInit(int pin); // initialize servo
void PWM_init(void); // initialize PWM
void turn(int pin); // spin a wheel
void move(int pin1, int pin2); //move both wheels

/* main code to control servo motor angular movement */
int main(void)
{
	servoInit(PA4);
	PWM_init();
	  while(1)
	{
			move(PA2,PA3);
      Servo_0_Degree(PA4);
      Delay_MicroSecond(two_second);
      Servo_90_Degree(PA4);
      Delay_MicroSecond(two_second);
      Servo_180_Degree(PA4);
       Delay_MicroSecond(two_second); 
	}
	
}

void move(int pin1, int pin2)
{
		SYSCTL->RCGCGPIO |= 0x01;   /* enable clock to PORTF */
	   GPIOA->DIR |= ((pin1) | (pin2));         /* pin digital */
     GPIOA->DEN |= ((pin1) | (pin2));            /* pin digital */
     GPIOA->DATA |=((pin1) | (pin2));
		 
		 Delay_MicroSecond(two_second);
		 GPIOA->DATA &= ~((pin1) | (pin2));
}

void turn(int pin)
{
	SYSCTL->RCGCGPIO |= 0x01;   /* enable clock to PORTF */
	   GPIOA->DIR |= (pin);         /* pin digital */
     GPIOA->DEN |= (pin);            /* pin digital */
     GPIOA->DATA |=(pin);
		 
		 Delay_MicroSecond(two_second);
		 GPIOA->DATA &= ~(pin);
}

void servoInit(int pin)
{
	 /* pin as a digital output signal to provide trigger signal */
	  SYSCTL->RCGCGPIO |= 1;      /* enable clock to PORTA */
	  GPIOA->DIR |=(pin);         /* set pin as a digial output pin */
	  GPIOA->DEN |=(pin);         /* make pin as digital */
}
/* This function generate a 3% duty cycle from 20ms PWM signal or 50Hz*/

void PWM_init(void)
{
   
     /* Clock setting for PWM and GPIO PORT */
    SYSCTL->RCGCPWM |= 2;       /* Enable clock to PWM1 module */
    SYSCTL->RCGCGPIO |= 0x20;  /* Enable system clock to PORTF */
	  SYSCTL->RCC |= (1<<20);    /* Enable System Clock Divisor function  */
    SYSCTL->RCC |= 0x000E0000; /* Use pre-divider valur of 64 and after that feed clock to PWM1 module*/

 /* Setting of PF2 pin for M1PWM6 channel output pin */
   	GPIOF->AFSEL |= (1<<2);          /* PF2 sets a alternate function */
    GPIOF->PCTL &= ~0x00000F00; /*set PF2 as output pin */
    GPIOF->PCTL |= 0x00000500; /* make PF2 PWM output pin */
    GPIOF->DEN |= (1<<2);          /* set PF2 as a digital pin */
    
    PWM1->_3_CTL &= ~(1<<0);   /* Disable Generator 3 counter */
	  PWM1->_3_CTL &= ~(1<<1);   /* select down count mode of counter 3*/
    PWM1->_3_GENA = 0x0000008C;  /* Set PWM output when counter reloaded and clear when matches PWMCMPA */
    PWM1->_3_LOAD = 5000;     /* set load value for 50Hz 16MHz/64 = 250kHz and (250KHz/5000) */
    PWM1->_3_CMPA = 4999;     /* set duty cyle to to minumum value*/
    PWM1->_3_CTL = 1;           /* Enable Generator 3 counter */
    PWM1->ENABLE = 0x40;      /* Enable PWM1 channel 6 output */
}
/* This function generates delay in ms */
/* calculations are based on 16MHz system clock frequency */

void Servo_0_Degree(int pin)
{
   int i=0;  	 
	for(i=0; i<50; i++) 
	{
	  /* Given 10us trigger pulse */
	  GPIOA->DATA |= (pin); /* make control  pin high */
	  Delay_MicroSecond(600); /*0.6ms seconds delay */
	  GPIOA->DATA &= ~(pin); /* make control  pin low */
	  Delay_MicroSecond(19400); /*1.94ms seconds delay */
  }
}
/* This function generate a 7% duty cycle from 20ms PWM signal or 50Hz*/
void Servo_90_Degree(int pin)
{
   int i=0; 
	 for(i=0; i<50; i++) 
    {	
	/* Given 10us trigger pulse */
	  GPIOA->DATA |= (pin); /* make control  pin high */
	  Delay_MicroSecond(1400); /*1.4ms seconds delay */
	  GPIOA->DATA &= ~(pin); /* make control  pin low */
	  Delay_MicroSecond(18600); /*1.86ms seconds delay */
		}
}
/* This function generate a 12% duty cycle from 20ms PWM signal or 50Hz*/
void Servo_180_Degree(int pin)
{
   	int i=0; 
	  for(i=0; i<50; i++) 
    {
	  /* Given 10us trigger pulse */
	  GPIOA->DATA |= (pin); /* make control  pin high */
	  Delay_MicroSecond(2400); /*2.4ms seconds delay */
	  GPIOA->DATA &= ~(pin); /* make trigger  pin high */
	  Delay_MicroSecond(17600); /*1.76ms seconds delay */
		}
}


/* Create one microsecond second delay using Timer block 1 and sub timer A */

void Delay_MicroSecond(int time)
{
    int i;
    SYSCTL->RCGCTIMER |= 2;     /* enable clock to Timer Block 1 */
    TIMER1->CTL = 0;            /* disable Timer before initialization */
    TIMER1->CFG = 0x04;         /* 16-bit option */ 
    TIMER1->TAMR = 0x02;        /* periodic mode and down-counter */
    TIMER1->TAILR = 16 - 1;  /* TimerA interval load value reg */
    TIMER1->ICR = 0x1;          /* clear the TimerA timeout flag */
    TIMER1->CTL |= 0x01;        /* enable Timer A after initialization */

    for(i = 0; i < time; i++)
    {
        while ((TIMER1->RIS & 0x1) == 0) ;      /* wait for TimerA timeout flag */
        TIMER1->ICR = 0x1;      /* clear the TimerA timeout flag */
    }
}

/* This function is called by the startup assembly code to perform system specific initialization tasks. */
void SystemInit(void)
{
    __disable_irq();    /* disable all IRQs */
    
    /* Grant coprocessor access */
    /* This is required since TM4C123G has a floating point coprocessor */
    SCB->CPACR |= 0x00F00000;
}
