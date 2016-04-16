//
//  Plastic.cpp
//  
//
//  Created by Macklin Groot on 4/15/16.
//
//

#include "Plastic.h"

Plastic::Plastic(Lambert* diffuse, Specular* specular) :
                 diffuseCmpnt(diffuse), specularCmpnt(specular)
{
    
}

Plastic::~Plastic()
{
    //Nothing in destructor
}

Lambert* Plastic::getDiffuseCmpnt()
{
    return diffuseCmpnt;
}

Specular* Plastic::getSpecularCmpnt()
{
    return specularCmpnt;
}

void Plastic::setDiffuseCmpnt(Lambert* diff)
{
    diffuseCmpnt = diff;
}

void Plastic::setSpecularCmpnt(Specular* spec)
{
    specularCmpnt = spec;
}

Vector3
Plastic::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    return diffuseCmpnt->shade(ray, hit, scene) + specularCmpnt->shade(ray, hit, scene);
}



