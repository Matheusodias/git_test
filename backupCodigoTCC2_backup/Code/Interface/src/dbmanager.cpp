#include "../../Interface/inc/dbmanager.h"

/**
 * @brief Constrói um novo objeto da classe DBManager.
 * 
 * Esta classe cria o banco de dados, se ele não existir, e insere os valores nas variáveis
 * this->create_table e this->insert_into_table.
 * 
 * @param path Nome do banco de dados com o caminho. Exemplo: home/user/databaseExperiment
 * @param temp_experiment_data Ponteiro para a intância da classe Experiment inicializada na classe MainWindow.
 */
DBManager::DBManager(const QString & path,Experiment * temp_experiment_data)
{
    this->experiment_data = temp_experiment_data;

    this->prova_conceito_database = QSqlDatabase::addDatabase("QSQLITE");
    this->prova_conceito_database.setDatabaseName(path);

    if(this->prova_conceito_database.open())
    {
        qDebug() << "Banco de dados: " << path << " criado.";
    } else{
        qDebug() << "Erro ao criar o banco. " << this->prova_conceito_database.lastError();
    }

    
    this->create_table[experiment_table] =  QString(
    "CREATE TABLE %1 "
    "(experiment_id INTEGER NOT NULL, "
    "name TEXT NOT NULL,"
    "operator_name TEXT	NOT NULL,"
    "initial_time INTEGER NOT NULL, "
    "test_type TEXT	NOT NULL,"
    "specimen_type TEXT	NOT NULL,"
    "uscs_class TEXT NOT NULL,"
    "ashto_class TEXT NOT NULL,"
    "boring_number INTEGER NOT NULL,"
    "initial_height REAL NOT NULL, "
    "initial_wet_weight REAL NOT NULL,"
    "initial_moisture REAL NOT NULL,"
    "spgr_solids REAL NOT NULL,"
    "plastic_limit REAL NOT NULL,"
    "liquid_limit REAL NOT NULL,"
    "initial_position REAL NOT NULL,"
    "diameter REAL NOT NULL,"
    "pressure REAL NOT NULL,"

    "PRIMARY KEY(experiment_id));"

    ).arg(this->table_name[experiment_table]);


    this->insert_into_table[experiment_table] = 
    QString(
        "INSERT INTO %1 ("
        "name, operator_name, initial_time, test_type,"
        "specimen_type, uscs_class, ashto_class,"
        "boring_number,initial_height,"
        "initial_wet_weight, initial_moisture, spgr_solids,"
        "plastic_limit, liquid_limit, initial_position,"
        "diameter, pressure)"
        "Values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
    ).arg(this->table_name[experiment_table]);

    this->create_table[densification_table] =  QString(
    "CREATE TABLE %1 "
    "(densification_id INTEGER NOT NULL,"
    "experiment_id INTEGER NOT NULL,"
    "sample_number INTEGER NOT NULL,"
    "vertical_displacement REAL NOT NULL, "
    "vertical_load REAL NOT NULL,"
    "PRIMARY KEY(densification_id),"
    "FOREIGN KEY(experiment_id) REFERENCES EXPERIMENT_TABLE(experiment_id));"
    ).arg(this->table_name[densification_table]);

    this->insert_into_table[densification_table] = 
    QString(
        "INSERT INTO %1 ("
        "experiment_id ,"
        "sample_number,"
        "vertical_displacement,"
        "vertical_load)"
        "Values (?,?,?,?)"
    ).arg(this->table_name[densification_table]);



    this->create_table[shear_table] =  QString(
    "CREATE TABLE %1 "
    "(shear_id  INTEGER NOT NULL,"
    "experiment_id INTEGER  NOT NULL,"
    "horizontal_displacement REAL NOT NULL,"
    "horizontal_load REAL NOT NULL,"
    "PRIMARY KEY(shear_id),"
    "FOREIGN KEY(experiment_id) REFERENCES EXPERIMENT_TABLE(experiment_id));"
    ).arg(this->table_name[shear_table]);

    this->insert_into_table[shear_table] =
    QString(
        "INSERT INTO %1 ("
        "experiment_id,"
        "horizontal_displacement,"
        "horizontal_load)"
        "Values (?,?,?)"
    ).arg(this->table_name[shear_table]);

    this->create_table[final_variables_table] =
    QString(
        "CREATE TABLE %1 ("
        "final_variables_id INTEGER NOT NULL,"
        "experiment_id INTEGER  NOT NULL,"
        "distance REAL NOT NULL,"
        "velocity REAL NOT NULL,"
        "duration INTEGER NOT NULL,"
        "PRIMARY KEY(final_variables_id),"
        "FOREIGN KEY(experiment_id) REFERENCES EXPERIMENT_TABLE(experiment_id));"
    ).arg(this->table_name[final_variables_table]);

    this->insert_into_table[final_variables_table] =
    QString(
        "INSERT INTO %1 ("
        "experiment_id,"
        "distance,"
        "velocity,"
        "duration)"
        "Values (?,?,?,?)"
    ).arg(this->table_name[final_variables_table]);


    this->create_table[sample_table] =
    QString(
        "CREATE TABLE %1 ("
        "sample_id INTEGER NOT NULL,"
        "sample_identification INTEGER NOT NULL,"
        "experiment_id INTEGER  NOT NULL,"
        "sample_preparations TEXT NOT NULL,"
        "sample_period INTEGER,"
        "sample_number_diff INTEGER,"
        "sample_location TEXT NOT NULL,"
        "sample_description TEXT NOT NULL,"
        "PRIMARY KEY(sample_id),"
        "FOREIGN KEY(experiment_id) REFERENCES EXPERIMENT_TABLE(experiment_id));"
    ).arg(this->table_name[sample_table]);

    this->insert_into_table[sample_table] =
    QString(
        "INSERT INTO %1 ("
        "sample_identification,"
        "experiment_id,"
        "sample_preparations ,"
        "sample_location,"
        "sample_description)"
        "Values (?,?,?,?,?)"
    ).arg(this->table_name[sample_table]);



/*
    1 cisalhamento e de adensamento
    
    experiment_id INTEGER PRIMARY KEY NOT NULL 4
    sample_number INTEGER NOT NULL 4
    horizontal_displacement REAL NOT NULL 8 
    vertical_displacement REAL NOT NULL 8
    horizontal_load REAL NOT NULL 8
    vertical_load REAL NOT NULL 8
    fase INTEGER NOT NULL 1(apenas uma flag)
   

    4+4+8+8+8+8+4 = 8+16+16+1 = 24+17 = 41B

    41 * (12H = 12*60*60s = 43200s) = 1771200B
    

    1771200 - (1209600+  518400.0) = 43200B ou 42.1875MB a mais se utilizar 1 classe em vez de 2


    Vanta


*/

/*
*   densification_id  INTEGER PRIMARY KEY NOT NULL 4
*   experiment_id INTEGER FOREIGN KEY NOT NULL 4
*   sample_number INTEGER NOT NULL 4
*   vertical_displacement REAL NOT NULL 8 
*   vertical_load REAL NOT NULL 8
*   28 TOTAL 


    28 * (12H = 12*60*60s = 43200s) = 1209600B
*/
 

/* 
*   shear_id INTEGER PRIMARY KEY NOT NULL 4
*   experiment_id INTEGER FOREIGN KEY NOT NULL 4
*   horizontal_displacement REAL NOT NULL 8
*   horizontal_load REAL NOT NULL 8
*   
*   24 TOTAL

24 * 43200/2 (supondo que o cisalhamento comece na metade do tempo) = 518400B;

Para recuperar o sample number da SHEAR_TABLE.

sample_number_shear atual = sample_number_densification - this->sample_number_diff
Então para mostrar o número de amostra do cisalhamento,
basta primeiro realizar o select na tabela de adensamento e realizar a conta acima

Assim, não precisa salvar esse dado na tabela de cisalhamento





*/




}

