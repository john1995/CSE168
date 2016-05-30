#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>

Lambert::Lambert(const Vector3 & kd, const Vector3 & ka) :
    Material(kd,Vector3(0.0f)), k_a(ka)
{

}

Lambert::~Lambert()
{
}

Vector3
Lambert::shade(Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);
    
    const Vector3 viewDir = -ray.d; // d is a unit vector
    
    const Lights *lightlist = scene.lights();
    
    // loop over all of the lights
    Lights::const_iterator lightIter;
    for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
    {
        PointLight* pLight = *lightIter;
    
        Vector3 l = pLight->position() - hit.P;
        
        // the inverse-squared falloff
        float falloff = l.length2();
        
        // normalize the light direction
        l /= sqrt(falloff);

        // get the diffuse component
        float nDotL = dot(hit.N, l);
        Vector3 result = pLight->color();
        result *= k_d;
        
        Ray shadow_ray(0);
        HitInfo hi;
        
        //shadow_ray.o = hit.P;
        //shadow_ray.d = l;
        
        if (scene.trace(hi, shadow_ray, 0.001f, sqrt(falloff)))
        {
            // We are in shadow
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
