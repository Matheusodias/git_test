#include "inc/machineclient.h"

#include <QDebug>
#include <errno.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h> 
#include "../Interface/inc/socket_local.h"


/**
 * @brief Construtor da classe machineClient.
 * 
 * Este construtor cria uma conexão socket com o servidor da interface, o qual é aberto
 * pela classe receiveData. Lembrando que a classe ThreadController que cria
 * objetos da classe receiveData.
 * 
 */
machineClient::machineClient()
{
    char temp_socket_name[] = "./interface";
    strncpy(this->socket_name,temp_socket_name,strlen(temp_socket_name)+1);
    struct sockaddr name;
    memset(&name,0,sizeof(name));

    this->socket_id = socket(PF_LOCAL, SOCK_STREAM,0);
    name.sa_family = AF_LOCAL;

    strncpy(name.sa_data,socket_name,strlen(this->socket_name)+1);

//    qDebug() << name.sa_data;
//    qDebug() << this->socket_name;
   

    if(connect(this->socket_id, &name, strlen(name.sa_data) + sizeof(name.sa_family))!=0){
        qDebug() << "The last error message is: " << strerror(errno);
        this->errorOccurred=1;
        close(this->socket_id);
        return;
    }
    this->errorOccurred=0;
    qDebug() << "Cliente se conectou";
}
/**
 * @brief Destrutor da classe machineClient.
 * 
 * Este destrutor encerra a conexão com o servidor da interface, fechando o socket.
 * Isto é feito na função close(this->socket_id);
 */
machineClient::~machineClient()
{
	qDebug() << "Fechando cliente da maquina local";
    close(this->socket_id);
}

/**
 * @brief Envia mensagens para o servidor da interface caso a conexão no construtor tenha sucesso.
 * 
 * Esta função envia mensagens para o servidor da interface, simulando o comportamento da máquina.
 * 
 * @param i Valor utilizado para alterar o conteúdo da mensagem, e permitir assim, uma
 * maior variedade de testes.
*/
void machineClient::sendMessages(int32_t i)
{
    if(this->errorOccurred){
        qDebug() << "Ocorreu um erro anteriormente ao tentar se conectar ao servidor da Interface."; 
        qDebug() << "Por favor tente novamente se conectar ao servidor e depois tente mandar mensagens";
        return;
    }
    machine_to_interface_message dados;
    dados.sample_number = 10 + i;
    //dados.sample_number[1] = 20 + i;
    //dados.date_time = 30 + i;
    //dados.date_time[1] = 40 + i;
    dados.displacement[0] = 50.20 + i;
    dados.displacement[1] = 60.2 + i;
    dados.load[0] = 70.2 + i;        
    dados.load[1] = 80.2 + i;
    dados.state = 1;

    int value = write(socket_id, dados.payload, sizeof(machine_to_interface_message));
    if(value <=0){
        qDebug("Erro no envio");
    }
}
