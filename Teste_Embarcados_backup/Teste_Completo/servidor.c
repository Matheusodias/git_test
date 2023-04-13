#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "socket_local_MI.h"

void print_client_message(int client_socket);
void end_server(int signum);

char socket_name[] = "/home/infralab/backupCodigoTCC2/Code/interface";
int  socket_id, id_socket_cliente;
FILE *data_received;

int main(){

    struct sockaddr socket_struct;
    signal(SIGINT,end_server);
	
	data_received = fopen("data_received.txt", "w");
	
    socket_id = socket(PF_LOCAL, SOCK_STREAM, 0);
    socket_struct.sa_family = AF_LOCAL;
    strncpy(socket_struct.sa_data, socket_name, strlen(socket_name));
    bind(socket_id, &socket_struct, sizeof(socket_struct));
    printf("Nome do socket = %s\n",socket_struct.sa_data);
    listen(socket_id,10);
    

    while(1)
    {
        struct sockaddr clienteAddr;
        socklen_t clienteLength = sizeof(( struct sockaddr *) & clienteAddr);
        //printf("antes do accept\n");
        id_socket_cliente = accept(socket_id, &clienteAddr, &clienteLength);
        //printf("depois do accept\n");
        print_client_message(id_socket_cliente);
    
    }
}


void print_client_message(int client_socket)
{
	machine_message message;
	
    while(1){
        
        if(read(client_socket,&message.payload,sizeof(machine_message))<=0){
            break;
        };
		
		printf("----- Received Data ------\n");
		fprintf(data_received, "----- Received Data ------\n");
		printf("Sample Number = %d\n", message.sample_number);
		fprintf(data_received, "Sample number: %d\n", message.sample_number);
        for(int j=0;j<2;j++){
			printf("Displacement %d = %.3f\n",j,message.displacement[j]);
			fprintf(data_received, "Displacement %d = %.3f\n", j, message.displacement[j]);
			printf("Load %d = %.2f\n",j,message.load[j]);
			fprintf(data_received, "Load %d = %.2f\n", j, message.load[j]);
        }
        printf("--------------------------\n");
		fprintf(data_received, "--------------------------\n");
    }

	return;
}

void end_server(int signum)
{
	fprintf(stderr,"Fechando servidor local %d\n", signum);
	unlink(socket_name);
	fclose(data_received);
	close(socket_id);
    close(id_socket_cliente);
	exit(0);
}
