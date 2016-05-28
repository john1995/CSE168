#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"
#include <random>
#include "photonmap.h"

Scene * g_scene = 0;


std::default_random_engine generator;
std::uniform_real_distribution<float> distribution(-1, 1);
std::uniform_real_distribution<float> distribution2(0, 1);

void
Scene::openGL(Camera *cam)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cam->drawGL();
    m_bvh.drawBVH();

    // draw objects
    for (size_t i = 0; i < m_objects.size(); ++i)
        m_objects[i]->renderGL();

    glutSwapBuffers();
}

void
Scene::preCalc()
{
    Objects::iterator it;
    for (it = m_objects.begin(); it != m_objects.end(); it++)
    {
        Object* pObject = *it;
        pObject->preCalc();
    }
    Lights::iterator lit;
    for (lit = m_lights.begin(); lit != m_lights.end(); lit++)
    {
        PointLight* pLight = *lit;
        pLight->preCalc();
    }

    m_bvh.build(&m_objects);
    
    m_bvh.printStatistics();
}

void
Scene::raytraceImage(Camera *cam, Image *img)
{
    Ray ray,photray;
    HitInfo hitInfo;
    Vector3 shadeResult;
    float r1 = distribution(generator);
    float r2 = distribution(generator);
    float r3 = distribution(generator);

    
    // loop over all pixels in the image
    for (int j = 0; j < img->height(); ++j)
    {
        for (int i = 0; i < img->width(); ++i)
        {
            ray = cam->eyeRay(i, j, img->width(), img->height());
            ray.numBounces = 0;
            if (trace(hitInfo, ray))
            {
                shadeResult = hitInfo.material->shade(ray, hitInfo, *this);
                img->setPixel(i, j, shadeResult);
            }
        }
        img->drawScanline(j);
        glFinish();
        printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
        fflush(stdout);
    }
    float x;
    float y;
    float z;
    int numPhotons = 0;
    while (numPhotons < 1000) {
        do{
            x = distribution(generator);
            y = distribution(generator);
            z = distribution(generator);
        }while((x*x + y*y + z*z)> 1.0f);
         
        const Lights *lightlist = lights();
        PointLight * pLight = *(lightlist->begin());
        Vector3 lightpos = pLight->position();
        photray.d = Vector3(x,y,z);
        photray.o = lightpos;
        HitInfo phothit;
        Photon photon;
        
        if (trace(phothit, ray))
        {
            
        }
        numPhotons += 1;
        
        photon.pos[0] = phothit.P.x;
        photon.pos[1] = phothit.P.y;
        photon.pos[2] = phothit.P.z;
        photon.power[0] = pLight->wattage();
        photon.power[1] = pLight->wattage();
        photon.power[2] = pLight->wattage();

        //Russian roulette
        
        
        
        
    }
    
    
    
    printf("Rendering Progress: 100.000 percent\n");
    debug("done Raytracing!\n");
    
    m_bvh.printStatistics();
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    return m_bvh.intersect(minHit, ray, tMin, tMax);
}
