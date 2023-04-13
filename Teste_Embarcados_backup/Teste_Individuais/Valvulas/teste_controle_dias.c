// Library headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <errno.h> 
#include <termios.h> 
#include <unistd.h> 
#include <signal.h>
#include <sys/time.h>
#include <time.h>
//#include <conio.h>

// Defines
#define hist 50

// Global Variables
int ligado;
int serial_port;
float setpoint;
FILE *measures;
char nomeArquivo[100];
size_t comprimentoVetor = 100;
time_t agora;

// Declaration Functions
void serial_port_configuration();
float get_normal_load_measure();
void timer_seconds(long int seconds);
void timer_useconds(long int useconds);
void set_pressure(int signo);
void control_pressure();
void set_low_pressure();
void set_valve(int valv, int state);
void disable_all_valve();
void enable_all_valve();
void reset_load_sensors();
void end_file(int signum);

// Main Function
int main(){
	
    signal(SIGINT,end_file);

	ligado = 0;
	serial_port_configuration();
	disable_all_valve();
	//reset_load_sensors();
	
	agora = time(NULL);
    strftime(nomeArquivo, comprimentoVetor, "Measures_%F_%Hh_%Mmin.csv", localtime(&agora));
	
    measures = fopen(nomeArquivo, "w");
	if(measures == NULL)
	{
		printf("Erro na abertura do arquivo!");
		return 1;
	}
	
	while(1){
		printf("Digite o modo de controle:\n"
				"0 - Controle Manual\n"
				"1 - Controle Automático\n"
				"2 - Desligar Sistema\n");
		int option = 3;
        scanf("%d",&option);
		
		switch (option){
            case 0: 
				while(1){
					printf("Digite o número da válvula e 0 - para desativar ou 1 - para ativar: \n"
							"1 - Válvula 1 (Supply)\n"
							"2 - Válvula 2 (Ventilação)\n"
							"3 - Válvula 3 (Alta Pressão)\n"
							"4 - Ativar todas as válvulas\n"
							"5 - Desativar todas as válvulas\n");
					int valv = 0;
					int state = 0;
					scanf("%d %d", &valv, &state);
					
					if(valv < 4){
						set_valve(valv, state);
					}
					else if(valv == 4){
						enable_all_valve();
					}
					else if(valv == 5){
						disable_all_valve();
					}
					else{
						printf("Escolha inválida!");
						break;
					}
				}
			break;
			
			case 1:
				//reset_load_sensors();
				printf("Digite a pressão a ser definida: ");
				scanf("%f", &setpoint);
				printf("\nControle automático da pressão ...\n ");
				ligado = 1;
				reset_load_sensors();
				set_low_pressure();
				control_pressure();
			break;
			
			case 2:
				printf("Desativando sistema ...\n");
				ligado = 0;
				system("echo 0 > /sys/class/gpio/gpio16/value");
				system("echo 1 > /sys/class/gpio/gpio20/value");
				system("echo 0 > /sys/class/gpio/gpio21/value");
			default:
				printf("Escolha inválida!");
			break;
		}
	}
};

// Serial Port Configuration
void serial_port_configuration(){
	
	printf("Configuring serial port ...\n");
	
	struct termios tty;
	char path_disp[] = "/dev/ttyUSB";
	char cmd[12];

	sprintf(cmd, "%s%d", path_disp, 0);
	serial_port = open(cmd, O_RDWR);
	
	if(tcgetattr(serial_port, &tty) != 0) {
		printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
		//dados.state = 1;
	}
	
	tty.c_cflag &= ~PARENB; 
	tty.c_cflag &= ~CSTOPB; 
	tty.c_cflag &= ~CSIZE; 
	tty.c_cflag |= CS8; 
	tty.c_cflag &= ~CRTSCTS; 

	tty.c_cc[VTIME] = 10;    
	tty.c_cc[VMIN] = 0;
	
	cfsetispeed(&tty, B9600);
	cfsetospeed(&tty, B9600);

	if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		//dados.state = 1;
	}
}

