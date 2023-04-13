#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include "Interface/inc/socket_local.h"
#include "functions.h"

void * enviaDados();
void createServer();
void end_server();
void print_client_message(int client_socket);
void setLowPressure();
void setVelocity(int vel_mv);
void get_measure_control(int signo);
void get_load_measure();

// Velocity Mode Variables
FILE *shear_disp_measures;
int serial_ports[servo_ports];
int enabled_vel = 0;

char server_socket_name[] = "./machine";
char client_socket_name[] = "./interface";
int  server_socket_id, id_socket_cliente;
int client_socket_id;
int keepThreading = 1;
int keepWriting = 1;
int sample_number=0;
int first_time=1;
int lowpressure = 0;
pthread_t thread_le_comandos;
pthread_mutex_t lock;

struct timespec sample_time;
float velocity=0;


int main(){

    signal(SIGINT,end_server);
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    pthread_mutex_lock(&lock);
    

    createServer();
    pthread_join(thread_le_comandos,NULL);
    pthread_mutex_destroy(&lock);
    return 0;

}

void createServer()
{
    
    struct sockaddr receive_data;
    server_socket_id = socket(PF_LOCAL, SOCK_STREAM, 0);
   

    receive_data.sa_family = AF_LOCAL;
    strncpy(receive_data.sa_data, server_socket_name,strlen(server_socket_name)+1);

    uint8_t socket_size = strlen(receive_data.sa_data) + sizeof(receive_data.sa_family);

    if(bind(server_socket_id, &receive_data, socket_size)!=0)
    {
        printf("Erro no bind do server socket id %s\n",strerror(errno));     
        return;
    }

    //qDebug() << "Nome do socket = " << receive_data.sa_data;
    if(listen(server_socket_id,2)!=0)
    {
        printf("Error in server listen: %s\n",strerror(errno));
        return ;
    }


    while(keepThreading){
        struct sockaddr clienteAddr;
        socklen_t clienteLength = sizeof(( struct sockaddr *) & clienteAddr);
        printf("Antes do accept machine\n");
        id_socket_cliente = accept(server_socket_id, &clienteAddr, &clienteLength);
        if(id_socket_cliente<0)
        {
            
            fprintf(stderr,"Error no accept do servidor machine: %s\n",strerror(errno));
            end_server(SIGINT);
            return;
        }
        printf("Depois do accept machine\n");
        print_client_message(id_socket_cliente);
    }
    printf("Terminou a run machine\n");
}

void print_client_message(int client_socket)
{


    while(keepThreading){
        
        interface_to_machine_message message;
        int value = read(client_socket,&message.payload,interface_payload_size);
        if(value<=0){
            break;
        };
        
        printf("Comando %d\n",message.command);
        switch (message.command)
        {
            case 0: // comando 0 iniciar recebimento de dados da máquina (enabled, sample_period);
                if(message.enabled == 1){
                    sample_time.tv_sec = message.sample_period/1000;
                    sample_time.tv_nsec = (message.sample_period%1000)*1000000;
          //          printf("liberei a thread\n");
                    if(first_time){
                        pthread_create(&thread_le_comandos,NULL,&enviaDados, NULL);
                        first_time=0;
                    }
                    pthread_mutex_unlock(&lock);
                    
                } else{
                    //printf("tentando pegar o lock\n");
                    pthread_mutex_lock(&lock);  
                    //printf("tentando pegar o lock2\n");
                }
                
                printf("Enabled = %d Sample Period= %d\n",message.enabled,message.sample_period);

            break;

            case 1: // comando 1 movimentar motor com velocidade constante (velocidade com sinal positivo(direita), negativo(esquerda))
                printf("Velocity = %d\n",message.velocity);
                //velocity = message.velocity;
                setVelocity(message.velocity);

            break;

            case 2: // comando 2 colocar uma pressão baixa, para fixar a amostra lowPressure(ligado).
                printf("Low pressure On\n");
                setLowPressure();
            break;
            case 3: // comando 3 setLoad(pressure); Experimento inicia
                printf("Pressure = %d\n",message.pressure);
            break;
            case 4: // comando 4 setDistance_Velocity(distance,velocity) . Cisalhamento
                printf("Distance = %d Velocity = %d\n",message.distance,message.velocity);
            break;


            case 5: // comando 5 0(disabled, 0), 2(desligado) 4(distance=0,velocity=0);interromper/finalizar experimento desliga pressão
                printf("End machine\n");
                fclose(shear_disp_measures);
                stop_move(serial_ports);
                //printf("tentando pegar o lock3\n");
                    sample_number=0;
                    pthread_mutex_lock(&lock);  
                //printf("tentando pegar o lock4\n");
                
                // end_server(SIGINT);
            break;
            default:
                break;
        }
        
    }
    fprintf(stderr,"Erro no read\n");
    
    end_server(2);
	return;
}

