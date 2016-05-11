#include "bbox.h"

bbox::bbox()
{
    
}

bbox::bbox(Vector3 max, Vector3 min)
{
    tmax = max;
    tmin = min;
}

bbox::~bbox()
{
}