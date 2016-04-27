//
//  Mirror.cpp
//  
//
//  Created by Macklin Groot on 4/26/16.
//
//

#include "Mirror.hpp"

Mirror::Mirror(Specular* specular) :
    specularCmpnt(specular)
{
    //set maximum number of bounces specular cmpnt can have
    specularCmpnt->setMaxBounces(MAX_BOUNCES);
}

Mirror::~Mirror()
{
    //Nothing in destructor
}

Specular* Mirror::getSpecularCmpnt()
{
    return specularCmpnt;
}

void Mirror::setSpecularCmpnt(Specular* spec)
{
    specularCmpnt = spec;
}

Vector3
Mirror::shade(Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    return specularCmpnt->shade(ray, hit, scene);
}
