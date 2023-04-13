#include <wiringPi.h>
#include <softTone.h>
#include <stdio.h>
#include <unistd.h> 
#include <signal.h>
#include <sys/time.h>

// Defines
#define BuzzPin 26

// Global Variables
int i;
int enable;
int ligado;
int time = 500000;

// Function Declaration
void set_alarm1(int tone);

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
				set_alarm1(0);
			break;
			
			case 1:
        printf("Digite o tom a ser enviado: ");
        int tone = 0;
        scanf("%d",&tone);
				set_alarm1(tone);
			break;
			
			default:
				printf("Comando inválido!\n");
			break;
		}
	}
	return 0;
}

void set_alarm1(int tone){
	printf("Controlando alarme ...\n");
	if(tone != 0){
			softToneWrite(BuzzPin, tone);
			usleep(time);
                        softToneWrite(BuzzPin, tone+100);
 			usleep(time);
			softToneWrite(BuzzPin, tone-100);
			printf("Alarme acionado...\n");	
      sleep(2);
      softToneWrite(BuzzPin, 0);
	}
	else{
		softToneWrite(BuzzPin, 0);
	}
	
}
