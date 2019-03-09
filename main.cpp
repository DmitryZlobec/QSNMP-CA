#include <QCoreApplication>
#include<QObject>
#include<QTimer>
#include"applicationclass.h"
#include"snmpserver.h"


int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);
    ApplicationClass snmpApp;
    QObject::connect(&snmpApp,SIGNAL(finished()),&a,SLOT(quit()));
    QObject::connect(&a,SIGNAL(aboutToQuit()),&snmpApp,SLOT(aboutToQuitApp()));
    QTimer::singleShot(10,&snmpApp,SLOT(run()));
    return a.exec();
}
