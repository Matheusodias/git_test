#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h> 
#include <errno.h> 
#include <termios.h> 
#include <unistd.h> 
#include <math.h>
#include <signal.h>
#include <sys/time.h>

#define c_dist 2000
#define c_vel 32212
#define acel 100
#define servo_ports 2


// Serial Port Configuration
void servo_serial_port_configuration(int serial_ports[servo_ports]){
	
	char *path_servo = "/dev/ttyS";
	char *path_disp = "/dev/ttyAMA";
	char cmd[servo_ports][12];
	struct termios tty[servo_ports];
	
	for(int i = 0; i < servo_ports; i++){
		
		if(i == 0){
			sprintf(cmd[i], "%s%d", path_servo, i);
		}
		else{
			sprintf(cmd[i], "%s%d", path_disp, i+1);
		}
	
		serial_ports[i] = open(cmd[i], O_RDWR);

		if(tcgetattr(serial_ports[i], &tty[i]) != 0) {
			printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
			//dados.state = 1;
		}
		
		tty[i].c_cflag &= ~PARENB; 
		tty[i].c_cflag &= ~CSTOPB; 
		tty[i].c_cflag |= CS8; 

		tty[i].c_cc[VTIME] = 10;    
		tty[i].c_cc[VMIN] = 0;
		
		if(i == 0){
			cfsetispeed(&tty[i], B9600);
			cfsetospeed(&tty[i], B9600);
		}
		else{
			cfsetispeed(&tty[i], B1200);
			cfsetospeed(&tty[i], B1200);
		}
		
		if (tcsetattr(serial_ports[i], TCSANOW, &tty[i]) != 0) {
			printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
			//dados.state = 1;
		}
	}
	
}

// Servo Setup Commands
void configuration_servo(int serial_ports[servo_ports]){
	usleep(100);
	int serial_port = serial_ports[0];
	
	char msg[13] = {"CMD UCP UDM\r"};
	for(int i = 0; i<strlen(msg); i++){
		write(serial_port, msg+i, 1);
	}
	printf("Message sent on configuration_servo: %s\n", msg);
	
	usleep(10);

	char read_servo[50];
	memset(&read_servo, '\0', sizeof(read_servo));
	
	int num_bytes = read(serial_port, &read_servo, sizeof(read_servo));
	//printf("Message received!\n");

	if (num_bytes < 0) {
		printf("Error reading: %s\n", strerror(errno));
	}
	else{
		//printf("Read %i bytes. Received message: %s\n", num_bytes, read_servo);
	}
}

// Interruption Timer in uSeconds
void timer_useconds(long int useconds){
	struct itimerval timer2;
 
	timer2.it_interval.tv_usec = useconds;
	timer2.it_interval.tv_sec = 0;
	timer2.it_value.tv_usec = useconds;
	timer2.it_value.tv_sec = 0;
	 
	setitimer (ITIMER_REAL, &timer2, NULL );
}

// Velocity Mode 
void velocity_mode(int vel_mv, int serial_ports[servo_ports]){
	usleep(100);
	int serial_port = serial_ports[0];
	
	int vel_res = vel_mv * c_vel;
	
	char msg[50];
	snprintf(msg, 50, "MV A=%d V=%d G\r", acel, vel_res);
	for(int i = 0; i<strlen(msg); i++){
		write(serial_port, msg+i, 1);
	}
	printf("Message sent on Velocity Mode: %s\n", msg);
	
	usleep(10);

	char read_servo_vm[50];
	memset(&read_servo_vm, '\0', sizeof(read_servo_vm));
	
	int num_bytes = read(serial_port, &read_servo_vm, sizeof(read_servo_vm));
	//printf("Message received!\n");

	if (num_bytes < 0) {
		printf("Error reading: %s", strerror(errno));
	}
	else{
		//printf("Read %i bytes. Received message: %s\n", num_bytes, read_servo_vm);
	}
}

// Stop Move
void stop_move(int serial_ports[servo_ports]){

	int serial_port = serial_ports[0];
	
	char msg[] = {"X\r"};
	for(int i = 0; i<strlen(msg); i++){
		write(serial_port, msg+i, 1);
	}
	printf("Message sent on Stop Move: %s\n", msg);
	
	usleep(10);

	char read_servo_sm[50];
	memset(&read_servo_sm, '\0', sizeof(read_servo_sm));
	
	int num_bytes = read(serial_port, &read_servo_sm, sizeof(read_servo_sm));
	//printf("Message received!\n");

	if (num_bytes < 0) {
		printf("Error reading: %s\n", strerror(errno));
	}
	else{
		//printf("Read %i bytes. Received message: %s\n", num_bytes, read_servo_sm);
	}
}
