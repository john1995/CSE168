//
//  Glass.cpp
//  
//
//  Created by Macklin Groot on 4/27/16.
//
//

#include "Glass.hpp"
#include "Ray.h"
#include "Scene.h"

Glass::Glass(float kt, float kd, float ks) :
    k_d(kd), k_s(ks), k_t(kt)
{
    
}

void Glass::setBackgroundColor(const Vector3 color)
{
    bgColor.set(color);
}

Vector3
Glass::shade(Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 reflected = Vector3(0.0f, 0.0f, 0.0f);
    
    const Vector3 viewDir = -ray.d; // d is a unit vector
    
    const Lights *lightlist = scene.lights();
    
    // loop over all of the lights
    Lights::const_iterator lightIter;
    for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
    {
        PointLight* pLight = *lightIter;
        
        //Find light vector. points from hit point to light
        Vector3 l = pLight->position() - hit.P;
        
        // the inverse-squared falloff
        float falloff = l.length2();
        
        // normalize the light direction
        l /= sqrt(falloff);
        
        // get the diffuse component
        float nDotL = dot(hit.N, l);
        Vector3 result = pLight->color();
        result *= k_d;
        
        //calculations necessary for reflection component
        //why did we use ray.d here instead of l? Would just need to change calculation a bit
        //to use l
        reflected = ray.d - 2.0f * (dot(hit.N, ray.d)) * hit.N;
        
        //calculations necessary for refraction/transmission component
        //n1/n2  refract from air to glass
        float M = 1.0f / 1.31f;
         
        //Check to see if refraction is coming into material or out of it
        if(dot(hit.N,l) > 0) {
            M = 1.0f / 1.31f;
        }
        else {
            M = 1.31f;
        }
        
        Vector3 omega = l.normalize();
        
        float cos_n1 = dot(omega, hit.N);
        float cos_n2 = sqrt(1 - pow(M, 2) * (1 - pow(cos_n1, 2)));
        //Vector3 wt = ray.d * M + (M * cos_n1 - cos_n2) * hit.N;
        
        //Calculate Refraction Ray
        Vector3 wt = -M * (omega-(dot(omega,hit.N)*hit.N)) -
        (sqrt(1- pow(M, 2) * (1- pow(dot(omega,hit.N),2))))*hit.N;
        
        //recursive ray tracing - calculate reflection and refraction bounces
        if (ray.numBounces < maxBounces)
        {
            //trace from reflected vector
            /*Ray reflect(ray.numBounces + 1);
            reflect.o = hit.P;
            reflect.d = reflected;
            HitInfo hitReflect;*/
            
            //trace from refracted vector
            Ray refract(ray.numBounces + 1);
            refract.o = hit.P;
            refract.d = wt;
            HitInfo hitRefract;
            
            if (scene.trace(hitRefract, refract, 0.008f))
            {
                //get color from object hit
                //printf("Bounced refracted ray hit something!\n");
                //reflection addition
                L += k_t * hitRefract.material->shade(refract, hitRefract, scene);
            }
            else
            {
                //get color from background
                L += bgColor;
            }
        }
        
        Ray shadow_ray(0);
        HitInfo hi;
        
        shadow_ray.o = hit.P;
        shadow_ray.d = l;
        
        if (scene.trace(hi, shadow_ray, 0.001f, sqrt(falloff)))
        {
            // We are in shadow
            L = Vector3(0.0f, 0.0f, 0.0f);
        }
        else
        {
            //Should be a 4.0 factor in PI * falloff, but image is too dark
            L += std::max(0.0f, (nDotL * pLight->wattage()) / (PI * falloff)) * result;
        }
    }
    
    // add the ambient component
    L += k_a;
    
    return L;
    
}