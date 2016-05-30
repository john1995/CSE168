//
//  Glass.hpp
//  
//
//  Created by Macklin Groot on 4/27/16.
//
//

#ifndef Glass_hpp
#define Glass_hpp

#include <stdio.h>
#include "Material.h"

class Glass : public Material
{
public:
    //constructor takes transmission coefficient k_t, absporption coefficient k_d,
    //and reflection coefficient k_s. k_t + k_d + k_s = 1
    Glass(float kd, float ks);
    Glass();
    virtual Vector3 shade(Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
    
    void setBackgroundColor(const Vector3);
    
protected:
    //Coefficients in order: diffuse, reflection, transmission, ambient
    float k_d, k_s, k_a;
    
    const int maxBounces = 4;
    Vector3 bgColor;        //background color needed for refraction
};

#endif /* Glass_hpp */
