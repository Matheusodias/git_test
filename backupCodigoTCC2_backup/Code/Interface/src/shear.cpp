#include "../../Interface/inc/shear.h"
#include <time.h>
#include <sys/time.h>
Shear::Shear(QObject *parent)
    : QObject{parent}
{
    this->setVelocity(0);
    this->setDistance(0);
    this->setShearVariables(0,0);
}

int16_t Shear::getVelocity() const
{
    return velocity;
}

void Shear::setVelocity(int16_t newVelocity)
{
    velocity = newVelocity;
}

int16_t Shear::getDistance() const
{
    return distance;
}

void Shear::setDistance(int16_t newDistance)
{
    distance = newDistance;
}

void Shear::setShearVariables(float horizontal_displacement, float horizontal_load)
{
    this->setHorizontal_displacement(horizontal_displacement);
    this->setHorizontal_load(horizontal_load);
}

QStringList Shear::getShearVariables()
{
    QStringList temp;
    temp.push_back("N. amostra");
    temp.push_back(this->getDate());
    temp.push_back(this->getHour_min_sec_ms());
    temp.push_back(QString::number(this->getHorizontal_displacement()));
    temp.push_back(QString::number(this->getHorizontal_load()));
    temp.push_back("Tensão nor.");
    temp.push_back("Tensão cis.");

    return temp;
}

QStringList Shear::getShearResults()
{
    QStringList temp;
    temp.push_back("Tempo total");
    temp.push_back(QString::number(this->getHorizontal_load()));
    temp.push_back("Carga normal");
    temp.push_back("Tensão de cisalhamento");
    temp.push_back("Tensão normal");
    temp.push_back("Deslocamento vertical");
    temp.push_back(QString::number(this->getHorizontal_displacement()));
    temp.push_back("Velocidade média(cm/min)");
    temp.push_back("Mudança de altura(mm)");
    temp.push_back("Volume final (cm³)");
        
    return temp;
}

uint32_t Shear::getSample_number_diff() const
{
    return sample_number_diff;
}

void Shear::setSample_number_diff(uint32_t newSample_number_diff)
{
    sample_number_diff = newSample_number_diff;
}

float Shear::getHorizontal_displacement() const
{
    return horizontal_displacement;
}

void Shear::setHorizontal_displacement(float newHorizontal_displacement)
{
    horizontal_displacement = newHorizontal_displacement;
}

float Shear::getHorizontal_load() const
{
    return horizontal_load;
}

void Shear::setHorizontal_load(float newHorizontal_load)
{
    horizontal_load = newHorizontal_load;
}

const QString &Shear::getDate() const
{
    return date;
}

void Shear::setDate(const QString &newDate)
{
    date = newDate;
}

const QString &Shear::getHour_min_sec_ms() const
{
    return hour_min_sec_ms;
}

void Shear::setHour_min_sec_ms(const QString &newHour_min_sec_ms)
{
    hour_min_sec_ms = newHour_min_sec_ms;
}

uint64_t Shear::getInitial_time_seconds() const
{
    return initial_time_seconds;
}

uint64_t Shear::getInitial_time_miliseconds() const
{
    return initial_time_miliseconds;
}

void Shear::setInitial_time()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    unsigned long long secondsSinceEpoch = (unsigned long long)(tv.tv_sec) ;
    unsigned long long milisecondsSinceEpoch = (unsigned long long)(tv.tv_usec)/1000 ;


    //+
    //(unsigned long long)(tv.tv_usec) / 1000;

    this->initial_time_seconds = secondsSinceEpoch;
    this->initial_time_miliseconds = milisecondsSinceEpoch;


}
