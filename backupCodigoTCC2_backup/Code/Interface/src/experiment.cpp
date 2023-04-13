#include "../../Interface/inc/experiment.h"

#include <cmath>
#include <QDebug>
#include <time.h>
#include <sys/time.h>
Experiment::Experiment()
{
    
}

QStringList Experiment::getAllData_forInfoTable()
{
    
    QStringList temporary_list;
    temporary_list.push_back(this->getName());
    temporary_list.push_back(this->getOperator_name());
    temporary_list.push_back(this->getInitial_timeString());

    QString present_time=this->day_month_year() + " " + this->hour_min_sec_ms();

    temporary_list.push_back(present_time);
    temporary_list.push_back(this->getTest_type());
    temporary_list.push_back(this->getSpecimen_type());
    temporary_list.push_back(this->getUscs_class());
    temporary_list.push_back(this->getAshto_class());
    temporary_list.push_back(this->getSample_preparations());
    temporary_list.push_back(QString::number(getSample_id()));
    temporary_list.push_back(QString::number(getBoring_number()));
    temporary_list.push_back(this->getSample_location());
    temporary_list.push_back(this->getSample_description());
    temporary_list.push_back(QString::number(getInitial_height()));;
    temporary_list.push_back(QString::number(getInitial_wet_weight()));
    temporary_list.push_back(QString::number(getInitial_moisture()));
    temporary_list.push_back(QString::number(getSpgr_solids()));
    temporary_list.push_back(QString::number(getPlastic_limit()));
    temporary_list.push_back(QString::number(getLiquid_limit()));
    temporary_list.push_back(QString::number(getInitial_position()));
    temporary_list.push_back(QString::number(getDiameter()));


    temporary_list.push_back(QString::number(getArea()));
    temporary_list.push_back(QString::number(getinitial_volume()));
    temporary_list.push_back(QString::number(getinitial_wet_density()));
    temporary_list.push_back(QString::number(getinitial_dry_density()));
    temporary_list.push_back(QString::number(getinitial_void_ratio()));
    temporary_list.push_back(QString::number(getwater_specific_weight()));
    temporary_list.push_back(QString::number(getinitial_saturation()));
    temporary_list.push_back(QString::number(getPressure()));


    return temporary_list;

}

QStringList Experiment::getAllData_forPhasesTable(){
    float calculations[] = {
        getArea(),getinitial_volume(),
        getinitial_wet_density(),getinitial_dry_density(),
        getinitial_void_ratio(),getwater_specific_weight(),
        getinitial_saturation()
    };

    QStringList temporary_list;

    for(unsigned long int i=0;i<sizeof(calculations)/sizeof(float);i++){
        temporary_list.push_back(QString::number(calculations[i]));    
    }

    return temporary_list;
}

QStringList Experiment:: updateDensificationTable()
{
    QStringList temporary_list =  densification_variables.getDensificationVariables();
   
    temporary_list[1] = this->day_month_year();
    temporary_list[2] =this->hour_min_sec_ms();

    this->shear_variables.setDate(temporary_list[1]);
    this->shear_variables.setHour_min_sec_ms(temporary_list[2]);

    uint64_t duration = this->getDuration(true);
    emit updateDensificationChart(duration/1000, densification_variables.getVertical_displacement());

    return temporary_list;
}

QStringList Experiment::updateDensificationResultsTable()
{
    QStringList temporary_list = densification_variables.getDensificationResults();
    temporary_list[0] = this->getDuration_string(true);
    float final_height = this->initial_height - densification_variables.getVertical_displacement();
    temporary_list[2] = QString::number(final_height*10); // converte para milímetros
    float final_volume = final_height * this->getArea();
    temporary_list[4] = QString::number(final_volume);

    return temporary_list;
}

QStringList Experiment:: updateShearTable()
{
    QStringList temporary_list =  shear_variables.getShearVariables();
   
    temporary_list[0] = QString::number(this->getShearSample_number());
    temporary_list[5] = QString::number(this->getNormalTension());
    temporary_list[6] = QString::number(this->getShearTension());

    return temporary_list;
}

QStringList Experiment::updateShearResultsTable()
{
    QStringList temporary_list = shear_variables.getShearResults();
    temporary_list[0] = this->getDuration_string(false);
    temporary_list[2] = QString::number(this->densification_variables.getVertical_load());
    temporary_list[3] = QString::number(this->getNormalTension());
    temporary_list[4] = QString::number(this->getShearTension());
    temporary_list[5] = QString::number(this->densification_variables.getVertical_displacement());
    temporary_list[7] = QString::number(this->getAverageSpeed());
    float final_height = this->initial_height - densification_variables.getVertical_displacement();
    temporary_list[8] = QString::number(final_height);
    float final_volume = final_height * this->getArea();
    temporary_list[9] = QString::number(final_volume);

    return temporary_list;
}


