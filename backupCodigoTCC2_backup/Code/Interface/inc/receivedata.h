#ifndef RECEIVEDATA_H
#define RECEIVEDATA_H

#include <QThread>

#include "socket_local.h"
#include "table.h"

/**
 * @brief Classe que recebe os dados da camada de controle.
 * 
 * Esta classe é responsável por adminstrar a recepção de dados,
 * enviados pela camada de controle.
 * 
 * Esta classe é uma Thread.
 */
class receiveData : public QThread
{
    Q_OBJECT
    private:
        void readClientMessage();
        void run() override;
        Table * shear_densification_variables; /*!< Receberá a instância da classe Table criada na MainWindow. */


    public:
        char server_socket_name[100]; /*!<Nome do socket, o socket cliente da camada de controle deve possuir o mesmo nome .*/
        int  server_socket_id; /*!< Identificador do socket servidor (Interface).*/
        int  client_socket_id; /*!< Identificador do socket cliente (Camada de controle).*/
        uint8_t errorOccurred; /*!< Identifica se algum erro ocorreu nas configurações iniciais.*/  
        receiveData(QObject * parent = nullptr,Table * shear_densification_variables = nullptr);
        ~receiveData();
        uint8_t initSocketServer();
        void init();
        machine_to_interface_message machine_message; /*!< Essa é a estrutura de envio de dados da camada de controle para a Interface.*/

    public slots:

    signals:
        void data_arrived();       

};

#endif // RECEIVEDATA_H
