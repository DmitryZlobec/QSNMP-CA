#ifndef APPCONFIG_H
#define APPCONFIG_H
#include<QSharedPointer>
#include<QList>
#include<QSettings>
#include"param.h"
class AppConfig
{
    static AppConfig* m_appConfig;
public:
    QSharedPointer<QSettings> settings;
    AppConfig();
    ~AppConfig();
    static AppConfig* getConfig();
    qint16 SNMP_PORT{161};
    QList<QSharedPointer<Param>>* params;
};

#endif // APPCONFIG_H
