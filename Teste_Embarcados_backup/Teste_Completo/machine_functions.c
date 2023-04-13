// library headers
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <fenv.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <errno.h> 
#include <termios.h> 
#include <unistd.h> 
#include <bcm2835.h>
#include <pthread.h>
#include "machine_functions.h"

char socket_name_cliente[] = "./interface";
int socket_id_cliente;
char socket_name_servidor[] = "./machine";
int  socket_id_servidor;
int  id_socket_cliente;

machine_message dados;
int serial_ports[ports];
int ligado;
int enable;
int enable_press;
int time_sample;
int n_sample;
float pressure;
float p_max1;
float p_max2;
float p_min;
FILE *data_send;

pthread_t thread_envia_dados;
pthread_t thread_controle_pressao;
pthread_t thread_checa_movimento;


int connection_cliente_machine(){
	
	signal(SIGINT, end_server);
    
    struct sockaddr name;
    memset(&name,0,sizeof(name));
	
	data_send = fopen("data_send.txt", "w");
    
    socket_id_cliente = socket(PF_LOCAL, SOCK_STREAM,0);
    name.sa_family = AF_LOCAL;
	
	printf("%s\n", socket_name_cliente);
	printf("%ld\n", strlen(socket_name_cliente));
	
    strncpy(name.sa_data, socket_name_cliente, 17);
    //strcpy(name.sa_data, socket_name_cliente);
    
    printf("%s\n",name.sa_data);
	printf("%ld\n",strlen(name.sa_data) + 2);

    if(connect(socket_id_cliente, &name, strlen(name.sa_data) + sizeof(name.sa_family))!=0){
        printf("ErroThe last error message is: %s\n", strerror(errno));
        close(socket_id_cliente);
        return -1;
    }  

    return 0;
}

void send_data_cliente_machine(){
	//printf("Tamanho do payload= %ld\n",sizeof(machine_message));
	write(socket_id_cliente, dados.payload, sizeof(machine_message));
	fprintf(data_send, "---------- Data Send ----------\n");
	fprintf(data_send, "Sample number: %d\n", dados.sample_number);
	fprintf(data_send, "Displecement Measure 1: %.3f\n", dados.displacement[0]);
	fprintf(data_send, "Load Measure 1: %.2f\n", dados.load[0]);
	fprintf(data_send, "Displecement Measure 2: %.3f\n", dados.displacement[1]);
	fprintf(data_send, "Load Measure 1: %.2f\n", dados.load[0]);
	
	//printf("----- Enviou dados ------\n");
	
	return;
}

int servidor_machine(){
	
	signal(SIGINT,end_server);
    struct sockaddr socket_struct;
    
    socket_id_servidor = socket(PF_LOCAL, SOCK_STREAM, 0);
    socket_struct.sa_family = AF_LOCAL;
    strncpy(socket_struct.sa_data, socket_name_servidor, strlen(socket_name_servidor));
	
	if(bind(socket_id_servidor, &socket_struct, sizeof(socket_struct))!=0)
    {
        printf("Error no bind do server socket id: %s\n", strerror(errno));
        return -1;
    }
	
	printf("Nome do socket = %s\n",socket_struct.sa_data);

    if(listen(socket_id_servidor,10)!=0)
    {
        printf("Error in server listen: %s\n", strerror(errno));
        return -1;
    }
	
	return 0;
}

void received_command(){
	while(1){
		struct sockaddr clienteAddr;
		socklen_t clienteLength = sizeof(( struct sockaddr *) & clienteAddr);
		printf("antes do accept machine\n");
		id_socket_cliente = accept(socket_id_servidor, &clienteAddr, &clienteLength);
		if(id_socket_cliente < 0){
			printf("Error in server accept: %s\n", strerror(errno));
			usleep(500000);
			continue;
		}	
		printf("Depois do accept machine\n");
		
		define_command(id_socket_cliente);
	}
}


