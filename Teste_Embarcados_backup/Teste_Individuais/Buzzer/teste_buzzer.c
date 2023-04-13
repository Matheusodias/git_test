// Library headers
#include <wiringPi.h>
#include <softTone.h>
#include <stdio.h>
#include <unistd.h> 
#include <signal.h>
#include <sys/time.h>

// Defines
#define BuzzPin 26
#define CL1 131 
#define CM7 495 
#define CH7 990 

// Global Variables
int i;
int enable;
int ligado;

// Function Declaration
void set_alarm1(int signo);
void control_alarm1();
void timer_seconds(long int seconds);

// Main Function
int main(void){
	i = 0;
	enable = 0;
	ligado = 0;

	if(wiringPiSetup() == -1){
		printf("WiringPi initialization failed !");
		return 1;
	}
	if(softToneCreate(BuzzPin) == -1){
		printf("Soft Tone Failed !");
		return 1;
	}

	while(1){
		printf("Digite o comando a ser enviado:\n"
				"0 - Desativar alarme\n"
				"1 - Ativar alarme\n");
		int option = 2;
        scanf("%d",&option);
		
		switch (option){
            case 0: 
				printf("Alarme desativado...\n");
				enable = 4;
				ligado = 0;
				control_alarm1();
			break;
			
			case 1:
				printf("Alarme acionado...\n");
				enable = 0;
				ligado = 1;
				control_alarm1();
			break;
			
			default:
				printf("Comando inválido!\n");
			break;
		}
	}
	return 0;
}

// Set Alarm
void set_alarm1(int signo){
	//printf("Controlando alarme ...\n");
	if(enable < 2){
		if(i%2 == 0){
			softToneWrite(BuzzPin, CM7);
			printf("Ligando som pela %dª vez...\n", enable+1);
			enable++;
		}
		else{
			softToneWrite(BuzzPin, 0);

		}
		i++;
	}
	else{
		softToneWrite(BuzzPin, 0);
		ligado = 0;
	}
	
}

// Control Alarm
void control_alarm1(){

	signal(SIGALRM, set_alarm1);
	timer_seconds(1);
	
	while(ligado);
}

// Time for Alarm
void timer_seconds(long int seconds){
	struct itimerval timer1;
	 
	timer1.it_interval.tv_usec = 0;
	timer1.it_interval.tv_sec = seconds;
	timer1.it_value.tv_usec = 0;
	timer1.it_value.tv_sec = seconds;
	 
	setitimer (ITIMER_REAL, &timer1, NULL );
}