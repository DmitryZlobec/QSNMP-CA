#ifndef APPLICATIONCLASS_H
#define APPLICATIONCLASS_H
#include<QCoreApplication>
#include <QObject>
#include <QSettings>
#include "snmpserver.h"
extern QString configFileName;
extern QString logFileName;

class ApplicationClass:public QObject
{
    Q_OBJECT
    QCoreApplication *app;
    SNMPServer *snmpServer;

public:
    explicit ApplicationClass(QObject *parent=nullptr);
    void quit();
    QString configFile {"qsnmp.ini"};


signals:
    void finished();

public slots:
    void run();
    void readSNMP();
    void aboutToQuitApp();
};

#endif // APPLICATIONCLASS_H
