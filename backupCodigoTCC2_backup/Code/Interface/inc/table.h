#ifndef TABLE_H
#define TABLE_H

#include <QObject>
#include <QTableWidget>
#include "experiment.h"

#define phases_table 0
#define info_table 1
#define densification_result_table 2
#define shear_result_table 3
#define design_densification_table 0
#define design_shear_table 1

class Table : public QObject
{
    Q_OBJECT
public:
    explicit Table(Experiment *parent = nullptr, QTableWidget *densification  = nullptr
    , QTableWidget *shear  = nullptr);
    void customizeTable(QTableWidget * table_widget);

    void initialConfig_ShearTable(QTableWidget * table_widget);
    void updateData_ShearTable(QTableWidget * table_widget);

    void initialConfig_StaticTable(QTableWidget *table_widget, uint8_t option);
    void updateData_StaticTable(QTableWidget *table_widget, uint8_t option);

    void updateData_DynamicTable(QStringList data, uint8_t option);
    void initialConfig_DynamicTable(QTableWidget *table_widget,uint8_t option);

    void clearDynamicTables(QTableWidget *table_widget);
    void clearStaticTables(QTableWidget *table_widget);



    void test();

    Experiment * table_variables;

private:
    
    QStringList lineNames[4];
    QStringList columnValues;

    QStringList headerNames[2];
    QStringList machineTablevalues[2];

    QTableWidget *densificationTable;
    QTableWidget *shearTable;

signals:

};

#endif // TABLE_H
