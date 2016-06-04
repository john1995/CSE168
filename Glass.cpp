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

Glass::Glass( Vector3 kd, Vector3 ks) :
    k_d(kd), k_s(ks), indexOfRefraction(1.31), transparency(0.5f)
{
    k_s = Vector3(0.8f);
    glassColor.set(0.0, 0.2, 0.4);
}
Glass::Glass() : indexOfRefraction(1.31), transparency(0.5f)
{
    k_d = Vector3();
    k_s = Vector3(0.8f);
    glassColor.set(1.0, 0.32, 0.36);
}

void Glass::setBackgroundColor(const Vector3 color)
{
    bgColor.set(color);
}

Vector3
Glass::shade(Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    //printf("bounce num: %d\n", ray.numBounces);
    Vector3 reflected = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 reflectedColor, refractedColor;
    
    //scale down intensity of light in proportion to num of ray bounces
    //Vector3 attenuation = k_s * ( 1 / (ray.numBounces+1));
    //k_s = 0.7f;
    const Lights *lightlist = scene.lights();
    
    // loop over all of the lights
    Lights::const_iterator lightIter;
    for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
    {
        //find reflected vector, given normal and incident light direction
        PointLight* pLight = *lightIter;
        
        Vector3 l = pLight->position() - hit.P;
        
        // the inverse-squared falloff
        float falloff = l.length2();
        
        // normalize the light direction
        l /= sqrt(falloff);
        
        //printf("light vector: %f, %f, %f\n", l.x, l.y, l.z);
        
        reflected = ray.d - 2.0f * (dot(hit.N, ray.d)) * hit.N;
        
        //Determine if we are entering material or exiting it. If ray direction and normal point roughly in same direction,
        //we are exiting the material.
        bool inside = false;
        Vector3 adjustedNorm;
        
        if(dot(ray.d, hit.N) >= 0)
        {
            inside = true;
            adjustedNorm = hit.N * Vector3(-1.0f);
        }
        else
        {
            adjustedNorm = hit.N;
        }
        //std::cerr << "adjustedNorm: " << adjustedNorm.x << ", " <<
        //adjustedNorm.y << ", " << adjustedNorm.z << std::endl;
        
        /*********** REFLECTION CALCULATION ***************/
        if (ray.numBounces < 10)
        {
            //trace from reflected vector now
            Ray reflect(ray.numBounces + 1);
            reflect.o = hit.P;
            reflect.d = reflected.normalize();
            HitInfo hitReflect;
            
            if (scene.trace(hitReflect, reflect, 0.1f))
            {
                //get color from object hit
                //printf("Bounced reflected ray hit something!\n");
                //printf("Reflected ray hit material: ");
                //hitReflect.material->print();
                reflectedColor = hitReflect.material->shade(reflect, hitReflect, scene);
            }
            else
            {
                //get color from background
                //printf("bounced reflected ray didn't hit anything\n");
                return Vector3(0.0f, 1.0f, 0.0f);
            }
        }
        
        /******************* REFRACTION CALCULATION ********************/
        //See http://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
        //Vector3 wt = n1 * (ray.d - No * dot(ray.d, No)/n2 - No* sqrt(c));
        
        //are we inside or outside of surface? Assuming other medium is air for now, has IoR 1.0
        float n1 = inside ? 1.0 : 1.0f;
        float n2 = inside ? 1.0f : 1.0;
        
        float eta = n1 / n2;
        
        //find cosine of angle between ray and normal
        
        //std::cerr << "adjustedNorm: " << adjustedNorm.x << ", " <<
        //adjustedNorm.y << ", " << adjustedNorm.z << std::endl;
        
        float cosi = dot(adjustedNorm, -ray.d);
        //std::cerr << "cosi: " << cosi << std::endl;
        
        //What's under the square root in equation 22 in article linked above.
        float k = 1 - eta * eta * (1 - cosi * cosi);
        
        float cos_t = 0.0f;
            
        if (k > 0)
        {
            //Compute direction of refracted ray
            Vector3 refractionDir = ray.d * eta + adjustedNorm * (eta *  cosi - sqrt(k));
            
            //std::cerr << "Incident ray direction: " << ray.d.x << ", " << ray.d.y << ", " <<
            //ray.d.z << std::endl;
            
            //std::cerr << "refraction direction: " << refractionDir.x << ", " <<
            //refractionDir.y << ", " << refractionDir.z << std::endl;
            
            refractionDir.normalize();
            
            //find cosine of angle between transmitted ray and normal
            cos_t = dot(-adjustedNorm, refractionDir);
            //std::cerr << "cos_t: " << cos_t << std::endl;
            
            //find new vector direction direction
            /******/
            if (ray.numBounces < 10)
            {
                //printf("calculating light from bounced refracted ray\n");
                //trace from reflected vector now
                Ray refract(ray.numBounces + 1);
                refract.o = hit.P;
                refract.d = refractionDir;
                HitInfo hitRefract;
                
                if (scene.trace(hitRefract, refract, 0.01f))
                {
                    Vector3 color = pLight->color();
                    //printf("Bounced refracted ray hit something!\n");
                    //printf("Refracted ray hit material: ");
                    //hitRefract.material->print();
                    refractedColor = hitRefract.material->shade(refract, hitRefract, scene);
                }
                else
                {
                    //printf("bounced refracted ray didn't hit anything\n");
                    return Vector3(0.0f, 1.0f, 0.0f);
                }
            }
        }
        else
        {
            //printf("Total internal reflection\n");
            refractedColor = Vector3(1.0f, 1.0f, 1.0f);
        }
        
        /****************** FRESNEL CALCULATION ********************/
        //Find fraction of reflected light
        float rho_perp = (n2 * cosi - n1 * cos_t) / (n2 * cosi + n1 * cos_t);
        float rho_parallel = (n1 * cosi - n2 * cos_t) / (n1 * cosi + n2 * cos_t);
        
        float fresnelZero = 0.5f * (pow(rho_perp, 2) + pow(rho_parallel, 2));
        float fracTransmitted = 1 - fresnelZero;
        
       //printf("Fraction reflected: %f\n", fresnelZero);
        //printf("Fraction transmitted: %f\n", fracTransmitted);
        
        /**************** FINAL COLOR **********************/
        //printf("reflectedColor: %f, %f, %f\n", reflectedColor.x, reflectedColor.y, reflectedColor.z);
        //printf("refractedColor: %f, %f, %f\n", refractedColor.x, refractedColor.y, refractedColor.z);
        L = reflectedColor * fresnelZero + refractedColor * fracTransmitted;
        //printf("glass color: %f, %f, %f\n", glassColor.x, glassColor.y, glassColor.z);
        //L = reflectedColor + refractedColor;
        //return L;
        
        
        /*****************  CHECK IF WE ARE IN SHADOW **************/
        Ray shadow_ray(0);
        HitInfo hi;
        
        shadow_ray.o = hit.P;
        shadow_ray.d = l;
        
        //std::cout<<"M = "<<M<< " hit.N = "<<hit.N<<std::endl;
        
        if (scene.trace(hi, shadow_ray, 0.001f, sqrt(falloff)))
        {
            // We are in shadow - zero out shadow vector
            //L.set(0.0f);
            //printf("In shadow!\n");
        }
        else
        {
            //get color of light
            //Vector3 color = pLight->color();
            
            //flip vector from eye so points from hit point back to eye
            //L += k_s * color * pow(std::max(0.0f, dot(h, hit.N)), shinyExp);
            //L += attenuation * color * pow(std::max(0.0f, dot(reflected, -ray.d)), shinyExp);
            
            //Specular Highlights
            //This is separate from the reflection calculation because it
            //needs to be dependent on just the shinyExp
            //https://en.wikipedia.org/wiki/Specular_highlight
            
            //Specular calculation for ABSORBED light
            //L += attenuation * pow(std::max(0.0f, dot(h, hit.N)), 50);
            
            //check entering or exiting and change n1/n2 n2/n1
            //dot product ray.dot.normal
            //Specular Refraction
            //L += attenuation * color * pow(std::max(0.0f, dot(wt, -ray.d)), shinyExp);
            //std::cout<<"Final Refraction vector = "<<(k_s * color * pow(std::max(0.0f,
            //dot(wt,ray.d)), shinyExp)) << std::endl;
        }
    }
    
    //printf("light from glass: %f, %f, %f\n", L.x, L.y, L.z);
    
    return L;
}

void Glass::fresnel(float indexOfRefraction, Vector3 normal, Vector3 rayDir, float &kr, float &kt)
{
    
}

void Glass::print() const 
{
    std::cerr << "Glass" << std::endl;
}