/**
 * @brief Fecha o banco de dados e destrói a instância dele.
 * 
 * O arquivo do banco de dados não é detruído. 
 * 
 */
DBManager::~DBManager()
{
    if(this->prova_conceito_database.isOpen())
    {
        this->prova_conceito_database.close();
    }
}

/**
 * @brief Verifica se o banco de dados está aberto.
 * 
 * @return true Caso esteja aberto.
 * @return false Caso esteja fechado.
 */
bool DBManager::isOpen() const
{
    return this->prova_conceito_database.isOpen();
}

/**
 * @brief Cria uma tabela no banco de dados.
 * 
 * @param option Qual tabela será criada.
 * @return true Se a operação foi bem sucedida.
 * @return false Se a operação falhou.
 */
bool DBManager::createTable(uint8_t option)
{
    bool success = false;
    QSqlQuery query;
    query.prepare(this->create_table[option]);
    //qDebug() << this->create_table[option];
    if(query.exec())
    {
        qDebug() << "Tabela "<< this->table_name[option] << "criada.";
        success = true;
    } else{
        qDebug() << "Erro ao criar a tabela: " << this->table_name[option] << "\n" << query.lastError();
    }
    return success;
}

/**
 * @brief Verifica se a tabela existe no banco de dados.
 * 
 * @param option A tabela escolhida.
 * @return true  Se a operação foi bem sucedida.
 * @return false  Se a operação falhou.
 */
