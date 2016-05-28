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
Glass::Glass(){}

void Glass::setBackgroundColor(const Vector3 color)
{
    bgColor.set(color);
}

Vector3
Glass::shade(Ray& ray, const HitInfo& hit, const Scene& scene) const
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
        
        Vector3 l = pLight->position() - hit.P;
        
        reflected = ray.d - 2.0f * (dot(hit.N, ray.d)) * hit.N;
        
        if (ray.numBounces < 0)
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
                //L += attenuation * hitReflect.material->shade(reflect, hitReflect, scene);
            }
            else
            {
                //get color from background
                L += bgColor;
            }
        }
        
        //Get halfway vector
        Vector3 h = (L + -1 * ray.d).normalize();
        
        //n1/n2  refract from air to glass
        float M = 1.0f / 1.31f;
        Vector3 No;
        float n1, n2;
        
        //Check to see if refraction is coming into material or out of it
        if(dot(hit.N,ray.d) < 0){
            n1 = 1.0f;
            n2 = 2.5f;
            No = hit.N;
        }
        else{
            n1 = 2.5f;
            n2 = 1.0f;
            Vector3 neg = Vector3(-1,-1,-1);
            No = -1 * hit.N;
        }
        
        //Calculate Refraction Ray
        //Vector3 wt = -M * (l-(dot(l,hit.N)*hit.N)) -
        // (sqrt(1- pow(M, 2) * (1- pow(dot(l,hit.N),2))))*hit.N;
        float c = 1 - (pow(n1, 2) * (1 - pow(dot(ray.d, No), 2)) / pow(n2, 2));
        if (c < 0)
        {
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
        }
        Vector3 wt = n1 * (ray.d - No * dot(ray.d, No)/n2 - No* sqrt(c));
        //d_r = n1 * (d - n * dot(d, n)) / n2 - n * sqrt(energy);
        //trace from refraction
        if (ray.numBounces < 2)
        {
            //trace from reflected vector now
            Ray refract(ray.numBounces + 1);
            refract.o = hit.P;
            refract.d = wt;
            HitInfo hitRefract;
            
            if (scene.trace(hitRefract, refract, 0.01f))
            {
                Vector3 color = pLight->color();
                //printf("Bounced refracted ray hit something!\n");
                L += hitRefract.material->shade(refract, hitRefract, scene);
                /*Vector3 one = Vector3(1,1,1);
                 float falloff = l.length2();
                 
                 // normalize the light direction
                 Vector3 l2;
                 l2 /= sqrt(falloff);
                 float nDotL = dot(hit.N, l2);
                 Vector3 result = pLight->color();
                 
                 
                 L +=  (one -  attenuation * color * pow(std::max(0.0f, dot(reflected, -ray.d)), shinyExp) - std::max(0.0f, (nDotL * pLight->wattage()) / (PI * falloff)) * result)* hitRefract.material->shade(refract, hitRefract, scene);*/
            }
            else
            {
                //printf("Bounced refracted ray didn't hit anything\n");
                L += bgColor;
            }
        }
        
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
            //L += attenuation * pow(std::max(0.0f, dot(h, hit.N)), 50* shinyExp);
            
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
