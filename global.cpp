#include"global.h"

Global::Global(QObject *parent) : QObject(parent)
{

}


bool Global::instanceFlag = false;
Global* Global::single = nullptr;
Global* Global::getInstance()

{

    if(! instanceFlag)
    {
        single = new Global();
        instanceFlag = true;
        return single;
    }
    else
    {
        return single;
    }
}
