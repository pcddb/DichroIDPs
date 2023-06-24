#ifndef GLOBAL_H
#define GLOBAL_H
#include <QtGlobal>
#include<QObject>


class Global: public QObject
{
    Q_OBJECT
private:
    explicit Global(QObject *parent = nullptr);

    static bool instanceFlag;
    static Global *single;


public:
    static Global* getInstance();

    ~Global()
    {
        instanceFlag = false;
    }
    QString globalFile;
};
#endif // GLOBAL_H
