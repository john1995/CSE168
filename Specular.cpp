//
//  Specular.cpp
//  
//
//  Created by Macklin Groot on 4/15/16.
//
//

#include "Specular.h"
#include "Ray.h"
#include "Scene.h"

Specular::Specular(const Vector3 & ks, const float exp) :
            k_s(ks), shinyExp(exp)
{
    
}

Specular::~Specular()
{
    //Nothing in destructor
}

Vector3 Specular::getSpecScaleFactor()
{
    return k_s;
}

void Specular::setSpecScaleFactor(Vector3 scaleFac)
{
    k_s = scaleFac;
}

float Specular::getShinyExp()
{
    return shinyExp;
}

void Specular::setShinyExp(float newExp)
{
    shinyExp = newExp;
}

Vector3
Specular::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    Vector3 reflected = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
    
    const Lights *lightlist = scene.lights();
    
    // loop over all of the lights
    Lights::const_iterator lightIter;
    for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
    {
        //find reflected vector, given normal and incident light direction
        PointLight* pLight = *lightIter;
        
        Vector3 l = pLight->position() - hit.P;
        
        reflected = -2.0f * (dot(hit.N, l)) * hit.N + l;
        
        //get color of light
        Vector3 color = pLight->color();
        
        Ray shadow_ray;
        HitInfo hi;
        
        shadow_ray.o = hit.P;
        shadow_ray.d = l;
        
        if (scene.trace(hi, shadow_ray, 0.001f, sqrt(l.length2())))
        {
            // We are in shadow
        }
        else
        {
            //flip vector from eye so points from hit point back to eye
            L += k_s * color * pow(dot(reflected, -1 * ray.d), shinyExp);
        }
    }
    
    return L;
}