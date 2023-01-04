#ifndef INSTANCE_H
#define INSTANCE_H


#include "common/shortguid/shortguid.h"

class Instance
{
private:
    zShortGuid _instance_id;
public:
    Instance();
    QString ToString(){ return _instance_id.value;}
};

#endif // INSTANCE_H
