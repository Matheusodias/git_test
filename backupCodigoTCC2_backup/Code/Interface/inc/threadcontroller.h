#ifndef THREADCONTROLLER_H
#define THREADCONTROLLER_H

#include <QObject>
#include <QThread>

#include "receivedata.h"
#include "table.h"


/**
 * @brief Classe que controla a thread criada por receiveData.
 * 
 * Esta classe controla a criação e destruição da Thread receiveData.
 * 
 */
class ThreadController : public QObject
{
    Q_OBJECT
    private: 
        bool isThreadDestroyed; /*!< Informa se a thread foi destruída ou não.*/
    public:
        receiveData *receiveDataThread; /*!< Instância da classe receiveData(Thread).*/
        ThreadController(QObject *parent = nullptr,Table * shear_densification_variables = nullptr);
        ~ThreadController();
    public slots:
        void receiveThreadFinishedSlot();
    signals:
};

#endif // THREADCONTROLLER_H
