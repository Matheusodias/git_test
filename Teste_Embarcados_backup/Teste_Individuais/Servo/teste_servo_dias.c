// Library headers
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

// Defines
#define c_dist 16106500
#define c_vel 16106500
#define acel 100
#define ports 2

// Declaration Functions
void serial_port_configuration();
void configuration_servo();
void get_shear_disp_measure();
void velocity_mode(float vel_mv);
void position_mode(float dis_mp, float vel_mp);
void report_status();
void stop_move();
void end_machine();
void timer_useconds(long int useconds);
void get_measure_control(int signo);
void reset_buffer();

// Global Variables

int serial_ports[ports];
int RBt;
int enabled;
FILE *shear_disp_measures;

// Main Function
int main() {
	
	signal(SIGINT,end_machine);
	
	serial_port_configuration();
	configuration_servo();
	shear_disp_measures = fopen("shear_disp_measures.csv", "w");
	if(shear_disp_measures == NULL)
	{
		printf("Erro na abertura do arquivo!");
		return 1;
	}
	
	enabled = 0;
	
	signal(SIGALRM, get_measure_control);
	timer_useconds(250000);
	
	while(1){
		printf("Digite o Comando a ser enviado:\n"
				"0 - Modo Velocidade\n"
				"1 - Modo Posição\n"
				"2 - Reporte de Estado\n"
				"3 - Parar Movimento\n");
				
		int option = 4;
        scanf("%d",&option);
		
		switch (option){
            case 0: 
				printf("Digite a velocidade do movimento (em mm/s): ");
				float vel_mv = 0;
				scanf("%f", &vel_mv);
				
				velocity_mode(vel_mv);
				enabled = 1;
				
				
			break;
			
			case 1:
				printf("Digite a distância (em mm) e a velocidade (em mm/s) do movimento: ");
				float vel_mp = 0;
				float dis_mp = 0;
				scanf("%f %f", &dis_mp, &vel_mp);
				
				position_mode(dis_mp, vel_mp);
				enabled = 1;
	
			break;
			
			case 2:
				printf("Reporte de Estado ...\n");
				report_status();
				reset_buffer();
			break;
			
			case 3:
				printf("Encerrando movimento ...\n");
				enabled = 0;
				stop_move();
				
			break;
			
			default:
				printf("Escolha inválida!");
			break;
		}
	}
	
	//close(serial_port);
	return 0; 
}

