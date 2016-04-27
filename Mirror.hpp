//
//  Mirror.hpp
//  
//
//  Created by Macklin Groot on 4/26/16.
//
//

#ifndef Mirror_hpp
#define Mirror_hpp

#include <stdio.h>
#include "Material.h"
#include "Specular.h"

//Mirrors are purely reflective surface. Right now, no
//diffuse cmpnt, but a small one could potentially be added.
//Uses Blinn model to calculate reflections
class Mirror : public Material
{
public:
    //Takes in a specular scale factor k_s, a shininess exponent s,
    //and a diffuse component encapsulated in a Lambert material.
    //Very high "shiny" exponent to make it look more like plastic
    Mirror(Specular* specular = new Specular(Vector3(0.5f), 2.0f));
    
    ~Mirror();
    
    Specular* getSpecularCmpnt();
    void setSpecularCmpnt(Specular*);
    
    virtual Vector3 shade(Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
    
protected:
    Specular* specularCmpnt;
    static const int MAX_BOUNCES =  2;
};

#endif /* Mirror_hpp */