void end_server(int signum){

	fprintf(stderr,"Fechando máquina %d\n", signum);
	ligado = 0;
	pthread_join(thread_envia_dados, NULL);
	pthread_join(thread_controle_pressao, NULL);
	pthread_join(thread_checa_movimento, NULL);
	unlink(socket_name_servidor);
	close(socket_id_servidor);
    close(id_socket_cliente);
    close(socket_id_cliente);
	for(int i=0; i<ports; i++){
		close(serial_ports[i]);
	}
	exit(0);
}


void define_command (int client_socket){
	
	interface_message message;
	ligado = 1;
	//printf("Ligado: %d", ligado);
	
	while(ligado){
	
		int value = read(client_socket, &message.payload, interface_payload_size);
		//printf("read: %d", value);
		if(value<=0){
			dados.state |= (1<<7);
			break;
		};
		
		printf("Comando %d\n",message.command);
		switch (message.command)
		{
			case 0: // comando 0 iniciar recebimento de dados da máquina (enabled, sample_period);
				printf("Comando 0 - Habilitação = %d Periodo de amostragem = %d ms\n", message.enabled, message.sample_period);
				enable = message.enabled;
				//time_sample = (message.sample_period);
				time_sample = (message.sample_period*1000);
				timer_useconds(time_sample);
				
			break;
			
			case 1: // comando 1 movimentar motor com velocidade constante (velocidade com sinal positivo(direita), negativo(esquerda))
				if(message.velocity != 0){
					printf("Comando 1 - Ajuste de posição realizado com movimento com velocidade = %.3f mm/s\n",((float)message.velocity/1000));
					define_position(((float)message.velocity/1000));
				}
				else{
					printf("Comando 1 - Posição ajustada");
					define_position(0);
				}
			break;

			case 2: // comando 2 colocar uma pressão baixa, para fixar a amostra lowPressure(ligado).
				printf("Comando 2 - Baixa pressão ativada\n");
				pressure = message.pressure;
				enable_press = 1;
				p_max1 = pressure + (hist/2);
				p_max2 = p_max1 * fator;
				p_min = pressure - (hist/2);
				
				pthread_create(&thread_controle_pressao, NULL, &control_pressure, NULL);
				
			break;
			
			case 3: // comando 3 setLoad(pressure); Experimento inicia
				printf("Comando 3 - Pressão = %d Psi\n", message.pressure);
				set_high_pressure();
			break;
			
			case 4: // comando 4 setDistance_Velocity(distance,velocity) . Cisalhamento
				printf("Comando 4 - Distancia = %.2f Velocidade = %.3f\n", (float)message.distance, ((float)message.velocity/1000));
				move_machine(((float)message.velocity/1000), (float)message.distance);
				pthread_create(&thread_checa_movimento, NULL, &check_move, NULL);				
			break;

			case 5: // comando 5 0(disabled, 0), 2(desligado) 4(distance=0,velocity=0);interromper/finalizar experimento desliga pressão
				printf("Comando 5 - Encerrando maquina!\n");
				enable_press = 0;
				disable_pressure();
				move_machine(0, 0);
				end_server(SIGINT);
			break;
			
			default:
				break;
		}
	}
	
	fprintf(stderr,"Erro no read\n");
    
    end_server(2);
}


