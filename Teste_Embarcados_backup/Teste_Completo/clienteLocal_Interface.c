#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "socket_local_IM.h"
void end_server(int signum);

char socket_name[] = "./server_inter";
int socket_id;

int main() {

    signal(SIGINT, end_server);
    
    
    struct sockaddr name;
    memset(&name,0,sizeof(name));
    
    socket_id = socket(PF_LOCAL, SOCK_STREAM,0);
    name.sa_family = AF_LOCAL;

    strncpy(name.sa_data,socket_name,strlen(socket_name)+1);
    
    printf("%s\n%s\n",name.sa_data,socket_name);
    printf("%ld\n",strlen(name.sa_data) + 2);

    if(connect(socket_id, &name, strlen(name.sa_data) + sizeof(name.sa_family))!=0){
        printf("The last error message is: %s\n", strerror(errno));
        close(socket_id);
        return 0;
    }

    while(1){
        printf("Digite o Comando a ser enviado:\n"
				"0 - Iniciar recebimento de dados\n"
                "1 - Ajustar posição inicial\n"
                "2 - Ativar baixa pressão\n"
                "3 - Iniciar adensamento\n"
                "4 - Iniciar cisalhamento\n"
                "5 - Interromper/Encerrar experimento\n");
        int option = 6;
        scanf("%d",&option);
        union_message dados;
        dados.command = option;
       switch (option)
       {
            case 0: 
				dados.enabled_data = 1;
				dados.sample_period = 1;
				printf("Recebimento de dados habilitado com período de amostragem de %d ms.\n", dados.sample_period);
            break;

            case 1:
				printf("Aperte 1 para iniciar movimento e 0 para parar o movimento: ");
				int escolha = 5;
				scanf("%d", &escolha);
				if(escolha == 1){
					dados.enabled_pos = 1;
					dados.velocity = 2;
					printf("Movimento de ajuste habilitado com velocidade constante de %.3f mm/s.\n",((float)dados.velocity/1000));
				}
				else{
					dados.enabled_pos = 0;
					dados.velocity = 0;
					printf("Movimento de ajuste encerrado.\n");
				}
            break;
			
			case 2:
				dados.pressure = 20;
                printf("Pressão utilizada no experimento = %d Psi\n",dados.pressure);
			break;
			
			case 3:
				printf("Ativação da alta pressão para inicio do experimento.\n");
			break;
			
			case 4:
				dados.distance = 20;
				dados.velocity = 1;
				printf("Movimento de cisalhamento com distancia de %.2f mm e velocidade %.3f de mm/s\n",(float)dados.distance, ((float)dados.velocity/1000));
			break;
			
			case 5:
				printf("Encerrando o experimento!");
				end_server(SIGINT);
			
			break;
            
            default:
                break;
       }
  
        printf("Tamanho do payload= %ld\n",sizeof(union_message));
        write(socket_id, dados.payload, sizeof(union_message));
        
    }
    close(socket_id);

    return 0;
}


void end_server(int signum)
{
    printf("Sinal = %d\n",signum);
	fprintf(stderr,"Fechando cliente interface!\n");
	close(socket_id);
	exit(0);
}
