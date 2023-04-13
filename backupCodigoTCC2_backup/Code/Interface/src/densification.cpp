#include "../../Interface/inc/densification.h"
#include <QString>
#include <QDebug>


/**
 * @brief Constrói uma nova instância da classe objeto.
 * 
 * Inicializa o sample_number e o diff_sampleNumber_initExperiment com o valor 0.
 * 
 */
Densification::Densification()
{
    this->setDiff_sampleNumber_initExperiment(0);
    this->sample_number = 0;
}

/**
 * @brief Insere os dados recebidos nas variáveis da classe this->sample_number, this->vertical_displacement e this->vertical_load.
 * 
 * @param sample_number Número da amostra recebido pela camada de controle.
 * @param vertical_displacement Deslocamento vertical recebido pela camada de controle.
 * @param vertical_load  Carga vertical recebido pela camada de controle.
 */
void Densification::setDensificationVariables(uint32_t sample_number, float vertical_displacement, float vertical_load)
{
    this->sample_number = sample_number;
    this->vertical_displacement = vertical_displacement;
    this->vertical_load = vertical_load;
}
/**
 * @brief Retorna alguns dos valores utilizados na tabela de adensamento.
 * 
 * @return QStringList Lista do tipo QString que guarda os valores.
 */
QStringList Densification::getDensificationVariables()
{
    QStringList temporary_list;
    temporary_list.push_back(QString::number(this->getSample_number()));
    temporary_list.push_back("dia");
    temporary_list.push_back("hora/min/seg");
    temporary_list.push_back(QString::number(this->vertical_displacement));
    temporary_list.push_back(QString::number(this->vertical_load));

    return temporary_list;

}
/**
 * @brief Retorna o valor do número de amostra atual.
 * 
 * A máquina sempre manda o número de amostra incrementando, 1,2,3,4 ... e assim por diante.
 * Durante o posicionamento inicial o número de amostra é incrementado e assim,
 * para o adensamento começar com o número de amostra 0 é necessário realizar uma diferença
 * entre o número atual e este número.
 * 
 * @return uint32_t Número da amostra.
 */
uint32_t Densification::getSample_number() const
{
    // qDebug() <<"Amostra = " <<  sample_number;
    // qDebug() << diff_sampleNumber_initExperiment;
    return sample_number - diff_sampleNumber_initExperiment;
}

/**
 * @brief Define o valor de this->diff_sampleNumber_initExperiment.
 * 
 * @param diff Valor definido.
 */
void Densification::setDiff_sampleNumber_initExperiment(uint32_t diff)
{
    this->diff_sampleNumber_initExperiment = diff;
}
/**
 * @brief Retorna o valor do deslocamento vertical.
 * 
 * @return float Valor do deslocamento vertical atual.
 */
float Densification::getVertical_displacement() const
{
    return vertical_displacement;
}
/**
 * @brief Retorna o valor da carga vertical.
 * 
 * @return float Valor da carga vertical atual.
 */
float Densification::getVertical_load() const
{
    return vertical_load;
}
/**
 * @brief Retorna alguns dos valores utilizados na tabela de resultados do adensamento.
 * 
 * @return QStringList Lista de QStrings com os valores.
 */
QStringList Densification::getDensificationResults()
{
    QStringList temporary_list;
    temporary_list.push_back("duration");
    temporary_list.push_back(QString::number(this->vertical_load));
    temporary_list.push_back("final_height");
    temporary_list.push_back(QString::number(this->vertical_displacement));
    temporary_list.push_back("final_volume");


    return temporary_list;
}


//Nome da variável	Nome na tabela	Observação
//duration_densification	Tempo Total Adensamento	present_time - initial time ao iniciar a fase de cisalhamento
//recommended_shear_ratio	Razão de cisalhamento recomendada
//final_read	Leitura da carga final	Última leitura da carga antes de iniciar a fase de cisalhamento.
//final_heigth	Altura final	initial_height - height_change
//heigth_change	Mudança de altura
//final_volume	Volume Final da fase de adensamento(cm³)	area * final_height
