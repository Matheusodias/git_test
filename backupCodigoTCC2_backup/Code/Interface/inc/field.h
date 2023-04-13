#ifndef FIELD_H
#define FIELD_H

#include <QObject>
#include <QGridLayout>
#include "experiment.h"
#include <QLabel>
#include <QLineEdit>
/**
 * @brief Classe dos campos de texto.
 * 
 * Utilizada para estilizar os campos de texto.
 * 
 */
class Field : public QObject
{
    Q_OBJECT
public:
    Field(Experiment *parent = nullptr);
    void customizeField(QLayout * gridLayout);
    void customizeOneField(QLabel *label, QLineEdit *lineEdit);
    void clearFields(QLayout * gridLayout);
    Experiment * info_variables; /*!< Variável da classe Experiment que receberá a instância da classe MainWindow.*/

private:
    QSize FieldMaximumSize[10];  /*!< Array com os valores máximos de tamanho que o campo de texto pode ter. */
    QSize FieldMinimumSize[10]; /*!< Array com os valores mínimos de tamanho que o campo de texto pode ter. */

signals:

public slots:
    void setVariables();


};

#endif // FIELD_H
