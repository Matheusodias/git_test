#include <QtTest>
#include <QCoreApplication>

// add necessary includes here
#include <QDebug>
#include "inc/machineclient.h"
#include "../Interface/inc/threadcontroller.h"
#include "../Interface/inc/socket_local.h"
#include "../Interface/inc/sendcommands.h"
#include "../Interface/inc/experiment.h"
#include "../Interface/inc/dbmanager.h"
#include "../Interface/inc/densification.h"
#include "../Interface/inc/shear.h"
#include "../Interface/inc/field.h"
#include "../Interface/inc/table.h"
#include "inc/machineserver.h"
#include <unistd.h>
#include <QTableWidget>
#define tests

/**
 * @brief Classe de testes para o socket.
 * 
 * Esta classe testa se o envio e recebimento das mensagens através 
 * do socket local funciona.
 * 
 * 
*/
class SocketTests : public QObject
{
    Q_OBJECT

public:
    
    SocketTests();
    ~SocketTests();
private:
    /**
     * @brief A classe ThreadController cria o socket servidor que irá aguardar a conexão do cliente da Máquina.
     * 
     * Instância um objeto da classe ThreadController, responsável por criar um objeto da classe receiveData (Thread). Fazendo com que a
     * interface e o serviço dos sockets rodem em paralelo.
     */
    ThreadController *receiveDataFromMachine;
    /**
     * @brief A classe machineServer criar o socket servidor que irá aguardar a conexão do cliente da Interface.
     * 
     * Instância um objeto da classe machine server. Este objeto inicia uma thread, garantindo que o servidor da Máquina
     * e o cliente da Interface rodem em paralelo.
     */
    machineServer *receiveDataFromInterface;
    void compareStructIntElements(int result, int expected);
    void compareStructFloatElements(float result, float expected);

    Experiment * info_variables;
    Field * setupFields;
    Table *tables;
    sendCommands test_sendData;
    machineClient * test_sendDataMachine;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_receiveDataFromMachine();
    void test_receiveDataFromMachine_data();
    void test_receiveDataFromInterface();
    void test_receiveDataFromInterface_data();


};
/**
 * @brief Contrutor da classe de testes do socket.
 * Pode ser utilizado para instanciar classes, diferentes da atual, ou para configurações iniciais.
*/
SocketTests::SocketTests()
{
    QTableWidget * ShearTable = new QTableWidget();
    QTableWidget * DensificationTable = new QTableWidget();

    this->info_variables = new Experiment();
    this->setupFields = new Field(this->info_variables);
    this->tables = new Table(this->info_variables,ShearTable,DensificationTable);


    this->receiveDataFromMachine =new ThreadController(NULL,this->tables);
    this->receiveDataFromInterface = new machineServer();
    this->receiveDataFromInterface->init();

    for(int i=0;i<20;i++){
        if(!test_sendData.connectToMachine()){
            QThread::msleep(500);
        } else{
          break;
        }
    }
    test_sendDataMachine = new machineClient();

    qDebug() << "Cria instância do socket test";
}

/**
 * @brief Destrutor da classe de testes do socket.
 * 
 * No destrutor é eliminada a instância receiveDataFromMachine da ThreadController.
*/

SocketTests::~SocketTests()
{
    qDebug() << "Destrói instância do socket test";
    delete this->receiveDataFromMachine;
    if(this->receiveDataFromInterface->isRunning()){
        qDebug("Controller terminou antes");
        this->receiveDataFromInterface->quit();
        this->receiveDataFromInterface->wait(1);
        // a thread não consegue sair no quit com o accept, mas no read sim.
        // Caso demore mais de 1 segundo para sair, pode dar terminate na thread.
        if(this->receiveDataFromInterface->isRunning())
        {
            this->receiveDataFromInterface->terminate();
            this->receiveDataFromInterface->wait();
        }
    }
    delete this->receiveDataFromInterface;
}

