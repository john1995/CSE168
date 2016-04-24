//
//  Stone.hpp
//  
//
//  Created by Macklin Groot on 4/24/16.
//
//

#ifndef StoneTex_hpp
#define StoneTex_hpp

#include <stdio.h>
#include "Vector3.h"
#include "Perlin.h"
#include "math.h"
#include "Material.h"
#include "Ray.h"
#include "Texture.hpp"

class StoneTex : public Texture
{
public:
    //Constructor
    StoneTex();
    
    /* NOTE: param ray used to be const, but was made changeable
     * so the number of bounces of a ray could be modified */
    virtual float makeNoise(Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;
    
    float fractal_noise(Vector3& p);
};

#endif /* Stone_hpp */
