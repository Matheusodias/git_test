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

void reset_load_sensors();

int main(){
	reset_load_sensors();
}

void reset_load_sensors(){
	system("echo 1 > /sys/class/gpio/gpio23/value");
	usleep(100000);
	system("echo 0 > /sys/class/gpio/gpio23/value");
	return; 
	
}	