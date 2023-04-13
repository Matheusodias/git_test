#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <time.h>
#include <signal.h>
#include "machine_functions.h"


int main(){
	printf("Starting machine!\n");
	signal(SIGALRM, get_measures);
	n_sample = 0;
	dados.state = 0;
	
	printf("Setting global variables ...\n");
	
	reset_load_sensors();
	printf("Resetting sensors ...\n");
	serial_port_configuration();
	printf("Configuring serial ports ...\n");
	servo_configuration_commands();
	printf("Setting up servo motor ...\n");
	
	servidor_machine();
	
	while(connection_cliente_machine() != 0){
		printf("Connecting socket machine - interface ...");
		usleep(500000);
	}
	


	received_command();
}


