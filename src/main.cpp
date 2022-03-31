#include "loginwindow.h"

#include <QApplication>

#include "log.h"

int main(int argc, char *argv[])
{
    //放在main中的第一行
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    LoginWindow w;
    logInit();
    w.show();
    return a.exec();
}
