#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

const int PWM_pin = 24;   /* GPIO 1 as per WiringPi, GPIO18 as per BCM */

int main(int argc, const char * argv[])
{
	int ciclo = atoi(argv[1]);
	int freq = atoi(argv[2]);	
	int divisor = 19200000/(1024*freq);    
	printf("Divisor: %d\n", divisor);

	if (wiringPiSetup () == -1)
		exit (1) ;

	pinMode(PWM_pin, PWM_OUTPUT); /* set PWM pin as output */
	pwmSetMode(PWM_MODE_MS);
	pwmSetClock(divisor);
	pwmSetRange(1024);
	pwmWrite(PWM_pin, ciclo);	/* provide PWM value for duty cycle */

	return 0;
}