uint64_t Experiment::getDuration(bool isDensification)
{
    uint64_t initial_time_seconds;
    uint64_t initial_time_miliseconds;
    if(isDensification)
    {
        initial_time_seconds = (uint64_t)(this->getInitial_time_seconds());
        initial_time_miliseconds = (uint64_t)(this->getInitial_time_miliseconds());
    } else {
        initial_time_seconds = (uint64_t)(this->shear_variables.getInitial_time_seconds());
        initial_time_miliseconds = (uint64_t)(this->shear_variables.getInitial_time_miliseconds());
    }


    this->setInitial_time(false);   
    uint64_t final_time_seconds = (uint64_t)(this->getPresent_time_seconds());
    uint64_t final_time_miliseconds = (uint64_t)(this->getPresent_time_miliseconds());

    uint64_t duration = (final_time_seconds - initial_time_seconds)*1000 + (final_time_miliseconds - initial_time_miliseconds);
    
    return duration;
}


QString Experiment::getDuration_string(bool isDensification)
{
   
    uint64_t duration = this->getDuration(isDensification);
    
    uint64_t hours = (uint64_t)(duration/3600000);
    uint64_t minutes_calculation = (duration%3600000);
    uint64_t minutes = (uint64_t)(minutes_calculation/60000);
    uint64_t seconds_calculation = (uint64_t)(minutes_calculation%60000);
    uint64_t seconds = (uint64_t)((seconds_calculation/1000));
    uint64_t miliseconds = (uint64_t)((seconds_calculation%1000));

    QString time = QString("%1H:%2min:%3seg:%4ms").arg(hours).arg(minutes).arg(seconds).arg(miliseconds);
    return time;
}

int Experiment::getSample_period() const
{
    return sample_period;
}

void Experiment::setSample_period(int newSample_period)
{
    sample_period = newSample_period;
}

uint32_t Experiment::getShearSample_number() const
{
    return this->densification_variables.getSample_number() - this->shear_variables.getSample_number_diff();
}

float Experiment::getNormalTension()
{
    float normal_load = this->densification_variables.getVertical_load();
    float area = this->getArea();
    return   normal_load/area;
}

float Experiment::getShearTension()
{
    float shear_load = this->shear_variables.getHorizontal_load();
    float area = this->getArea();
    return shear_load/area;
}

float Experiment::getAverageSpeed()
{
    float displacement = this->shear_variables.getHorizontal_displacement();

    uint64_t time = getDuration(false);
    float minutes = time/60000.0;
    return displacement/minutes;
}

QString Experiment::hour_min_sec_ms()
{
    this->setInitial_time(false);
    time_t seconds = (time_t)(this->getPresent_time_seconds());
    struct tm * time_seconds = localtime(&seconds);
    QString time_string = QString("%1:%2:%3:%4").arg(time_seconds->tm_hour).arg(time_seconds->tm_min)
    .arg(time_seconds->tm_sec).arg(this->getPresent_time_miliseconds());

    return time_string;
}

QString Experiment::day_month_year()
{
    this->setInitial_time(false);
    time_t seconds = (time_t)(this->getPresent_time_seconds());
    struct tm * time_seconds = localtime(&seconds);
    QString time_string = QString("%1/%2/%3").arg(time_seconds->tm_mday).arg(time_seconds->tm_mon).
    arg(time_seconds->tm_year - 100);

    return time_string;
}


bool Experiment::getExperimentStarted() const
{
    return experimentStarted;
}

void Experiment::setExperimentStarted(bool newExperimentStarted)
{
    experimentStarted = newExperimentStarted;
}

const QString &Experiment::getName() const
{
    return name;
}

void Experiment::setName(const QString &newName)
{
    name = newName;
}

const QString &Experiment::getOperator_name() const
{
    return operator_name;
}

void Experiment::setOperator_name(const QString &newOperator_name)
{
    operator_name = newOperator_name;
}

const QString &Experiment::getTest_type() const
{
    return test_type;
}

void Experiment::setTest_type(const QString &newTest_type)
{
    test_type = newTest_type;
}

const QString &Experiment::getSpecimen_type() const
{
    return specimen_type;
}

void Experiment::setSpecimen_type(const QString &newSpecimen_type)
{
    specimen_type = newSpecimen_type;
}

const QString &Experiment::getUscs_class() const
{
    return uscs_class;
}

void Experiment::setUscs_class(const QString &newUscs_class)
{
    uscs_class = newUscs_class;
}

const QString &Experiment::getAshto_class() const
{
    return ashto_class;
}

void Experiment::setAshto_class(const QString &newAshto_class)
{
    ashto_class = newAshto_class;
}

const QString &Experiment::getSample_preparations() const
{
    return sample_preparations;
}

void Experiment::setSample_preparations(const QString &newSample_preparations)
{
    sample_preparations = newSample_preparations;
}

int Experiment::getSample_id() const
{
    return sample_id;
}

void Experiment::setSample_id(int newSample_id)
{
    sample_id = newSample_id;
}

int Experiment::getBoring_number() const
{
    return boring_number;
}

void Experiment::setBoring_number(int newBoring_number)
{
    boring_number = newBoring_number;
}

const QString &Experiment::getSample_location() const
{
    return sample_location;
}

