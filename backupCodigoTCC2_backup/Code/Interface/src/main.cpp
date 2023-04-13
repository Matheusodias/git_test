#include "inc/mainwindow.h"
#include <QApplication>
#include <QThread>

int main(int argc, char *argv[])
{
    //qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowFlags(Qt::FramelessWindowHint);

    w.showMaximized();


    // w.show();

    return a.exec();
}