void serial_port_configuration(){
	
	printf("Configuring serial ports ...\n");
	
	int i;
	struct termios tty[ports];
	char *path_servo = "/dev/ttyS";
	char *path_disp = "/dev/ttyAMA";
	char *path_load = "/dev/ttyUSB";
	char cmd[ports][12];
	
	for(i = 0; i < ports; i++){
		if(i == 0){
			sprintf(cmd[i], "%s%d", path_servo, i);
		}
		else if(i > 0 && i < 3){
			sprintf(cmd[i], "%s%d", path_disp, i);
		}
		else{
			sprintf(cmd[i], "%s%d", path_load, (i-3));
		}
		
		serial_ports[i] = open(cmd[i], O_RDWR);
		printf("Configuring serial port %s in position %d = serial port %d\n", cmd[i], i, serial_ports[i]);
		
		if(tcgetattr(serial_ports[i], &tty[i]) != 0) {
			printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
			dados.state |= (1<<i);
		}
		
		tty[i].c_cflag &= ~PARENB; 
		tty[i].c_cflag &= ~CSTOPB; 
		tty[i].c_cflag |= CS8;  

		tty[i].c_cc[VTIME] = 10;    
		tty[i].c_cc[VMIN] = 0;
		
		if(i > 0 && i < 3){
			cfsetispeed(&tty[i], B1200);
			cfsetospeed(&tty[i], B1200);
		}
		else{
			cfsetispeed(&tty[i], B9600);
			cfsetospeed(&tty[i], B9600);
		}
		
		if (tcsetattr(serial_ports[i], TCSANOW, &tty[i]) != 0) {
			printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
			//dados.state = 1;
		}
	}
}


void get_measures(int signo){
	
	int i, j;
	char disp_temp[n_sensors][7];
	char disp_measures[n_sensors][15];
	char shear_load_measures[10];
	int disp_num_bytes[n_sensors];
	int shear_load_num_bytes;	
	//printf("Cliente da maquina enviando dados para interface ...\n");
	//printf("---------- Dados Enviados ----------\n");
	
	int disp_ports[n_sensors] = {serial_ports[1], serial_ports[2]};
	int shear_load_port = serial_ports[4];
	
	printf("Sample function\n");
	
	if(enable == 1){
		printf("Entry in function\n");
		// Increment sample number
		n_sample += 1;
		dados.sample_number = n_sample;
		//printf("Amostra nº: %d\n", dados.sample_number);
		
		while(shear_load_num_bytes < 1){
			// Command to receive load sensor data
			unsigned char cmd_load[] = {"*1B1 \r\0"};

			for(j = 0; j<strlen(cmd_load); j++){
				write(shear_load_port, cmd_load+j, 1);
			}

			// Receiving data from shear load sensor
			memset(&shear_load_measures, '\0', sizeof(shear_load_measures));
			shear_load_num_bytes = read(shear_load_port, &shear_load_measures, sizeof(shear_load_measures));
			
			if (shear_load_num_bytes < 0){
				printf("Error reading: %s", strerror(errno));
			}
		}
		// Filling structure for sending to interface
		if (shear_load_num_bytes > 1){
			dados.load[1] = atof(shear_load_measures);
			//printf("Medida sensor de carga: %.2f\n", dados.load[1]);
		}
		
		// Receiving data from displacement sensors
		for(i = 0; i < n_sensors; i++){
			while(disp_num_bytes[i] < 14){
				// Command to receive displacement sensors data
				system("echo 1 > /sys/class/gpio/gpio6/value");
				usleep(10);
				system("echo 0 > /sys/class/gpio/gpio6/value");
				
				memset(&disp_measures[i], '\0', sizeof(disp_measures[i]));
				disp_num_bytes[i] = read(disp_ports[i], &disp_measures[i], sizeof(disp_measures[i]));
				
				if (disp_num_bytes[i] < 0){
					printf("Error reading: %s", strerror(errno));
				}
			}
			
			// Filling structure for sending to interface
			if (disp_num_bytes[i] == 14) {
				// colocar parte de filtrar string
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
				//printf("Medida sensor de deslocamento: %.3f\n", dados.displacement[i]);
			}
		}
		//printf("------------------------------------\n");
		send_data_cliente_machine();
		return; 
	}
	
}


