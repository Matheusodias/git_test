#ifndef SOCKET_LOCAL_H
#define SOCKET_LOCAL_H

#include <stdint.h>
#define interface_payload_size 3*sizeof(uint16_t)

typedef union {
    unsigned char payload[interface_payload_size];
    struct {
        uint8_t command;
        union {
            uint16_t pressure;
            struct {
                int16_t distance;
                int16_t velocity;
            };
            struct {
                uint8_t enabled;
                uint16_t sample_period;
            };

        };
    };
} interface_message;

#define machine_payload_size 6*sizeof(uint32_t)

typedef union {
    unsigned char payload[machine_payload_size];
    struct {
        uint32_t sample_number;
        float displacement[2];
        float load[2];
        uint8_t state;
    };
} machine_message;

#endif

#define c_dist 317180
#define c_vel 16106000
#define acel 100
#define hist 2
#define ports 5
#define n_sensors 2
#define sm_port 0
#define nd_port 1
#define sd_port 2
#define nl_port 3
#define sl_port 4
#define PIN RPI_GPIO_P1_12
#define PWM_CHANNEL 0
#define RANGE 1024
#define fator 1.2

extern char socket_name_cliente[20];
extern int socket_id_cliente;
extern char socket_name_servidor[20];
extern int  socket_id_servidor;
extern int  id_socket_cliente;

extern machine_message dados;
extern int serial_ports[ports];
extern int ligado;
extern int enable;
extern int time_sample;
extern int n_sample;
extern float pressure;

extern pthread_t thread_controle_pressao;
extern pthread_t thread_checa_movimento;

int connection_cliente_machine();
void send_data_cliente_machine();
int servidor_machine();
void received_command();
void end_server(int signum);
void define_command (int client_socket);
void serial_port_configuration();
void get_measures(int signo);
float get_normal_load_measure();
void reset_load_sensors();
void set_pressure(int signo);
void set_high_pressure();
void disable_pressure();
void servo_configuration_commands();
void set_init_position();
void define_position(float vel);
void move_machine(float vel, float dist);
int check_trajetory();
int check_limits();
void reset_buffer();
void set_buzzer();
void timer_useconds(long int useconds);
void timer_seconds(long int seconds);
void send_data();
void control_pressure();
void check_move();





