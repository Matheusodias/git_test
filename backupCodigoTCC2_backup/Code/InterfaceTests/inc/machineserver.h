#ifndef MACHINESERVER_H
#define MACHINESERVER_H

#include <QThread>
#include "../Interface/inc/socket_local.h"

/**
 * @brief Classe que simula o servidor da máquina real.
 * 
 * Esta classe é uma thread e tem como objetivo auxiliar o teste do socket, simulando o 
 * recebimento de dados pela máquina real. 
 */
class machineServer : public QThread
{
    Q_OBJECT
    public:
        machineServer(QObject * parent = nullptr);
        ~machineServer();
        void init();
        char server_socket_name[100]; /*!< Nome do socket servidor da máquina. O nome deve ser igual ao do socket cliente da Interface.*/
        int  server_socket_id; /*!< Identificador do socket do servidor da máquina.*/
        int  client_socket_id; /*!< Identificador do socket do cliente aceitado pelo servidor.*/
        interface_to_machine_message interface_message; /*!< Union que contém a estrutura dos dados enviados pela interface.*/
        uint8_t errorOccurred; /*!< Sinaliza se algum erro ocorreu no construtor da classe.*/

    private:
        void run() override;
        void read_interface_message();

};

#endif // MACHINESERVER_H
