#ifndef SNMPSERVICE_H
#define SNMPSERVICE_H
#include "qtservice.h"
#include "applicationclass.h"
#include <QCoreApplication>

class SNMPService:public QtService<QCoreApplication>
{

public:
    SNMPService(int argc, char **argv);


protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);

private:
    bool disabled;
    ApplicationClass *snmpApp;
signals:
public slots:
};

#endif // SNMPSERVICE_H
