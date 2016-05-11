//
//  Refract.hpp
//  
//
//  Created by Macklin Groot on 4/27/16.
//
//

#ifndef Refract_hpp
#define Refract_hpp

#include <stdio.h>
#include "Material.h"

class Refract : public Material
{
public:
    //Takes in a transmission coefficient k_t
    Refract(const Vector3 & kt = Vector3(1),
             const float exp = 1.0f);
    
    //Destructor
    ~Refract();
    
    //Getters and setters
    Vector3 getTransmissionCoef();
    void setTransmissionCoef(Vector3);
    void setBackgroundColor(const Vector3);
    void setMaxBounces(const int);
    
    virtual Vector3 shade(Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
    
protected:
    Vector3 k_t;            //specular reflection coefficient
    
    Vector3 bgColor;        //background color needed for refraction
    int maxBounces;
};

#endif /* Refract_hpp */
