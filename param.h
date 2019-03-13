#ifndef PARAM_H
#define PARAM_H
#include<QString>

class Param
{
public:
    Param();
    Param(QString ,QString, QString, QString);
    QString oid{"NONE"};
    QString class_type{"NONE"};
    QString type{"NONE"};
    QString value{"NONE"};
    QString getParamString();
    qint32 getParamInt();
};

#endif // PARAM_H
