#include <stdio.h>
#include <wiringPi.h>

#define PWM_PIN 1
#define PWM_RANGE 1023

int main (void) {
    int pin, duty_cycle;

    // Inicializa a biblioteca WiringPi
    wiringPiSetupGpio();
    
	printf("Digite o número do pino (GPIO): ");
    scanf("%d", &pin);
    // Configura o pino como saída PWM
    pinMode(pin, PWM_OUTPUT);

    // Define a frequência do PWM
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(PWM_RANGE);
    pwmSetClock(384);

    // Loop principal
    while (1) {
        // Lê o número do pino e o valor do ciclo útil
        
        printf("Digite o valor do ciclo útil (0 a 1023): ");
        scanf("%d", &duty_cycle);

        // Configura o pino e o ciclo útil do PWM
        pwmWrite(pin, duty_cycle);
    }

    return 0;
}
