#include "param.h"
#include "QDebug"
#define INT_ERROR_ANSWER -1;
#define STRING_ERROR_ANSWER "ERROR";

Param::Param(QString _oid,QString _class_type, QString _type,QString _value):oid(_oid),class_type(_class_type),type(_type),value(_value)
{

}

qint32 Param::getParamInt()
{
    if(type.toUpper() == "INTEGER")
    {
        qDebug() << "Conversion " << value.toInt();
        return value.toInt();
    }
    else {
        return  INT_ERROR_ANSWER;
    }

}

QString Param::getParamString()
{
    if(type.toUpper() == "STRING")
    {
        return value;
    }
    else {
        return  STRING_ERROR_ANSWER;
    }

}
