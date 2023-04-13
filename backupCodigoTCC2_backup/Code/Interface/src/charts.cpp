#include "inc/charts.h"

Charts::Charts(QObject *parent)
    : QObject{parent}
{

}

void Charts::initialConfiguration(QVBoxLayout * layout)
{
    this->series = new QSplineSeries(this);
    m_axisX = new QValueAxis();
    m_axisY = new QValueAxis();

    series->append(0, 0);
//    series->append(2, 4);
//    series->append(3, 8);
//    series->append(7, 4);
//    series->append(10, 5);
//    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->addAxis(m_axisX,Qt::AlignBottom);
    chart->addAxis(m_axisY,Qt::AlignLeft);
    series->attachAxis(m_axisX);
    series->attachAxis(m_axisY);
    //chart->createDefaultAxes();
    chart->setTitle("Tempo por deslocamento vertical");

    m_axisX->setTitleText("Tempo (s)");
    m_axisY->setTitleText("Desloc. vertical (cm)");
    
//    chart->axes(Qt::Vertical).at(0)->setTitleText("Deslocamento vertical (cm)");
//    chart->axes(Qt::Horizontal).at(0)->setTitleText("Tempo");



    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);


    layout->addWidget(chartView);
}

void Charts:: updateCharts(int time, float vertical_displacement)
{
    qDebug() << "Estou aqui" <<time  << " " << vertical_displacement;
    series->append(time, vertical_displacement);
    chart->removeSeries(series);
    chart->addSeries(series);
    if(time > m_axisX->max()){
        qDebug() << "entrei aqui";
        m_axisX->setMax(time);
        //chart->axes(Qt::Horizontal).at(0)->setMax(20+current_value);
    }
    if(vertical_displacement > m_axisY->max()){
        m_axisY->setMax(vertical_displacement);
    }
    //chartView->update();
}