float get_normal_load_measure(){
	
	char normal_load_measures[10];
	int normal_load_num_bytes;
	
	int normal_load_port = serial_ports[nl_port];
	
	
	// Command to receive load sensor data
	unsigned char cmd_load[] = {"*1B1 \r\0"};

	for(int i = 0; i<strlen(cmd_load); i++){
		write(normal_load_port, cmd_load+i, i);
	}
	
	// Receiving data from normal load sensor
	memset(&normal_load_measures, '\0', sizeof(normal_load_measures));
	normal_load_num_bytes = read(normal_load_port, &normal_load_measures, sizeof(normal_load_measures));
	
	if (normal_load_num_bytes < 0){
		printf("Error reading: %s", strerror(errno));
	}
	// Filling structure for sending to interface
	else if (normal_load_num_bytes > 1){
		dados.load[0] = atof(normal_load_measures);
		//printf("Medida sensor de carga: %.2f\n", dados.load[0]);
	}
	
	return atof(normal_load_measures);
}

void reset_load_sensors(){
	
	system("sudo echo 1 > /sys/class/gpio/gpio23/value");
	usleep(10);
	system("sudo echo 0 > /sys/class/gpio/gpio23/value");

	return; 
	
}	

void set_pressure(int signo){
	// v1 - válvula de entrada - GPIO 16
	// v2 - válvula de saída - GPIO 20
	
	static int estado1 = 0;
	static int estado2 = 0;
	
	if(enable_press != 0){
		float normal_load = get_normal_load_measure(); 
		
		switch(estado1){
			case 0:
				if(normal_load < p_min){
					system("echo 1 > /sys/class/gpio/gpio16/value");
					estado1++;
				}
			break;
			
			case 1:
				if(normal_load > p_max1){
					system("echo 0 > /sys/class/gpio/gpio16/value");
					estado1 = 0;
				}
			break;
		}		
	}
	else{
		system("echo 0 > /sys/class/gpio/gpio16/value");
		system("echo 1 > /sys/class/gpio/gpio20/value");
	}

}

void set_high_pressure(){
	// v3 - válvula de alta pressão
	system("echo 1 > /sys/class/gpio/gpio21/value");
}

void disable_pressure(){

	system("echo 0 > /sys/class/gpio/gpio16/value");
	system("echo 0 > /sys/class/gpio/gpio20/value");
	system("echo 0 > /sys/class/gpio/gpio21/value");
}

void servo_configuration_commands(){
	
	int servo_port;
	
	servo_port = serial_ports[sm_port];
	
	// Write to serial port servo
	unsigned char cmd[] = {"CMD UCP UDM\r"};
	for(int i = 0; i<strlen(cmd); i++){
		write(servo_port, cmd+i, i);
	}

	// Receiving data from the servo
	char read_servo [30];
	memset(&read_servo, '\0', sizeof(read_servo));
	int num_bytes = read(servo_port, &read_servo, sizeof(read_servo));
	printf("Message received!\n");

	if (num_bytes < 0) {
		printf("Error reading: %s", strerror(errno));
	}

	
	return; 
}

void set_init_position(){
	
	int servo_port;
	
	servo_port = serial_ports[sm_port];
	
	// Write to serial port servo
	unsigned char cmd[] = {"MP O=0\r"};
	for(int i = 0; i<strlen(cmd); i++){
		write(servo_port, cmd+i, i);
	}

	// Receiving data from the servo
	char read_servo [30];
	memset(&read_servo, '\0', sizeof(read_servo));
	int num_bytes = read(servo_port, &read_servo, sizeof(read_servo));

	if (num_bytes < 0) {
		printf("Error reading: %s", strerror(errno));
	}

	
	return; 
}

