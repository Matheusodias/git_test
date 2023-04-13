#ifndef CHARTS_H
#define CHARTS_H

#include <QObject>
#include <QtCharts>
#include <QVBoxLayout>
class Charts : public QObject
{
    Q_OBJECT
public:
    explicit Charts(QObject *parent = nullptr);
    void initialConfiguration(QVBoxLayout * layout);
private:
    QSplineSeries *series;
    QChart *chart;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
signals:

public slots:
    void updateCharts(int time, float vertical_displacement);

};

#endif // CHARTS_H