bool DBManager::tableExists(uint8_t option)
{
    bool success = false;
    QSqlQuery query;
    QString command = QString("SELECT EXISTS ( SELECT name FROM sqlite_schema WHERE type='table' AND name='%1')").arg(this->table_name[option]);
    query.prepare(command);

    if(query.exec()){
        while(query.next()){
            if(query.value(0) == 1)
            {
                qDebug() << "A tabela "<< this->table_name[option] << " existe.";
                success = true;
            } else{
                qDebug() << "A tabela "<< this->table_name[option] << " não existe.";
                success = false;
            }

        }
    } else{
        qDebug() << "Erro na seleção da tabela " << this->table_name[option] << ": " << query.lastError();
    }
    return success;
}
/**
 * @brief Insere o valor da variável this->experiment_id após consulta no banco.
 * 
 * Esta função captura o maior valor do experiment_id da tabela EXPERIMENT_TABLE, e o utiliza
 * como chave estrangeira nas outras tabelas. Esse esquema funciona atualmente, pois a chave
 * primária da classe EXPERIMENT_TABLE é gerada automaticamente e é incremental, 1,2,3,4... e assim 
 * por diante.
 * 
 * @return true Se a operação foi bem sucedida.
 * @return false Se a operação falhou.
 */
bool DBManager::selectExperimentId()
{
    bool success = false;
    QSqlQuery query;
    QString command = QString("SELECT MAX(experiment_id) FROM  %1;").arg(this->table_name[experiment_table]);
    query.prepare(command);

    if(query.exec()){
        while(query.next()){
               qDebug() << "ExperimentId " << query.value(0).toString();
               this->experiment_id = query.value(0).toInt();

        }

        success = true;
    } else{
        qDebug() << "Erro na seleção da tabela " << this->table_name << ": " << query.lastError();
    }
    return success;
}

/**
 * @brief Insere dados em uma tabela escolhida.
 * 
 * @param option A tabelha escolhida.
 * @return true Se a operação foi bem sucedida;
 * @return false Se a operação falhou.
 */
bool DBManager::insertIntoTable(uint8_t option)
{
    bool success = false;



    QSqlQuery query;
    query.prepare(this->insert_into_table[option]);
    //qDebug() << this->insert_into_table[option];

    if(option == experiment_table){
        this->insertValuesIntoBind_Experiment(&query);
    } else if(option == densification_table) {
        this->insertValuesIntoBind_Densification(&query);
    } else if(option == shear_table) {
        this->insertValuesIntoBind_Shear(&query);
    } else if(option == final_variables_table){
        this->insertValuesIntoBind_FinalVariables(&query);
    } else if(option == sample_table){
        this->insertValuesIntoBind_SampleVariables(&query);
    }
    

    if(query.exec()){
        //qDebug() << "Inserção concluída com sucesso.";
        success = true;
        if(option == experiment_table){
            while(!this->selectExperimentId());
        }
    } else{
        qDebug() << "Erro na inserção da tabela " << this->table_name[option] << ": " << query.lastError();
    }
    return success;

}
/**
 * @brief Atualiza os dados de uma tabela escolhida.
 * 
 * @param option A tabela escolhida.
 * @return true Se a operação foi bem sucedida.
 * @return false Se a operação falhou.
 */
bool DBManager::updateTable(uint8_t option)
{
    bool success = false;
    QString update;
    QSqlQuery query;
    if(option == sample_table){
       update =  QString(
            "UPDATE %1 "
            "SET sample_period = :sample_period,"
            "sample_number_diff = :sample_number_diff"
            " WHERE experiment_id = %2"
        ).arg(this->table_name[sample_table]).arg(this->experiment_id);
        query.prepare(update);
        insertValuesIntoBind_SampleVariablesUpdate(&query);
    }



    qDebug() << update;

    if(query.exec()){
        qDebug() << "Atualização concluída com sucesso.";
        success = true;
        if(option == experiment_table){
            while(!this->selectExperimentId());
        }
    } else{
        qDebug() << "Erro na inserção da tabela " << this->table_name[option] << ": " << query.lastError();
    }
    return success;

}

/**
 * @brief DBManager::getExperiment_id
 * @return experiment_id da Tabela EXPERIMENT_TABLE no banco
 */
uint32_t DBManager::getExperiment_id() const
{
    return experiment_id;
}


