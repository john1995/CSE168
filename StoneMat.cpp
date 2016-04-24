//
//  StoneMat.cpp
//  
//
//  Created by Macklin Groot on 4/24/16.
//
//

#include "StoneMat.hpp"

StoneMat::StoneMat(Lambert* diffuse, StoneTex* tex) :
    diffuseCmpnt(diffuse), stoneTexture(tex)
{
    
}

Vector3
StoneMat::shade(Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    Vector3 L = diffuseCmpnt->shade(ray, hit, scene);
    
    float noise = stoneTexture->makeNoise(ray, hit, scene);
    
    return noise * L;
}
