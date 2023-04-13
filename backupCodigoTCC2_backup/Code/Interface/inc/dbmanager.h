#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlRecord>
#include "experiment.h"

#define experiment_table 0 /*!< Número identificador da tabela EXPERIMENT_TABLE. */
#define densification_table 1 /*!< Número identificador da tabela DENSIFICATION_TABLE. */
#define shear_table 2 /*!< Número identificador da tabela SHEAR_TABLE. */
#define final_variables_table 3 /*!< Número identificador da tabela FINAL_VARIABLES_TABLE. */
#define sample_table 4 /*!< Número identificador da tabela SAMPLE_TABLE. */

/**
 * @brief Classe do banco de dados.
 * 
 * Esta classe é responsável por administrar todas as operações relacionadas ao banco de dados.
 */
class DBManager : public QObject
{
    Q_OBJECT
public:
    DBManager(const QString & path, Experiment * temp_experiment_data);
    ~DBManager();
    bool isOpen() const;
    bool createTable(uint8_t option);
    bool tableExists(uint8_t option);
    bool insertIntoTable(uint8_t option);
    bool updateTable(uint8_t option);
   
    Experiment * experiment_data; /*!< Instância da classe Experiment utilizada para atualizar/inserir dados nas tabelas. */


    uint32_t getExperiment_id() const;

private:
    uint32_t experiment_id; /*!< Id do experimento utilizado para preencher a chave estrangeira em outras tabelas. */

    QSqlDatabase prova_conceito_database; /*!< Variável do banco de dados. */
    QString table_name[5] = {"EXPERIMENT_TABLE","DENSIFICATION_TABLE","SHEAR_TABLE","FINAL_VARIABLES_TABLE","SAMPLE_TABLE"}; /*!< Array com o nome das tabelas.*/
    
    void insertValuesIntoBind_Experiment(QSqlQuery *query);
    void insertValuesIntoBind_Densification(QSqlQuery *query);
    void insertValuesIntoBind_Shear(QSqlQuery *query);
    void insertValuesIntoBind_FinalVariables(QSqlQuery *query);
    void insertValuesIntoBind_SampleVariables(QSqlQuery *query);
    void insertValuesIntoBind_SampleVariablesUpdate(QSqlQuery *query);

    QString create_table[5]; /*!< Array com as strings utilizadas para criar as tabelas. */
    QString insert_into_table[6]; /*!> Array com as string utilizadas para inserir dados nas tabelas. */
    

    bool selectExperimentId();


public slots:
    void update_database_table();

};
#endif // DBMANAGER_H