// Get Normal Load Measurement
float get_normal_load_measure(){
	
	int j;

	char load_measures[10];
	int load_num_bytes = 0;	
	int load_port = serial_port;
	float load_measure;
	
		
	while(load_num_bytes <= 0){
		// Command to receive load sensor data
		unsigned char cmd_load[] = {"*1B1 \r\0"};
		for(j = 0; j<strlen(cmd_load); j++){
			write(load_port, cmd_load+j, 1);
		}

		// Receiving data from shear load sensor
		memset(&load_measures, '\0', sizeof(load_measures));
		load_num_bytes = read(load_port, &load_measures, sizeof(load_measures));
		
		if (load_num_bytes < 0){
			printf("Error reading: %s", strerror(errno));
			// dados.state = 1;
		}
	}
	
	// Filling structure for sending to interface
	if (load_num_bytes > 1){
		load_measure =  atof(load_measures);
		//printf("Medida sensor de carga: %.2f\n", load_measure);
	}
	
	return load_measure;
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

void timer_useconds(long int useconds){
	struct itimerval timer2;
 
	timer2.it_interval.tv_usec = useconds;
	timer2.it_interval.tv_sec = 0;
	timer2.it_value.tv_usec = useconds;
	timer2.it_value.tv_sec = 0;
	 
	setitimer (ITIMER_REAL, &timer2, NULL );
}

// Set Pressure
void set_pressure(int signo){
	// v1 - válvula de entrada - GPIO 
	// v2 - válvula de saída - GPIO
	int v1 = 0;
	if(ligado == 1){
		float normal_load = get_normal_load_measure();
		
		if(normal_load < (setpoint - (hist/2))){
			system("echo 1 > /sys/class/gpio/gpio16/value");
			v1 = 1;
		}
		else if(normal_load > (setpoint + (hist/2))){
			system("echo 0 > /sys/class/gpio/gpio16/value");
			v1 = 0;
		}
		fprintf(measures, "%.1f;%d\n", normal_load, v1);
	}
	else{
		system("echo 0 > /sys/class/gpio/gpio16/value");
		system("echo 1 > /sys/class/gpio/gpio20/value");
		system("echo 0 > /sys/class/gpio/gpio21/value");
	}
}

// Control Pressure
void control_pressure(){
	
	signal(SIGALRM, set_pressure);
	timer_useconds(1000);
	
	while(1);
}

// Set Low Pressure
void set_low_pressure(){
	// v3 - válvula de baixa pressão
	system("echo 0 > /sys/class/gpio/gpio20/value");
	usleep(1000000);	
	system("echo 1 > /sys/class/gpio/gpio21/value");
	usleep(1000000);
	printf("Baixa pressão acionada!");
}

// Set Valve
void set_valve(int valv, int state){
	char msg[40];
	if(valv == 1){
		snprintf(msg, 40, "echo %d > /sys/class/gpio/gpio16/value", state);
	}
	else if(valv == 2){
		snprintf(msg, 40, "echo %d > /sys/class/gpio/gpio20/value", state);
	}
	else{
		snprintf(msg, 40, "echo %d > /sys/class/gpio/gpio21/value", state);
	}
	system(msg);
	printf("Valvula %d acionada!", valv);
}

// Disable All Valves
void disable_all_valve(){

	system("echo 0 > /sys/class/gpio/gpio16/value");
	system("echo 0 > /sys/class/gpio/gpio20/value");
	system("echo 0 > /sys/class/gpio/gpio21/value");
}

// Enable All Valves
void enable_all_valve(){

	system("echo 1 > /sys/class/gpio/gpio16/value");
	system("echo 1 > /sys/class/gpio/gpio20/value");
	system("echo 1 > /sys/class/gpio/gpio21/value");
}

void reset_load_sensors(){
	
	system("sudo echo 1 > /sys/class/gpio/gpio23/value");
	usleep(10);
	system("sudo echo 0 > /sys/class/gpio/gpio23/value");

	return; 
	
}	

void end_file(int signum){

	fprintf(stderr,"Fechando máquina %d\n", signum);
	fclose(measures);
	exit(0);
}