/**
 * @brief Insere valores no bind da inserção da tabela DENSIFICATION_TABLE.
 * 
 * @param query Variável utilizada para executar operações SQL.
 */
void DBManager::insertValuesIntoBind_Densification(QSqlQuery *query)
{
    query->addBindValue(this->experiment_id);
    query->addBindValue(this->experiment_data->densification_variables.getSample_number());
    query->addBindValue(this->experiment_data->densification_variables.getVertical_displacement());
    query->addBindValue(this->experiment_data->densification_variables.getVertical_load());
    

}

/**
 * @brief Insere valores no bind da inserção da tabela SHEAR_TABLE.
 * 
 * @param query Variável utilizada para executar operações SQL.
 */
void DBManager::insertValuesIntoBind_Shear(QSqlQuery *query)
{
    query->addBindValue(this->experiment_id);
    query->addBindValue(this->experiment_data->shear_variables.getHorizontal_displacement());
    query->addBindValue(this->experiment_data->shear_variables.getHorizontal_load());
}

/**
 * @brief Insere valores no bind da inserção da tabela FINAL_VARIABLES_TABLE.
 * 
 * @param query Variável utilizada para executar operações SQL.
 */
void DBManager::insertValuesIntoBind_FinalVariables(QSqlQuery *query)
{
    query->addBindValue(this->experiment_id);
    query->addBindValue(this->experiment_data->shear_variables.getDistance());
    query->addBindValue(this->experiment_data->shear_variables.getVelocity());
    uint64_t duration = this->experiment_data->getDuration(true);
    query->addBindValue(QVariant::fromValue(duration));
}

/**
 * @brief Insere valores no bind da inserção da tabela SAMPLE_VARIABLES_TABLE.
 * 
 * @param query Variável utilizada para executar operações SQL.
 */
void DBManager::insertValuesIntoBind_SampleVariables(QSqlQuery *query)
{
    query->addBindValue(this->experiment_data->getSample_id());
    query->addBindValue(this->experiment_id);
    query->addBindValue(this->experiment_data->getSample_preparations());
    query->addBindValue(this->experiment_data->getSample_location());
    query->addBindValue(this->experiment_data->getSample_description());

}

/**
 * @brief Insere valores no bind da atualização da tabela SAMPLE_VARIABLES_TABLE.
 * 
 * @param query Variável utilizada para executar operações SQL.
 */
void DBManager::insertValuesIntoBind_SampleVariablesUpdate(QSqlQuery *query)
{
    query->bindValue(":sample_number_diff",this->experiment_data->shear_variables.getSample_number_diff());
    query->bindValue(":sample_period",this->experiment_data->getSample_period());
}

/**
 * @brief Insere valores no bind da inserção da tabela EXPERIMENT_TABLE.
 * 
 * @param query Variável utilizada para executar operações SQL.
 */
void DBManager::insertValuesIntoBind_Experiment(QSqlQuery *query)
{
    
    query->addBindValue(this->experiment_data->getName());
    query->addBindValue(this->experiment_data->getOperator_name());
    uint64_t miliseconds = this->experiment_data->getInitial_time_seconds()*1000 + this->experiment_data->getInitial_time_miliseconds();
    query->addBindValue(QVariant::fromValue(miliseconds));
    query->addBindValue(this->experiment_data->getTest_type());
    query->addBindValue(this->experiment_data->getSpecimen_type());
    query->addBindValue(this->experiment_data->getUscs_class());
    query->addBindValue(this->experiment_data->getAshto_class());
    query->addBindValue(this->experiment_data->getBoring_number());
    query->addBindValue(this->experiment_data->getInitial_height());
    query->addBindValue(this->experiment_data->getInitial_wet_weight());
    query->addBindValue(this->experiment_data->getInitial_moisture());
    query->addBindValue(this->experiment_data->getSpgr_solids());
    query->addBindValue(this->experiment_data->getPlastic_limit());
    query->addBindValue(this->experiment_data->getLiquid_limit());
    query->addBindValue(this->experiment_data->getInitial_position());
    query->addBindValue(this->experiment_data->getDiameter());
    query->addBindValue(this->experiment_data->getPressure());
}

/**
 * @brief Atualiza as tabelas de adensamento e cisalhamento.
 * 
 * Recebe o sinal data_arrived() da classe receiveData para executar a atualização.
 */
void DBManager::update_database_table()
{   
    
    this->insertIntoTable(densification_table);

    if(this->experiment_data->getPhase()==shear_phase){
        this->insertIntoTable(shear_table);
    }
}
