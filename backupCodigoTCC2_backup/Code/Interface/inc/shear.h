#ifndef SHEAR_H
#define SHEAR_H

#include <QObject>

class Shear : public QObject
{
    Q_OBJECT
public:
    explicit Shear(QObject *parent = nullptr);

    int16_t getVelocity() const;
    void setVelocity(int16_t newVelocity);

    int16_t getDistance() const;
    void setDistance(int16_t newDistance);

    void setShearVariables(float horizontal_displacement,float horizontal_load);
    QStringList getShearVariables();
    QStringList getShearResults();

    uint32_t getSample_number_diff() const;
    void setSample_number_diff(uint32_t newSample_number_diff);

    float getHorizontal_displacement() const;
    void setHorizontal_displacement(float newHorizontal_displacement);

    float getHorizontal_load() const;
    void setHorizontal_load(float newHorizontal_load);

    const QString &getDate() const;
    void setDate(const QString &newDate);

    const QString &getHour_min_sec_ms() const;
    void setHour_min_sec_ms(const QString &newHour_min_sec_ms);

    uint64_t getInitial_time_seconds() const;


    uint64_t getInitial_time_miliseconds() const;


    void setInitial_time();

private:

    uint32_t sample_number_diff;
    float horizontal_displacement;
    float horizontal_load;
    
    int16_t velocity;
    int16_t distance;

    QString date;
    QString hour_min_sec_ms;

    uint64_t initial_time_seconds;
    uint64_t initial_time_miliseconds;

//    vertical_displacement // recebido
//    horizontal_displacement // recebido
//    shear_load // recebido
//    normal_tension // normal_load/area
//    shear_tension // shear_load/area

signals:

};

#endif // SHEAR_H
