#include "instance.h"

Instance::Instance()
{
    _instance_id = zShortGuid::createNew();
}
