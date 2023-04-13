#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>

const int PWM_pin = 26;   /* GPIO 1 as per WiringPi, GPIO18 as per BCM */

int main (void)
{
  int intensity = 0 ;            

  if (wiringPiSetup () == -1)
    exit (1) ;

  pinMode (PWM_pin, PWM_OUTPUT) ; /* set PWM pin as output */

  while(1)
  {
	
    
    pwmWrite (PWM_pin, intensity) ;	/* provide PWM value for duty cycle */
    
  }
}
