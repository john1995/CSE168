#ifndef CSE168_MATERIAL_H_INCLUDED
#define CSE168_MATERIAL_H_INCLUDED

#include "Miro.h"
#include "Vector3.h"

class Material
{
public:
    Material();
    virtual ~Material();

    virtual void preCalc() {}
    
    /* NOTE: param ray used to be const, but was made changeable
     * so the number of bounces of a ray could be modified */
    virtual Vector3 shade(Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
};

#endif // CSE168_MATERIAL_H_INCLUDED
