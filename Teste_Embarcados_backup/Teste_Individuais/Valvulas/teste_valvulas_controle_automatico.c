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

#define fator 1.2

// Global Variables
int ligado;
int serial_port;
int enable;
float pressure;
float hist;
float p_max1;
float p_max2;
float p_min;
FILE *load_measures;
FILE *state_valves;

// Declaration Functions
void serial_port_configuration();
float get_normal_load_measure();
void timer_useconds(long int useconds);
void set_pressure(int signo);
void set_high_pressure();
void disable_all_valve();
void reset_load_sensors();

// Main Function
int main(){
	pressure = 0;
	serial_port_configuration();
	float medida = get_normal_load_measure();
	
	load_measures = fopen("load_measures.csv", "w");
	state_valves = fopen("state_valves.csv", "w");
	
	while(abs(medida) > 0.1){
		reset_load_sensors();
		medida = get_normal_load_measure();
	}
	
	disable_all_valve();
	signal(SIGALRM, set_pressure);
	timer_useconds(40000);
	
	while(pressure >= 0){
		printf("Digite a pressão a ser definida: ");
		scanf("%f", &pressure);
		printf("\nDigite a histere a ser definida: ");
		scanf("%f", &hist);
		printf("\nDigite 1 para ativar alta pressão: ");
		scanf("%f", &enable);
		set_high_pressure();
		
		p_max1 = pressure + (hist/2);
		p_max2 = p_max1 * fator;
		p_min = pressure - (hist/2);
	}
	
	fclose(load_measures);
	fclose(state_valves);
	
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
	// v1 - válvula de entrada - GPIO 16
	// v2 - válvula de saída - GPIO 20
	
	static int estado1 = 0;
	static int estado2 = 0;
	
	if(pressure >= 0){
		float normal_load = get_normal_load_measure(); 
		fprintf(load_measures, "%.2f\n", normal_load);
		
		switch(estado1){
			case 0:
				if(normal_load < p_min){
					system("echo 1 > /sys/class/gpio/gpio16/value");
					fprintf(state_valves, "V1 = 1;\n");
					estado1++;
				}
			break;
			
			case 1:
				if(normal_load > p_max1){
					system("echo 0 > /sys/class/gpio/gpio16/value");
					fprintf(state_valves, "V1 = 0;\n");
					estado1 = 0;
				}
			break;
		}
					
	}
	else{
		system("echo 0 > /sys/class/gpio/gpio16/value");
		system("echo 1 > /sys/class/gpio/gpio20/value");
		fprintf(state_valves, "V1 = 0;\n");
		fprintf(state_valves, "V2 = 1;\n");
	}
	
	

}


// Set Low Pressure
void set_high_pressure(){
	// v3 - válvula de alta pressão
	if(enable == 1){
		system("echo 1 > /sys/class/gpio/gpio21/value");
		fprintf(state_valves, "V3 = 1;\n");
	}
	else{
		system("echo 0 > /sys/class/gpio/gpio21/value");
		fprintf(state_valves, "V3 = 0;\n");
	}
	
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

