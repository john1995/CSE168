#ifndef CSE168_LAMBERT_H_INCLUDED
#define CSE168_LAMBERT_H_INCLUDED

#include "Material.h"

class Lambert : public Material
{
public:
    Lambert(const Vector3 & kd = Vector3(1),
            const Vector3 & ka = Vector3(0));
    virtual ~Lambert();

    const Vector3 & getkd() const {return k_d;}
    const Vector3 & getka() const {return k_a;}

    void setKd(const Vector3 & kd) {k_d = kd;}
    void setKa(const Vector3 & ka) {k_a = ka;}

    virtual void preCalc() {}
    
    virtual Vector3 shade(Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
    
    void print() const;
protected:
    
    Vector3 k_a;
};

#endif // CSE168_LAMBERT_H_INCLUDED