/**
* \brief Primeiro teste.
* 
* Teste que roda antes de todos os outros testes. Pode ser utilizado para alguma configuração inicial.
*/
void SocketTests::initTestCase()
{
    qDebug() << "Teste antes dos socket test";
}

/**
 * @brief Último teste.
 * 
 * Teste que roda ao final de todos os outros testes. Pode ser utilizado para alguma configuração final dos dados.
 * Como liberação de memória ou algum outro pós-processamento.
 */
void SocketTests::cleanupTestCase()
{
    qDebug() << "Teste depois dos socket test";
}

/**
 * @brief Teste que verifica se a interface está recebendo corretamente os dados da máquina.
 * 
 * Este teste é uma simulação, não é utilizada a máquina real para este teste.
 */
void SocketTests::test_receiveDataFromMachine()
{


    QFETCH(int, number);
    QFETCH(machine_to_interface_message, message);

    test_sendDataMachine->sendMessages(number);
    //Tempo mínimo de intervalo entre um envio e outro
    QThread::msleep(5);

    compareStructIntElements(receiveDataFromMachine->receiveDataThread->machine_message.sample_number,message.sample_number);
    //compareStructIntElements(receiveDataFromMachine->receiveDataThread->machine_message.sample_number[1],message.sample_number[1]);
    //compareStructIntElements(receiveDataFromMachine->receiveDataThread->machine_message.date_time,message.date_time);
    //compareStructIntElements(receiveDataFromMachine->receiveDataThread->machine_message.date_time[1],message.date_time[1]);
    compareStructFloatElements(receiveDataFromMachine->receiveDataThread->machine_message.displacement[0],message.displacement[0]);
    compareStructFloatElements(receiveDataFromMachine->receiveDataThread->machine_message.displacement[1],message.displacement[1]);
    compareStructFloatElements(receiveDataFromMachine->receiveDataThread->machine_message.load[0],message.load[0]);
    compareStructFloatElements(receiveDataFromMachine->receiveDataThread->machine_message.load[1],message.load[1]);
    compareStructIntElements(receiveDataFromMachine->receiveDataThread->machine_message.state,message.state);
}

/**
 * @brief Função que possui os dados necessários para realizar o teste test_receiveDataFromMachine.
 * 
 * Funções que terminam com _data nos testes, tem a função de fornecerem os dados para o teste ser executado.
 * Exemplo: test_receiveDataFromMachine e   test_receiveDataFromMachine_data.
 */
void SocketTests::test_receiveDataFromMachine_data()
{
    QTest::addColumn<int>("number");
    QTest::addColumn<machine_to_interface_message>("message");
    
    machine_to_interface_message test_message[10000];
    for(int i=0,j=0;i<10000;i++,j++){
        test_message[j].sample_number = 10 + i;
        //test_message[j].sample_number[1] = 20 + i;
        //test_message[j].date_time = 30 + i;
        //test_message[j].date_time[1] = 40 + i;
        test_message[j].displacement[0] = 50.2 + i;
        test_message[j].displacement[1] = 60.2 + i;;
        test_message[j].load[0] = 70.2 + i;
        test_message[j].load[1] = 80.2 + i;
        test_message[j].state = 1;

        QString test_name_temp = QString("Send %1").arg(j);
        std::string str = test_name_temp.toStdString();
        const char* test_name = str.c_str();
        QTest::newRow(test_name) << i << test_message[j];
    }


}
/**
 * @brief Teste que verifica se a interface está recebendo corretamente os dados da máquina.
 * 
 * Este teste é uma simulação, não é utilizada a máquina real para este teste.
 */
