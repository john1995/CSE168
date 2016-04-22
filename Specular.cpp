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

//Blinn-Phong shading model
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
        
        reflected = ray.d - 2.0f * (dot(hit.N, ray.d)) * hit.N;

	//trace from reflected vector now
        Ray re;
	re.o = hit.P;
 	re.d = reflected;

	if (hit.numBounces > HitInfo::MAX_BOUNCES)
	{
	    if (scene.trace(hit, re))
	    {
		hit.numBounces++;
	        L += shade(re, bounces, scene);
	    }
	}
	else
	{
	    hit.numBounces = 0;
	}

        //Get halfway vector
        Vector3 h = (L + -1 * ray.d).normalize();
        
        //get color of light
        Vector3 color = pLight->color();
        
        Ray shadow_ray;
        HitInfo hi;
        
        shadow_ray.o = hit.P;
        shadow_ray.d = l;

	//n1/n2  refract from air to glass
	float M = 1.0f / 1.5f;

	//Calculate Refraction
	Vector3 wt = (- M) * (l-(dot(l,hit.N)*hit.N)) - (sqrt(1- pow((M),2) * (1- pow(dot(l,hit.N),2))))*hit.N;
	//std::cout<<"M = "<<M<< " hit.N = "<<hit.N<<std::endl;

        if (scene.trace(hi, shadow_ray, 0.001f, sqrt(l.length2())))
        {
            // We are in shadow
        }
        else
        {
            //flip vector from eye so points from hit point back to eye
            //L += k_s * color * pow(std::max(0.0f, dot(h, hit.N)), shinyExp);
	     L += k_s * color * pow(std::max(0.0f, dot(reflected, -ray.d)), shinyExp);

	    //Specular Highlights
	    //This is separate from the reflection calculation because it
            //needs to be dependent on just the shinyExp
	    //https://en.wikipedia.org/wiki/Specular_highlight
	    //L += pow(std::max(0.0f, dot(h, hit.N)), shinyExp);

	    //Specular Refraction
	    L += k_s * color * pow(std::max(0.0f, dot(wt, -ray.d)), shinyExp);
	    //std::cout<<"Final Refraction vector = "<<(k_s * color * pow(std::max(0.0f, dot(wt, -ray.d)), shinyExp))<<std::endl;
	}

    }
    
    return L;
}
