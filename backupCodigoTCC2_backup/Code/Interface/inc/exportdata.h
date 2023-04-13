#ifndef EXPORTDATA_H
#define EXPORTDATA_H

#include <QObject>
#include <QTableWidget>
/**
 * @brief Classe que exporta os dados para csv.
 * 
 * Esta classe é responsável por exportar os dados para o Github.
 * 
 */
class exportData : public QObject
{
    Q_OBJECT
public:
    explicit exportData(QObject *parent = nullptr);

private:
    void write(QTableWidget *table_widget,QString filename);
signals:

public slots:
    void exportCSV(QTableWidget *table_widget, int file_identification, int option);
};

#endif // EXPORTDATA_H