// Serial Port Configuration
void serial_port_configuration(){
	
	char *path_servo = "/dev/ttyS";
	char *path_disp = "/dev/ttyAMA";
	char cmd[ports][12];
	struct termios tty[ports];
	
	for(int i = 0; i < ports; i++){
		
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
void configuration_servo(){
	
	int serial_port = serial_ports[0];
	
	char msg[] = {"CMD UCP UDM\r"};
	for(int i = 0; i<strlen(msg); i++){
		write(serial_port, msg+i, 1);
	}
	//printf("Message sent: %s", msg);
	
	usleep(10);

	char read_servo[50];
	memset(&read_servo, '\0', sizeof(read_servo));
	
	int num_bytes = read(serial_port, &read_servo, sizeof(read_servo));
	//printf("Message received!\n");

	if (num_bytes < 0) {
		printf("Error reading: %s", strerror(errno));
	}
	else{
		//printf("Read %i bytes. Received message: %s", num_bytes, read_servo);
	}
}

void get_measure_control(int signo){
	if(enabled == 1){
		get_shear_disp_measure();
	}
}

// Get Normal disp Measurement
void get_shear_disp_measure(){
	
	int j;

	char disp_measures[15];
	char disp_temp[7];
	int disp_num_bytes = 0;	
	int disp_port = serial_ports[1];
	float disp_measure;
	
	while(disp_num_bytes < 14){
		// Command to receive displacement sensors data
		system("echo 1 > /sys/class/gpio/gpio6/value");
		usleep(0.01);
		system("echo 0 > /sys/class/gpio/gpio6/value");
		
		memset(&disp_measures, '\0', sizeof(disp_measures));
		disp_num_bytes = read(disp_port, &disp_measures, sizeof(disp_measures));
		
		if (disp_num_bytes < 0){
			printf("Error reading: %s", strerror(errno));
		}
	}
	
	if (disp_num_bytes == 14) {
		int len = strlen(disp_measures);
		
		if(disp_measures[len-1] == '\n'){
			disp_measures[--len] = 0;
		}
		memset(&disp_temp, '\0', sizeof(disp_temp));
		
		for(int j = 0; j < (sizeof(disp_temp)-1); j++){
			if(j==0){
				disp_temp[j] = disp_measures[1];
			}
			if(j==1){
				if(disp_measures[4] == ' '){
					disp_temp[j] = '0';
				}
				else{
					disp_temp[j] = disp_measures[4];
				}
			}
			else{
				disp_temp[j] = disp_measures[j+3];
			}
		}
		
		disp_measure = atof(disp_temp);
		//printf("Medida sensor de deslocamento: %.3f\n", disp_measure);
		fprintf(shear_disp_measures, "%.3f\n", disp_measure);

	}

	
}

// Velocity Mode 
void velocity_mode(float vel_mv){
	
	int serial_port = serial_ports[0];
	
	int vel_res = lround(vel_mv * c_vel);
	
	char msg[50];
	snprintf(msg, 50, "MV A=%d V=%d G\r", acel, vel_res);
	for(int i = 0; i<strlen(msg); i++){
		write(serial_port, msg+i, 1);
	}
	//printf("Message sent: %s", msg);
	
	usleep(10);

	char read_servo[50];
	memset(&read_servo, '\0', sizeof(read_servo));
	
	int num_bytes = read(serial_port, &read_servo, sizeof(read_servo));
	//printf("Message received!\n");

	if (num_bytes < 0) {
		printf("Error reading: %s", strerror(errno));
	}
	else{
		//printf("Read %i bytes. Received message: %s", num_bytes, read_servo);
	}
}

// Position Mode
void position_mode(float dis_mp, float vel_mp){
	
	int serial_port = serial_ports[0];
	
	int dist_res = lround(dis_mp * c_dist);
	int vel_res = lround(vel_mp * c_vel);
	
	char msg[50];
	snprintf(msg, 50, "MP O=0 A=%d V=%d P=%d G\r", acel, vel_res, dist_res);
	for(int i = 0; i<strlen(msg); i++){
		write(serial_port, msg+i, 1);
	}
	//printf("Message sent: %s", msg);
	
	usleep(10);

	char read_servo[50];
	memset(&read_servo, '\0', sizeof(read_servo));
	
	int num_bytes = read(serial_port, &read_servo, sizeof(read_servo));
	//printf("Message received!\n");

	if (num_bytes < 0) {
		printf("Error reading: %s", strerror(errno));
	}
	else{
		//printf("Read %i bytes. Received message: %s", num_bytes, read_servo);
	}
}


void report_status(){
	
	int serial_port = serial_ports[0];
	
	char msg[] = {"RBl RBr\r"};
	for(int i = 0; i<strlen(msg); i++){
		write(serial_port, msg+i, 1);
	}
	//printf("Message sent: %s", msg);
	
	usleep(10);

	char read_servo[50];
	memset(&read_servo, '\0', sizeof(read_servo));
	
	int num_bytes = read(serial_port, &read_servo, sizeof(read_servo));
	//printf("Message received!\n");

	if (num_bytes < 0) {
		printf("Error reading: %s", strerror(errno));
	}
	else{
		printf("Read %i bytes.\nReceived message: %c - %c\n\n", num_bytes, read_servo[4], read_servo[10]);
		int RBl = read_servo[4] - '0';
		int RBr = read_servo[10] - '0';
	}
}

// Stop Move
void stop_move(){
	
	int serial_port = serial_ports[0];
	
	char msg[] = {"X\r"};
	for(int i = 0; i<strlen(msg); i++){
		write(serial_port, msg+i, 1);
	}
	//printf("Message sent: %s", msg);
	
	usleep(10);

	char read_servo[50];
	memset(&read_servo, '\0', sizeof(read_servo));
	
	int num_bytes = read(serial_port, &read_servo, sizeof(read_servo));
	//printf("Message received!\n");

	if (num_bytes < 0) {
		printf("Error reading: %s", strerror(errno));
	}
	else{
		//printf("Read %i bytes. Received message: %s", num_bytes, read_servo);
	}
}

// Stop Move
void reset_buffer(){
	
	int serial_port = serial_ports[0];
	
	char msg[] = {"Z\r"};
	for(int i = 0; i<strlen(msg); i++){
		write(serial_port, msg+i, 1);
	}
	
	usleep(10);

	char read_servo[50];
	memset(&read_servo, '\0', sizeof(read_servo));
	
	int num_bytes = read(serial_port, &read_servo, sizeof(read_servo));

	if (num_bytes < 0) {
		printf("Error reading: %s", strerror(errno));
	}

}

void timer_useconds(long int useconds){
	struct itimerval timer2;
 
	timer2.it_interval.tv_usec = useconds;
	timer2.it_interval.tv_sec = 0;
	timer2.it_value.tv_usec = useconds;
	timer2.it_value.tv_sec = 0;
	 
	setitimer (ITIMER_REAL, &timer2, NULL );
}

void timer_seconds(long int seconds){
	struct itimerval timer1;
	 
	timer1.it_interval.tv_usec = 0;
	timer1.it_interval.tv_sec = seconds;
	timer1.it_value.tv_usec = 0;
	timer1.it_value.tv_sec = seconds;
	 
	setitimer (ITIMER_REAL, &timer1, NULL );
}

void end_machine(int signum){

	fprintf(stderr,"Fechando máquina %d\n", signum);
	enabled = 0;
	fclose(shear_disp_measures);
	exit(0);
}




	