void Experiment::setSample_location(const QString &newSample_location)
{
    sample_location = newSample_location;
}

const QString &Experiment::getSample_description() const
{
    return sample_description;
}

void Experiment::setSample_description(const QString &newSample_description)
{
    sample_description = newSample_description;
}

float Experiment::getInitial_height() const
{
    return initial_height;
}

void Experiment::setInitial_height(float newInitial_height)
{
    initial_height = newInitial_height;
}

float Experiment::getInitial_wet_weight() const
{
    return initial_wet_weight;
}

void Experiment::setInitial_wet_weight(float newInitial_wet_weight)
{
    initial_wet_weight = newInitial_wet_weight;
}

float Experiment::getInitial_moisture() const
{
    return initial_moisture;
}

void Experiment::setInitial_moisture(float newInitial_moisture)
{
    initial_moisture = newInitial_moisture;
}

float Experiment::getSpgr_solids() const
{
    return spgr_solids;
}

void Experiment::setSpgr_solids(float newSpgr_solids)
{
    spgr_solids = newSpgr_solids;
}

float Experiment::getPlastic_limit() const
{
    return plastic_limit;
}

void Experiment::setPlastic_limit(float newPlastic_limit)
{
    plastic_limit = newPlastic_limit;
}

float Experiment::getLiquid_limit() const
{
    return liquid_limit;
}

void Experiment::setLiquid_limit(float newLiquid_limit)
{
    liquid_limit = newLiquid_limit;
}

float Experiment::getInitial_position() const
{
    return initial_position;
}

void Experiment::setInitial_position(float newInitial_position)
{
    initial_position = newInitial_position;
}

float Experiment::getDiameter() const
{
    return diameter;
}

void Experiment::setDiameter(float newDiameter)
{
    diameter = newDiameter;
}

float Experiment::getArea()
{
    float value = (pi_value*(diameter*diameter))/4.0;
    return value;
}

float Experiment::getinitial_volume()
{
    float value =  getArea() * initial_height;

    return (std::isfinite(value))? value : 0;
}

float Experiment::getinitial_wet_density()
{
    float value =  initial_wet_weight * getinitial_volume();
    return (std::isfinite(value))? value : 0;
}

float Experiment::getinitial_dry_density()
{
    float value =  ((getinitial_wet_density() * 100) / (100 + initial_moisture));
    return (std::isfinite(value))? value : 0;
}

float Experiment::getinitial_void_ratio()
{
    float value =  ((spgr_solids/getinitial_dry_density()) - 1);
    return (std::isfinite(value))? value : 0;
}

float Experiment::getwater_specific_weight()
{
    return 1.09;
}

float Experiment::getinitial_saturation()
{
    float value =  (spgr_solids *  initial_moisture) / (getinitial_void_ratio() * getwater_specific_weight());
    return (std::isfinite(value))? value : 0;
}

float Experiment::getPressure() const
{
    return pressure;
}

void Experiment::setPressure(float newPressure)
{
    pressure = newPressure;
}

int Experiment::getPhase() const
{
    return this->phase;
}

void Experiment::changePhase()
{
    this->phase = shear_phase;
}

uint64_t Experiment::getInitial_time_seconds() const
{
    return initial_time_seconds;
}

uint64_t Experiment::getPresent_time_seconds() const
{
    return present_time_seconds;
}

uint64_t Experiment::getInitial_time_miliseconds() const
{
    return initial_time_miliseconds;
}

uint64_t Experiment::getPresent_time_miliseconds() const
{
    return present_time_miliseconds;
}

void Experiment::setInitial_time(bool isInitial)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    unsigned long long secondsSinceEpoch = (unsigned long long)(tv.tv_sec) ;
    unsigned long long milisecondsSinceEpoch = (unsigned long long)(tv.tv_usec)/1000 ;


    //+
    //(unsigned long long)(tv.tv_usec) / 1000;
    if(isInitial){
        this->initial_time_seconds = secondsSinceEpoch;
        this->initial_time_miliseconds = milisecondsSinceEpoch;
    } else{
        this->present_time_seconds = secondsSinceEpoch;
        this->present_time_miliseconds = milisecondsSinceEpoch;
    }

}

QString Experiment::getInitial_timeString()
{
    time_t seconds = (time_t)(this->getInitial_time_seconds());
    struct tm * time_seconds = localtime(&seconds);
    QString time_string = QString("%1/%2/%3 %4H:%5min:%6seg:%7ms").arg(time_seconds->tm_mday).arg(time_seconds->tm_mon).
    arg(time_seconds->tm_year + 1900).arg(time_seconds->tm_hour).arg(time_seconds->tm_min).arg(time_seconds->tm_sec)
    .arg(this->getInitial_time_miliseconds());

    return time_string;
}

/*
 * tabela info precisa do time completo dia hora min seg ...
 * tabela adensamento e cisalhamento precisa do dia/mes/ano e hora/min/seg separados
 * o calculo do tempo é feito pegando o tempo inicial e somando o número de amostra
 *
 * *
 *
 * /
 *
 */





