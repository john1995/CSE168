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

void Specular::setBackgroundColor(const Vector3 color)
{
    bgColor.set(color);
}

void Specular::setMaxBounces(const int max)
{
    maxBounces = max;
}

//Blinn-Phong shading model
Vector3
Specular::shade(Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    Vector3 reflected = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
    
    //scale down intensity of light in proportion to num of ray bounces
    Vector3 attenuation = k_s * ( 1 / (ray.numBounces+1));
    
    const Lights *lightlist = scene.lights();
    
    // loop over all of the lights
    Lights::const_iterator lightIter;
    for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
    {
        //find reflected vector, given normal and incident light direction
        PointLight* pLight = *lightIter;
        
        //light vector points from hit point to light
        Vector3 l = pLight->position() - hit.P;
        
        //why did we use ray.d here instead of l? Would just need to change calculation a bit
        //to use l
        reflected = ray.d - 2.0f * (dot(hit.N, ray.d)) * hit.N;

        if (ray.numBounces < maxBounces)
        {
            //trace from reflected vector now
            Ray reflect(ray.numBounces + 1);
            reflect.o = hit.P;
            reflect.d = reflected;
            HitInfo hitReflect;
        
            if (scene.trace(hitReflect, reflect, 0.008f))
            {
                //get color from object hit
                //printf("Bounced reflected ray hit something!\n");
                L += attenuation * hitReflect.material->shade(reflect, hitReflect, scene);
            }
            else
            {
                //get color from background
                L += bgColor;
            }
        }

        //Get halfway vector
        Vector3 h = (L + -1 * ray.d).normalize();
        
        Ray shadow_ray(0);
        HitInfo hi;
        
        shadow_ray.o = hit.P;
        shadow_ray.d = l;
        
        //std::cout<<"M = "<<M<< " hit.N = "<<hit.N<<std::endl;

        if (scene.trace(hi, shadow_ray, 0.001f, sqrt(l.length2())))
        {
            // We are in shadow
        }
        else
        {
            //get color of light
            Vector3 color = pLight->color();
            
            //flip vector from eye so points from hit point back to eye
            //L += k_s * color * pow(std::max(0.0f, dot(h, hit.N)), shinyExp);
            //L += attenuation * color * pow(std::max(0.0f, dot(reflected, -ray.d)), shinyExp);
            
            //Specular Highlights
            //This is separate from the reflection calculation because it
            //needs to be dependent on just the shinyExp
            //https://en.wikipedia.org/wiki/Specular_highlight
            
            //Specular calculation for ABSORBED light
            L += attenuation * pow(std::max(0.0f, dot(h, hit.N)), 50* shinyExp);

	    //check entering or exiting and change n1/n2 n2/n1
           //dot product ray.dot.normal  
            //Specular Refraction
            //L += attenuation * color * pow(std::max(0.0f, dot(wt, -ray.d)), shinyExp);
            //std::cout<<"Final Refraction vector = "<<(k_s * color * pow(std::max(0.0f,
            //dot(wt,ray.d)), shinyExp))<<std::endl;
        }
    }
    
    return L;
}