void * enviaDados(){
    struct sockaddr name;
    memset(&name,0,sizeof(name));
    
    client_socket_id = socket(PF_LOCAL, SOCK_STREAM,0);
    name.sa_family = AF_LOCAL;

    strncpy(name.sa_data,client_socket_name,strlen(client_socket_name)+1);
    
    //printf("Nome = %s\n%s\n",name.sa_data,client_socket_name);
    //printf("Tamanho = %ld\n",strlen(name.sa_data) + 2);

    if(connect(client_socket_id, &name, strlen(name.sa_data) + sizeof(name.sa_family))!=0){
        printf("The last error message is: %s\n", strerror(errno));
        close(client_socket_id);
        return 0;
    }
    int i=0,plus=1;
    
    int direcao = 1;
    while(keepWriting){
        pthread_mutex_lock(&lock);
        
        int option = 0;
        //scanf("%d",&option);
        machine_to_interface_message dados;
        dados.sample_number = sample_number++;
        dados.displacement[0] = 14.20 + i%1000;
        dados.displacement[1] = 28.2 + i%1000;
        dados.load[0] = 100.2 + i%1000;        
        dados.load[1] = 200.2 + i%1000;
        dados.state = (1 << (i%8));   

        //printf("displacement = %lf\n",dados.displacement[0]);
        
        if(write(client_socket_id, dados.payload, sizeof(machine_to_interface_message))<=0){
            break;
        };
        
        if(direcao==1 && velocity<0){
            direcao = 0;
            plus = -1;
            
        } else if(direcao==0 && velocity>0) {
            direcao = 1;
            plus = 1; 
        }
        i+=plus;
        pthread_mutex_unlock(&lock);
        nanosleep(&sample_time, NULL);
    }
    close(client_socket_id);
    //printf("passei aqui");
    pthread_exit(0);
}

void end_server(int signum)
{
	fprintf(stderr,"Fechando máquina %d\n", signum);
    keepThreading=0;
    keepWriting=0;
    // pthread_mutex_unlock(&lock);
    // pthread_mutex_destroy(&lock);
    // pthread_cancel(thread_le_comandos);
    // fprintf(stderr,"Depois de fechar a thread %d\n", signum);
    pthread_join(thread_le_comandos,NULL);
    fprintf(stderr,"Depois de fechar a thread %d\n", signum);

    fclose(shear_disp_measures);

	unlink(server_socket_name);
    //unlink(client_socket_name);
	close(server_socket_id);
    close(id_socket_cliente);
    close(client_socket_id);
	exit(0);
}

void setLowPressure(){
    if(lowpressure == 0){
        //ativa valvula de suprimento
        system("echo 1 > /sys/class/gpio/gpio16/value");

        //ativa valvula de escape
        system("echo 1 > /sys/class/gpio/gpio20/value");
        printf("Acionou setVelocity\n");
        setVelocity(1);
        lowpressure = 1;
    } else if(lowpressure == 1){
        //desativa valvula de suprimento
        system("echo 0 > /sys/class/gpio/gpio16/value");

        //desativa valvula de escape
        system("echo 0 > /sys/class/gpio/gpio20/value");
        //setVelocity(0);
        printf("Acionou stop_move\n");
        stop_move(serial_ports);
        enabled_vel = 0;
        lowpressure = 0;
    }
}

void setVelocity(int vel_mv){

    servo_serial_port_configuration(serial_ports);
    printf("Passou do servo_serial_port_configuration\n");
	configuration_servo(serial_ports);
	printf("Passou do configuration_servo\n");

	shear_disp_measures = fopen("shear_disp_measures.csv", "w");
	if(shear_disp_measures == NULL){
		printf("Erro na abertura do arquivo!");
		return 1;
	}

    signal(SIGALRM, get_measure_control);
	timer_useconds(250000);
	printf("Acionou velocity_mode\n");
    velocity_mode(vel_mv, serial_ports);
	enabled_vel = 1;

}

// Get Measures if(enabled_vel == 1)
void get_measure_control(int signo){
	if(enabled_vel == 1){
		get_load_measure();
	}
}

// Get Normal disp Measurement
void get_load_measure(){
	
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
