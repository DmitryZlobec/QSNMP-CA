#include <QCoreApplication>
#include <iostream>
#include<QObject>
#include<QTimer>
#include<QCommandLineParser>
#include <QFileInfo>
#include <QtGlobal>
#include <QTextStream>
#include <QDateTime>
#include "applicationclass.h"
#include "snmpserver.h"
#include "snmpservice.h"

QString configFileName = "qsnmp.ini";
QString logFileName = "qsnmp.log";

void snmpDebugMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    QString message=QString("%2 in %3 file %4").arg(localMsg.data()).arg(function).arg(file);
    QString txt;
    switch(type){
        case QtDebugMsg:
                txt = QString("%1 Debug: %2 ").arg(QDateTime::currentDateTime().toString()).arg(message);
                break;
        case QtWarningMsg:
                txt = QString("%1 Warning: %2 ").arg(QDateTime::currentDateTime().toString()).arg(message);
                break;
        case QtCriticalMsg:
                txt = QString("%1 Critical: %2 ").arg(QDateTime::currentDateTime().toString()).arg(message);
                break;
        case QtFatalMsg:
                txt = QString("%1 Fatal: %2 ").arg(QDateTime::currentDateTime().toString()).arg(message);
                abort();
        default:
                txt = QString("%1 Other: %2").arg(QDateTime::currentDateTime().toString()).arg(message);
    }
    QFile logFile(logFileName);
    logFile.open(QIODevice::WriteOnly| QIODevice::Append);
    QTextStream logStream(&logFile);
    logStream << txt << "\n\r" << endl;
}


int main(int argc, char *argv[])
{
    qInstallMessageHandler(snmpDebugMessageHandler);
    QCoreApplication::setApplicationName("QSNMP-CA");
    QCoreApplication::setApplicationVersion("0.1");
    QCommandLineParser parser;
    parser.setApplicationDescription("QSNMP-CA helper");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption conigFileOption(QStringList() << "c" << "config-file",
                 QCoreApplication::translate("main", "Config file <config-file>."),
                 QCoreApplication::translate("main", "config-file"));
    QCommandLineOption logFileOption(QStringList() << "l" << "log-file",
                 QCoreApplication::translate("main", "Log file <log-file>."),
                 QCoreApplication::translate("main", "log-file"));
    QCommandLineOption asServiceOption(QStringList() << "s" << "service",
                QCoreApplication::translate("main", "Run as sevice."));
    parser.addOption(asServiceOption);
    parser.addOption(conigFileOption);
    parser.addOption(logFileOption);

    QStringList arguments;
    for(int k=0;k<argc;++k)
    {
        arguments <<QString(argv[k]);
    }
    parser.process(arguments);


    QString logFileNameArgument = parser.value(logFileOption);

    if(parser.isSet(conigFileOption))
    {
        configFileName = parser.value(conigFileOption);
    }

    if(parser.isSet(logFileOption))
    {
        logFileName = parser.value(logFileOption);
    }

    bool asService = parser.isSet(asServiceOption);

    if(asService)
    {
        QFileInfo confFile(configFileName);
        if(!confFile.exists())
        {
            qDebug() << "No config file found!";
            abort();
        }
        SNMPService service(argc, argv);
        return service.exec();

    }
    else
    {
        std::cout << "Qt SNMP Common Agent 2019" << std::endl;
        std::cout << "Config file name: " << configFileName.toStdString() << std::endl;
        std::cout << "Log file name: " << logFileName.toStdString() << std::endl;

        QFileInfo confFile(configFileName);
        if(!confFile.exists())
        {
            std::cout << "No config file found!" <<std::endl;
            abort();
        }

        QCoreApplication a(argc, argv);
        ApplicationClass snmpApp;
        snmpApp.configFile = configFileName;
        QObject::connect(&snmpApp,SIGNAL(finished()),&a,SLOT(quit()));
        QObject::connect(&a,SIGNAL(aboutToQuit()),&snmpApp,SLOT(aboutToQuitApp()));
        QTimer::singleShot(10,&snmpApp,SLOT(run()));

        return a.exec();
    }
}
