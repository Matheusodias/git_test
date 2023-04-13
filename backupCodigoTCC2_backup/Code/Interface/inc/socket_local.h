#ifndef SOCKET_LOCAL_H
#define SOCKET_LOCAL_H

#include<stdint.h>

//#define tests

#ifdef tests
 #include <QMetaType>
#endif

#define interface_payload_size 3 * sizeof(uint16_t) /*!< Tamanho do payload da mensagem enviada pela interface.*/
#define machine_payload_size 6 * sizeof(uint32_t) /*!< Tamanho do payload da mensagem enviada pela máquina.*/

/**
 * @brief Essa union tem como objetivo facilitar o envio de mensagens da interface para a máquina.
 * 
 * Utilizando a union, a transformação dos dados para string (unsigned char [] neste caso) é facilitada, pois
 * os dados ocupam a mesma região de memória que a string. Assim para enviar os dados pelo socket
 * basta enviar a variável payload.
 */
typedef union interface_to_machine_message{
    unsigned char payload[interface_payload_size]; /*!< String com os dados a serem enviados.*/
    struct {
        /**
         * @brief Identificação do comando a ser enviado.
         *
         * 0 para pressão, 1 para distância e velocidade e 2 para o período de amostragem 
         * e permitir o recebimento de dados. 
        */
        uint8_t command; 
        union {
            uint16_t pressure; /*!< Pressão da máquina desejada pelo usuário*/
            struct {
                int16_t distance; /*!< Distância que a máquina percorrerá na fase de cisalhamento.*/
                int16_t velocity;/*!< Velocidade que a máquina terá na fase de cisalhamento. */
            };
            struct {
                uint8_t enabled; /*!<Indica que a interface já pode receber os dados da máquina.*/
                uint16_t sample_period;  /*!< Período de amostragem do recebimento dos dados vindos da máquina.*/
            };
        };
    };
} interface_to_machine_message; 

/**
 * @brief Essa union tem como objetivo facilitar o envio de mensagem da máquina para a interface.
 * 
 * Utilizando a union, a transformação dos dados para string (unsigned char [] neste caso) é facilitada, pois
 * os dados ocupam a mesma região de memória que a string. Assim para enviar os dados pelo socket
 * basta enviar a variável payload.
 */
typedef union  machine_to_interface_message{
    unsigned char payload[machine_payload_size]; /*!< String com os dados a serem enviados.*/
    struct {
        uint32_t sample_number; /* Número da amostra, [0] para as amostras de adensamento e [1] para as de cisalhamento.*/
        //uint32_t date_time;/*!< Dia, hora, minuto e segundo que o essa union foi enviada para a interface.*/
        float displacement[2];/*!< Valor do deslocamento, [0] para os deslocamentos verticais de adensamento e [1] para os horizontais de cisalhamento.*/
        float load[2];/*!< Valor da carga, [0] para as cargas verticais de adensamento e [1] para as horizontais de cisalhamento.*/
        uint8_t state; /*! < Variável que guarda os estados da resposta. Utilizada para verificar se erros ocorreram. */
    };
} machine_to_interface_message;

/*
1 tabela de experimento

n amostras

1 cisalhamento e de adensamento
id do experimento
fase
sample_number
displacement[2]
load[2]
sample_number_diff

*/



#ifdef tests
Q_DECLARE_METATYPE(interface_to_machine_message);
Q_DECLARE_METATYPE(machine_to_interface_message);
#endif



// mandar só um numero de amostra e quando iniciar a fase de cisalhamento, subtrair o número de
// amostra atual pelo último numero de 
// amostra antes de iniciar o cisalhamento. Exemplo: 802 - 800 = numero de amostra 2 para cisalhamento
// número 802 para adensamento.







#endif // SOCKET_LOCAL_H
