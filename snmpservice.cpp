#include <QTimer>
#include "snmpservice.h"
#include "applicationclass.h"
SNMPService::SNMPService(int argc, char **argv): QtService<QCoreApplication>(argc, argv, "Qt SNMP Daemon")
{
    setServiceDescription("A SNMP service implemented with Qt");
    setServiceFlags(QtServiceBase::CanBeSuspended);


}

void SNMPService::start()
{
    QCoreApplication *a=application();
    snmpApp =new ApplicationClass();
    snmpApp->configFile = configFileName;
    QObject::connect(snmpApp,SIGNAL(finished()),a,SLOT(quit()));
    QObject::connect(a,SIGNAL(aboutToQuit()),snmpApp,SLOT(aboutToQuitApp()));
    QTimer::singleShot(10,snmpApp,SLOT(run()));

}

void SNMPService::stop()
{
    delete snmpApp;
}

void SNMPService::pause()
{
  disabled = true;
}

void SNMPService::resume()
{
    disabled = false;

}

void SNMPService::processCommand(int code)
{

}
