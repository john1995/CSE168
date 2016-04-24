//
//  Stone.cpp
//  
//
//  Created by Macklin Groot on 4/24/16.
//
//

#include "StoneTex.hpp"

StoneTex::StoneTex()
{
    
}

float StoneTex::makeNoise(Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    //2 methods to generate noise - Perlin or fractal
    float n = PerlinNoise::noise(hit.P.x, hit.P.y, hit.P.z);
    //float n = fractal_noise(hit.P);
    Vector3 x = hit.P * 4.0f;   //Scaled to add detail
    float marble = fabs(sin(x.y + n));
    
    return marble;
}

float StoneTex::fractal_noise(Vector3& p)
{
    float f = 0.0f;
    float freq = 1.0f;
    
    for (int i = 0; i < 4; i++)
    {
        f += PerlinNoise::noise(p.x * freq, p.y * freq, p.z * freq) * (1.0f / freq);
        
        freq *= 2.0f;
    }
    
    return f;
}
