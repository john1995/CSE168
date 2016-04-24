//
//  StoneMat.hpp
//  
//
//  Created by Macklin Groot on 4/24/16.
//
//

#ifndef StoneMat_hpp
#define StoneMat_hpp

#include <stdio.h>
#include "Material.h"
#include "StoneTex.hpp"
#include "Lambert.h"

class StoneMat : public Material
{
public:
    //constructor
    StoneMat(Lambert* diffuse = new Lambert(), StoneTex* tex = new StoneTex());
    
    //shade function
    virtual Vector3 shade(Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
    
protected:
    Lambert* diffuseCmpnt;
    StoneTex* stoneTexture;
};

#endif /* StoneMat_hpp */