void SocketTests::test_receiveDataFromInterface()
{

    QFETCH(interface_to_machine_message, message_interface);

    test_sendData.setCommand(message_interface.command);
    switch (message_interface.command)
    {
        case 1:
            test_sendData.setPressure(message_interface.pressure);
        break;
        case 2:
            test_sendData.setVelocity(message_interface.velocity);
            test_sendData.setDistance(message_interface.distance);
        break;    
        case 3:
            test_sendData.setEnabled(message_interface.enabled);
            test_sendData.setSamplingPeriod(message_interface.sample_period);
        break;
        default:
            qDebug() << "Invalid Command";
        break;
    }
   
    test_sendData.sendMessage();
    //Tempo mínimo de intervalo entre um envio e outro
    QThread::msleep(5);

    compareStructIntElements(receiveDataFromInterface->interface_message.command,message_interface.command);
    //qDebug() << "Command = " << receiveDataFromInterface->interface_message.command << " " <<message_interface.command ;
    if(message_interface.command==1) {
        compareStructIntElements(receiveDataFromInterface->interface_message.pressure,message_interface.pressure);
        //qDebug() << "Pressure = " << receiveDataFromInterface->interface_message.pressure << " " << message_interface.pressure;
    }else if(message_interface.command==2){
        compareStructIntElements(receiveDataFromInterface->interface_message.distance,message_interface.distance);
        compareStructIntElements(receiveDataFromInterface->interface_message.velocity,message_interface.velocity);
        //qDebug() << "Distance = " << receiveDataFromInterface->interface_message.distance << " " << message_interface.distance;
        //qDebug() << "Velocity = " << receiveDataFromInterface->interface_message.velocity << " " << message_interface.velocity;
    } else if(message_interface.command==3) {
        compareStructIntElements(receiveDataFromInterface->interface_message.enabled,message_interface.enabled);
        compareStructIntElements(receiveDataFromInterface->interface_message.sample_period,message_interface.sample_period);
        //qDebug() << "Enabled = " << receiveDataFromInterface->interface_message.enabled << " " << message_interface.enabled;
        //qDebug() << "Sample_period = " << receiveDataFromInterface->interface_message.sample_period << " " << message_interface.sample_period;

    }
   
}
/**
 * @brief Função que possui os dados necessários para realizar o teste test_receiveDataFromInterface.
 * 
 * Funções que terminam com _data nos testes, tem a função de fornecerem os dados para o teste ser executado.
 * Exemplo: test_receiveDataFromInterface e   test_receiveDataFromInterface_data.
 * 
 */
void SocketTests::test_receiveDataFromInterface_data()
{

    QTest::addColumn<interface_to_machine_message>("message_interface");

    interface_to_machine_message test_message[10000];
    for(int i=0,j=0;i<10000;i++,j++){
           test_message[i].command= i%3 + 1;
           switch (test_message[i].command)
           {
               case 1:
                    test_message[i].pressure = 1;
               break;
               case 2:
                   test_message[i].distance = 2;
                   test_message[i].velocity = 3;
               break;
               case 3:
                   test_message[i].enabled = i&1;
                   test_message[i].sample_period = 10;
               break;
               default:
                   qDebug() << "Invalid Command";
               break;
           }

        QString test_name_temp = QString("Receive %1").arg(j);
        std::string str = test_name_temp.toStdString();
        const char* test_name = str.c_str();
        QTest::newRow(test_name) << test_message[j];
    }


}


/**
 * @brief Compara a variável result com  a expected.
 * 
 * Esta função realiza uma comparação entre os 2 parâmetros do tipo int e apresenta um log
 * caso haja diferenças.
 * Ela foi criada para permitir diversas comparações em apenas 1 teste.
 * 
 * @param result Resultado do envio de dados através do socket.
 * @param expected Valor experado que chegue através do socket.
 */
void SocketTests::compareStructIntElements(int result, int expected)
{   
    QCOMPARE(result,expected);
}
/**
 * @brief Compara a variável result com  a expected.
 * 
 * Esta função realiza uma comparação entre os 2 parâmetros do tipo float e apresenta um log
 * caso haja diferenças.
 * Ela foi criada para permitir diversas comparações em apenas 1 teste.
 * 
 * @param result Resultado do envio de dados através do socket.
 * @param expected Valor experado que chegue através do socket.
 */
void SocketTests::compareStructFloatElements(float result, float expected)
{
    QCOMPARE(result,expected);
}

QTEST_MAIN(SocketTests)

#include "tst_sockettests.moc"
