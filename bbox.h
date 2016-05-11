#ifndef CSE168_BBOX_H_INCLUDED
#define CSE168_BBOX_H_INCLUDED

#include "Vector3.h"


class bbox
{

public:
    bbox();
    bbox(Vector3, Vector3);
    ~bbox();
    Vector3   tmin;
    Vector3   tmax;

};




#endif 