#ifndef APPLICATIONCLASS_H
#define APPLICATIONCLASS_H
#include<QCoreApplication>
#include <QObject>
#include "snmpserver.h"

class ApplicationClass:public QObject
{
    Q_OBJECT
    QCoreApplication *app;
    SNMPServer *snmpServer;

public:
    explicit ApplicationClass(QObject *parent=0);
    void quit();


signals:
    void finished();

public slots:
    void run();
    void readSNMP();
    void aboutToQuitApp();
};

#endif // APPLICATIONCLASS_H
