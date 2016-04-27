//
//  Plastic.hpp
//  
//
//  Created by Macklin Groot on 4/15/16.
//
//

#ifndef Plastic_h
#define Plastic_h

#include <stdio.h>
#include "Material.h"
#include "Lambert.h"
#include "Specular.h"

//No reflections, but there is a highlight
class Plastic : public Material
{
public:
    //Takes in a specular scale factor k_s, a shininess exponent s,
    //and a diffuse component encapsulated in a Lambert material.
    //Very high "shiny" exponent to make it look more like plastic
    Plastic(Lambert* diffuse = new Lambert(),
            Specular* specular = new Specular(Vector3(0.5f), 2.0f));
    
    ~Plastic();
    
    Lambert* getDiffuseCmpnt();
    Specular* getSpecularCmpnt();
    void setDiffuseCmpnt(Lambert*);
    void setSpecularCmpnt(Specular*);
    
    virtual Vector3 shade(Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
    
protected:
    Lambert* diffuseCmpnt;
    Specular* specularCmpnt;
    static const int MAX_BOUNCES =  0;
};

#endif /* Plastic_hpp */
