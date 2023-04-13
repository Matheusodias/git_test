#include "inc/exportdata.h"
#include <QDebug>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QTextStream>
#include <string.h>

/**
 * @brief Constrói uma nova instância da classe exportData.
 * 
 * @param parent Classe pai
 */
exportData::exportData(QObject *parent)
    : QObject{parent}
{

}

/**
 * @brief Essa função exporta o arquivo csv para o Github.
 * 
 * Esse slot é disparado quando o botão exportar recebe o
 * sinal clicked.
 * 
 * @param table_widget Tabela com os dados 
 * @param file_identification Número do experimento. Utilizado para nomear o arquivo.
 * @param option Nome do arquivo, baseado na tabela escolhida.
 */
void exportData::exportCSV(QTableWidget *table_widget, int file_identification, int option)
{
    QString file_names[] = {
        "informação",
        "tabelaAdensamento",
        "resultadoAdensamento",
        "tabelaCisalhamento",
        "resultadoCisalhamento"
    };




     QString filename = QString("./csv_files/experimento%1_%2.csv").arg(file_identification).arg(file_names[option]);
     write(table_widget,filename);
     char add[100] = "git add ";
     QByteArray ba_filename = filename.toLocal8Bit();
     const char *c_filename = ba_filename.data();

     strncat(add, c_filename, strlen(c_filename));

     //qDebug() << add;
     system(add);
     char commit[150] = "git commit -m \"Add file ";
     QString chosenfile = QString("experimento%1_%2.csv").arg(file_identification).arg(file_names[option]) + "\"";
     QByteArray ba_commit = chosenfile.toLocal8Bit();
     const char *c_commit = ba_commit.data();
     strncat(commit, c_commit, strlen(c_commit));
     //qDebug() << commit;
     char push[] = "git push origin main";
     system(commit);
     system(push);
}

/**
 * @brief Escreve os dados de table_widget em um arquivo csv.
 * 
 * @param table_widget Tabela com os dados.
 * @param filename  Nome do arquivo.
 */
void exportData::write(QTableWidget *table_widget,QString filename)
{
    QFile file(filename);
    // Trying to open in WriteOnly and Text mode
    if(!file.open(QFile::WriteOnly |
                  QFile::Text))
    {
        qDebug() << " Could not open file for writing";
        return;
    }

    // To write text, we use operator<<(),
    // which is overloaded to take
    // a QTextStream on the left
    // and data types (including QString) on the right

//    qDebug() << "Quantidade de linhas" << table_widget->rowCount();
//    qDebug() << "Quantidade de colunas" << table_widget->columnCount();


//    qDebug() << table_widget->horizontalHeaderItem(0)->text();
//    qDebug() << table_widget->horizontalHeaderItem(1)->text();


    QTextStream out(&file);


    for(int i=0;i<table_widget->columnCount();i++){
        out << table_widget->horizontalHeaderItem(i)->text();
        if(i == table_widget->columnCount()-1){
            out << "\n";
        } else{
            out << ",";
        }
    }
    for(int i=0;i<table_widget->rowCount();i++){
        for(int j=0;j<table_widget->columnCount();j++){
            QTableWidgetItem * data = table_widget->item(i,j);
            if(data!=nullptr){
                out << data->text();
            }
            if(j == table_widget->columnCount()-1){
                out << "\n";
            } else{
                out << ",";
            }

        }
    }

    file.flush();
    file.close();
}
