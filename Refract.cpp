//
//  Refract.cpp
//  
//
//  Created by Macklin Groot on 4/27/16.
//
//

#include "Refract.hpp"


Vector3 Refract::shade(Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    //n1/n2  refract from air to glass
    /*float M = 1.0f / 1.31f;
    
    //Check to see if refraction is coming into material or out of it
    if(dot(hit.N,l) > 0) {
        M = 1.0f / 1.31f;
    }
    else {
        M = 1.31f;
    }
    
    //Calculate Refraction Ray
    Vector3 wt = -M * (l-(dot(l,hit.N)*hit.N)) -
     (sqrt(1- pow(M, 2) * (1- pow(dot(l,hit.N),2))))*hit.N;
     
     //trace from refraction
     if (ray.numBounces < maxBounces)
     {
     //trace from reflected vector now
     Ray refract(ray.numBounces + 1);
     refract.o = hit.P;
     refract.d = wt;
     HitInfo hitRefract;
     
     if (scene.trace(hitRefract, refract, 0.01f))
     {
     Vector3 color = pLight->color();
     printf("Bounced refracted ray hit something!\n");
     L += hitRefract.material->shade(refract, hitRefract, scene);
     }
     else
     {
     printf("Bounced refracted ray didn't hit anything\n");
     L += bgColor;
     }
     }*/
    
    return Vector3(0.0f, 0.0f, 0.0f);
}