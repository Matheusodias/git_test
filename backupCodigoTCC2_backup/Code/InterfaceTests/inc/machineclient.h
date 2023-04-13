#ifndef MACHINECLIENT_H
#define MACHINECLIENT_H
#include<stdint.h>
/**
* \brief Classe que simula o envio de dados da máquina real.
*
* Esta classe tem como objetivo auxiliar o teste do socket, simulando o envio de dados
* da máquina real.
*/
class machineClient
{
    public:
        machineClient();
        ~machineClient();
        void sendMessages(int32_t i);
    private:
        /**
        * @brief Nome do socket local. 
        * 
        * Deve possuir o mesmo nome do socket aberto pelo servidor da interface na classe receiveData.
        */
        char socket_name[200];
        /**
         * @brief Identificador do socket local criado na função socket().
         * 
         * Um exemplo: this->socket_id = socket(PF_LOCAL, SOCK_STREAM,0).
         */
        int  socket_id;
        /**
         * @brief Identifica se ocorreu um erro na conexão com o servidor da interface.
         * 
         * O valor 1 significa que um erro ocorreu e que deve ocorrer uma nova tentativa de reconexão.
         * O valor 0 significa que não houve erro.
         */
        uint8_t errorOccurred;  



};

#endif // MACHINECLIENT_H
