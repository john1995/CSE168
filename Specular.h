//
//  Specular.hpp
//  
//
//  Created by Macklin Groot on 4/15/16.
//
//

#ifndef Specular_h
#define Specular_h

#include <Math.h>
#include "Material.h"

class Specular : public Material
{
public:
    //Takes in a specular scale factor k_s and a shininess exponent s
    Specular(const Vector3 & ks = Vector3(1),
            const float exp = 1.0f);
    
    //Destructor
    ~Specular();
    
    //Getters and setters
    Vector3 getSpecScaleFactor();
    void setSpecScaleFactor(Vector3);
    float getShinyExp();
    void setShinyExp(float);
    void setBackgroundColor(const Vector3);
    
    virtual Vector3 shade(Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
    
protected:
    Vector3 k_s;            //specular scale factor
    float shinyExp;         //shininess exponent
    
    Vector3 bgColor;        //background color needed for refraction
};

#endif /* Specular_hpp */
