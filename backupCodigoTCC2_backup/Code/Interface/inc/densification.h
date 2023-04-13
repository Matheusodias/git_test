#ifndef DENSIFICATION_H
#define DENSIFICATION_H

#include <stdint.h>
#include <QStringList>
/**
 * @brief Classe do adensamento.
 * 
 * Esta classe é reponsável por administrar as operações e dados relacionados 
 * ao adensamento no experimento.
 */
class Densification
{
public:
    Densification();
    void setDensificationVariables(uint32_t sample_number, float vertical_displacement,
                                   float vertical_load);
    
    QStringList getDensificationVariables();
    QStringList getDensificationResults();

    uint32_t getSample_number() const;

    float getVertical_displacement() const;
    float getVertical_load() const;

    void setDiff_sampleNumber_initExperiment(uint32_t diff);

private:
    uint32_t sample_number; /*!< Número da amostra atual. */
    float vertical_displacement;  /*!< Deslocamento vertical. */
    float vertical_load; /*!< Carga vertical. */
    /**
     * @brief Último número de amostra antes depois do posicionamento inicial.
     * 
     * A máquina sempre manda o número de amostra incrementando, 1,2,3,4 ... e assim por diante.
     * Durante o posicionamento inicial o número de amostra é incrementado e assim,
     * para o adensamento começar com o número de amostra 0 é necessário realizar uma diferença
     * entre o número atual e este número.
     */
    uint32_t diff_sampleNumber_initExperiment; 


};

#endif // DENSIFICATION_H
