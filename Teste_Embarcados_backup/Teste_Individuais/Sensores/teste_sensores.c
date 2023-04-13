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

// Defines
#ifndef SOCKET_LOCAL_H
#define SOCKET_LOCAL_H

#define machine_payload_size 8*sizeof(int)
typedef union {
    unsigned char payload[machine_payload_size];
    struct {
        int sample_number;
        float displacement[2];
        float load[2];
        int state;
    };
} machine_message;
#endif

#define ports 4
#define n_sensors 2

// Global Variables
machine_message dados;
int serial_ports[ports];
int n_sample;
int enabled;
FILE *measures;

// Function Declaration
void serial_port_configuration();
void get_measures(int signo);
void timer_seconds(long int seconds);
void timer_useconds(long int useconds);
void reset_load_sensors();
void end_server(int signum);

// Main Function
int main() {
	
	signal(SIGINT,end_server);
	
	enabled = 0;
	measures = fopen("measures.csv", "w");
	if(measures == NULL)
	{
		printf("Erro na abertura do arquivo!");
		return 1;
	}
	
	serial_port_configuration();
	reset_load_sensors();
	
	printf("Digite 1 para habilitar leitura dos sensores: ");
	scanf("%d", &enabled);
	printf("------------------------------------------------\n");
	
	if(enabled == 1){
		signal(SIGALRM, get_measures);
		timer_seconds(1);
	}
	while(1);

};

// Serial Ports Configuration
void serial_port_configuration(){
	
	printf("Configuring serial ports ...\n");
	
	int i;
	struct termios tty[ports];
	char *path_disp = "/dev/ttyAMA";
	char *path_load = "/dev/ttyUSB";
	char cmd[ports][12];
	
	for(i = 0; i < ports; i++){
		if(i < 2){
			sprintf(cmd[i], "%s%d", path_disp, i+1);
		}
		else{
			sprintf(cmd[i], "%s%d", path_load, (i-2));
		}
		
		serial_ports[i] = open(cmd[i], O_RDWR);
		//printf("Configuring serial port %s in position %d = serial port %d\n", cmd[i], i, serial_ports[i]);
		
		if(tcgetattr(serial_ports[i], &tty[i]) != 0) {
			printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));

		}
		
		tty[i].c_cflag &= ~PARENB; 
		tty[i].c_cflag &= ~CSTOPB; 
		tty[i].c_cflag &= ~CSIZE; 
		tty[i].c_cflag |= CS8; 
		tty[i].c_cflag &= ~CRTSCTS; 

		tty[i].c_cc[VTIME] = 10;    
		tty[i].c_cc[VMIN] = 0;
		
		if(i < 2){
			cfsetispeed(&tty[i], B1200);
			cfsetospeed(&tty[i], B1200);
		}
		else{
			cfsetispeed(&tty[i], B9600);
			cfsetospeed(&tty[i], B9600);
		}
		
		if (tcsetattr(serial_ports[i], TCSANOW, &tty[i]) != 0) {
			printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		}
	}
}

// Get Measures From Sensors
void get_measures(int signo){
	
	printf("Reading sensors - Sample period: 1s ...\n");
	
	int i, j;
	char disp_temp[n_sensors][7];
	char disp_measures[n_sensors][15];
	char load_measures[n_sensors][10];
	int disp_num_bytes[n_sensors];
	int load_num_bytes[n_sensors];	

	int disp_ports[n_sensors] = {serial_ports[0], serial_ports[1]};
	int load_ports[n_sensors] = {serial_ports[2], serial_ports[3]};
	
	// Increment sample number
	n_sample += 1;
	dados.sample_number = n_sample;
	printf("Amostra nº: %d\n", dados.sample_number);
	fprintf(measures, "%d;", n_sample);
	
	for(i=0; i < n_sensors; i++){
		
		while(load_num_bytes[i] <= 0){
			// Command to receive load sensor data
			unsigned char cmd_load[] = {"*1B1 \r\0"};
			for(j = 0; j<strlen(cmd_load); j++){
				write(load_ports[i], cmd_load+j, 1);
			}

			// Receiving data from shear load sensor
			memset(&load_measures[i], '\0', sizeof(load_measures[i]));
			load_num_bytes[i] = read(load_ports[i], &load_measures[i], sizeof(load_measures[i]));
			
			if (load_num_bytes[i] < 0){
				printf("Error reading: %s", strerror(errno));
			}
		}
		
		// Filling structure for sending to interface
		if (load_num_bytes[i] > 1){
			int len = strlen(load_measures[i]);
			
			if(load_measures[i][len-1] == '\n'){
				load_measures[i][--len] = 0;
			}
				
			dados.load[i] = atof(load_measures[i]);
			printf("Leitura sensor de carga %d: %s\n", (i+1), load_measures[i]);
			fprintf(measures, "%s;", load_measures[i]);
			printf("Medida sensor de carga %d: %.2f\n", (i+1), dados.load[i]);
			fprintf(measures, "%.2f;", dados.load[i]);
		}

	
		// Receiving data from displacement sensors

		while(disp_num_bytes[i] < 14){
			// Command to receive displacement sensors data
			system("echo 1 > /sys/class/gpio/gpio6/value");
			usleep(0.01);
			system("echo 0 > /sys/class/gpio/gpio6/value");
			
			memset(&disp_measures[i], '\0', sizeof(disp_measures[i]));
			disp_num_bytes[i] = read(disp_ports[i], &disp_measures[i], sizeof(disp_measures[i]));
			
			if (disp_num_bytes[i] < 0){
				printf("Error reading: %s", strerror(errno));
			}
		}
		
		// Filling structure for sending to interface
		if (disp_num_bytes[i] == 14) {
			int len = strlen(disp_measures[i]);
			
			if(disp_measures[i][len-1] == '\n'){
				disp_measures[i][--len] = 0;
			}
			memset(&disp_temp[i], '\0', sizeof(disp_temp[i]));
			
			for(int j = 0; j < (sizeof(disp_temp[i])-1); j++){
				if(j==0){
					disp_temp[i][j] = disp_measures[i][1];
				}
				if(j==1){
					if(disp_measures[i][4] == ' '){
						disp_temp[i][j] = '0';
					}
					else{
						disp_temp[i][j] = disp_measures[i][4];
					}
				}
				else{
					disp_temp[i][j] = disp_measures[i][j+3];
				}
			}
			
			dados.displacement[i] = atof(disp_temp[i]);
			printf("Leitura sensor de deslocamento %d: %s\n", (i+1), disp_measures[i]);
			printf("Medida sensor de deslocamento %d: %.3f\n", (i+1), dados.displacement[i]);
			
			if(i == 1){
				fprintf(measures, "%s;", disp_measures[i]);
				fprintf(measures, "%.3f\n", dados.displacement[i]);
			}
			else{
				fprintf(measures, "%s;", disp_measures[i]);
				fprintf(measures, "%.3f;", dados.displacement[i]);
			}
		}
		
		load_num_bytes[i] = 0;
		disp_num_bytes[i] = 0;
		
	}
	printf("------------------------------------------------\n");
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

void reset_load_sensors(){
	system("echo 1 > /sys/class/gpio/gpio23/value");
	usleep(100000);
	system("echo 0 > /sys/class/gpio/gpio23/value");
	return; 
	
}

void end_server(int signum){

	fprintf(stderr,"Fechando máquina %d\n", signum);
	fclose(measures);
	exit(0);
}

