#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void)
{
  printf("Raspberry Pi wiringPi test program\n") ;

  if(wiringPiSetupGpio() == -1)
    exit(1) ;

  pinMode(12, PWM_OUTPUT);
  pwmSetMode(PWM_MODE_MS);
  pwmSetClock(200);
  pwmSetRange(100);
  for(;;)
  {
    pwmWrite(12, 5);
    delay(300);
    //pwmWrite(12, 0);
    delay(300);
  }

}