void define_position(float vel){
	
	int servo_port;
	
	servo_port = serial_ports[sm_port];
	
	int vel_res;
	
	// Stop command
	if (vel == 0.0){
		unsigned char cmd[] = {"X\r"};
		for(int i = 0; i<strlen(cmd); i++){
			write(servo_port, cmd+i, i);
		}
	}
	else{
		// Motion command
		vel_res = lround(vel * c_vel);
		
		char cmd[50];
		snprintf(cmd, 50, "MV A=%d V=%d G\r", acel, vel_res);
		for(int i = 0; i<strlen(cmd); i++){
			write(servo_port, cmd+i, i);
		}
	}
	
	
	// Receiving data from the servo
	char read_servo [30];
	memset(&read_servo, '\0', sizeof(read_servo));
	int num_bytes = read(servo_port, &read_servo, sizeof(read_servo));

	if (num_bytes < 0) {
		printf("Error reading: %s", strerror(errno));
	}

	return;
}

void move_machine(float vel, float dist){
	
	int servo_port;
	
	servo_port = serial_ports[sm_port];
	
	int vel_res, dist_res;
	
	// Stop command
	if (vel == 0.0 && dist == 0.0){
		unsigned char cmd[] = {"X\r"};
		for(int i = 0; i<strlen(cmd); i++){
			write(servo_port, cmd+i, i);
		}
	}
	// Motion command
	else{
		vel_res = lround(vel * c_vel);
		dist_res = lround(dist * c_dist);
		
		char cmd[50];
		snprintf(cmd, 50, "A=%d V=%d P=%d G\r", acel, vel_res, dist_res);
		for(int i = 0; i<strlen(cmd); i++){
			write(servo_port, cmd+i, i);
		}
	}
	
	// Receiving data from the servo
	char read_servo [30];
	memset(&read_servo, '\0', sizeof(read_servo));
	int num_bytes = read(servo_port, &read_servo, sizeof(read_servo));

	if (num_bytes < 0) {
		printf("Error reading: %s", strerror(errno));
	}
	
	return;
}

int check_trajetory(){

	int servo_port;
	int RBt;
	
	servo_port = serial_ports[sm_port];
	
	// Write to serial port servo
	unsigned char cmd[] = {"RBt\r"};
	for(int i = 0; i<strlen(cmd); i++){
		write(servo_port, cmd+i, i);
	}

	// Receiving data from the servo
	char read_servo [5];
	memset(&read_servo, '\0', sizeof(read_servo));
	int num_bytes = read(servo_port, &read_servo, sizeof(read_servo));

	if (num_bytes < 0) {
		printf("Error reading: %s", strerror(errno));
	}

	if (num_bytes > 1) {
		RBt = read_servo[4] - '0';
		//int RBt = strtol(read_servo[0], NULL, 10);
	}	
	
	return RBt; 	
}

int check_limits(){

	int servo_port;
	int state;
	
	servo_port = serial_ports[sm_port];
	
	// Write to serial port servo
	unsigned char cmd[] = {"RBl RBr\r"};
	for(int i = 0; i<strlen(cmd); i++){
		write(servo_port, cmd+i, i);
	}

	// Receiving data from the servo
	char read_servo [10];
	memset(&read_servo, '\0', sizeof(read_servo));
	int num_bytes = read(servo_port, &read_servo, sizeof(read_servo));

	if (num_bytes < 0) {
		printf("Error reading: %s", strerror(errno));
	}

	if (num_bytes > 1) {
		//printf("Read %i bytes. Received message: %s", num_bytes, read_servo);
		
		
		int RBl = read_servo[4] - '0';
		int RBr = read_servo[10] - '0';
		
		if((RBl == 1 || RBr == 1)){
			state = 1;
			dados.state |= (1<<5);
			//PWM Buzzer - Ligado
			
		}
		else{
			state = 0;
			dados.state &= (0<<5);
		}
	}	
	
	return state; 	
}

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

/* void send_data(){

	signal(SIGALRM, get_measures);
	timer_useconds(time_sample);
	//timer_seconds(time_sample);

} */

void control_pressure(){

	signal(SIGALRM, set_pressure);
	timer_useconds(40000);
	 
	while(ligado);  
}

void check_move(){

	while(check_trajetory() == 1 && ligado == 1){
		check_limits();
		reset_buffer();
	}	
}





