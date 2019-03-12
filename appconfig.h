#ifndef APPCONFIG_H
#define APPCONFIG_H
#include<QSharedPointer>
#include<QSettings>

class AppConfig
{
    static AppConfig* m_appConfig;
public:
    QSharedPointer<QSettings> settings;
    AppConfig();
    static AppConfig* getConfig();
    qint16 SNMP_PORT{161};
};

#endif // APPCONFIG_H
