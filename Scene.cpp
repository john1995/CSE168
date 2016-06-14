#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"
#include <random>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "Lambert.h"
Scene * g_scene = 0;


//std::default_random_engine generator;
//std::uniform_real_distribution<float> distribution(-1, 1);
//std::uniform_real_distribution<float> distribution2(0, 1);

void
Scene::openGL(Camera *cam)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cam->drawGL();
    //m_bvh.drawBVH();

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
    Ray ray;
    HitInfo hitInfo;
    Vector3 shadeResult;

    // loop over all pixels in the image
    /*for (int j = 0; j < img->height(); ++j)
    {
        for (int i = 0; i < img->width(); ++i)
        {
            ray = cam->eyeRay(i, j, img->width(), img->height());
            ray.numBounces = 0;
            if (trace(hitInfo, ray))
            {
                shadeResult = hitInfo.material->shade(ray, hitInfo, *this);
                //printf("shade result: %f, %f, %f\n", shadeResult.x, shadeResult.y, shadeResult.z);
                img->setPixel(i, j, shadeResult);
            }
        }
        img->drawScanline(j);
        glFinish();
        printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
        fflush(stdout);
    }*/

    
    /********** FIRST PASS - Emit photons ****************/
    printf("Starting photon map construction.\n");
    emitPhotons();
    //globalMap->scale_photon_power(1.0f/((float)MAX_PHOTONS));
    //causticMap->scale_photon_power(1.0f/(float)MAX_PHOTONS);

    globalMap->balance();
    causticMap->balance();
    /********** SECOND PASS - Trace image ****************/
    
    float irG[3];
    float irC[3];

    float posi[3];
    float nor[3];
    Vector3 globalIR,causticIR;
    ////////////////////////////Default Tracer/////////////////////////
    // loop over all pixels in the image
    for (int j = 0; j < img->height(); ++j)
    {
        for (int i = 0; i < img->width(); ++i)
        {
            ray = cam->eyeRay(i, j, img->width(), img->height());
            ray.numBounces = 0;
            if (trace(hitInfo, ray))
            {
                posi[0] = hitInfo.P.x;
                posi[1] = hitInfo.P.y;
                posi[2] = hitInfo.P.z;
                nor[0] = hitInfo.N.x;
                nor[1] = hitInfo.N.y;
                nor[2] = hitInfo.N.z;
                globalMap->irradiance_estimate(irG, posi, nor, 0.1f, 10000);
                causticMap->irradiance_estimate(irC, posi, nor, 0.1f, 10000);
                
                //std::cout << irC[0] << " " << irC[1] << " " << irC[2] << " " << std::endl;
                
                globalIR.set(irG[0],irG[1],irG[2]);
                causticIR.set(irC[0],irC[1],irC[2]);
                //std::cout<<globalIR.x<<" "<<globalIR.y<<" "<<globalIR.z<< " "<<std::endl;
                //std::cout<<causticIR.x<<" "<<causticIR.y<<" "<<causticIR.z<< " "<<std::endl;

                shadeResult = hitInfo.material->shade(ray, hitInfo, *this) + globalIR + causticIR;
                img->setPixel(i, j, shadeResult);
            }
        }
        img->drawScanline(j);
        glFinish();
        printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
        fflush(stdout);
    }
    ////////////////////////////DOF//////////////////////////////////
    /*for (int j = 0; j < img->height(); ++j)
    {
        for (int i = 0; i < img->width(); ++i)
        {
            for(int k = 0; k < 16; k++)
            {

                ray = cam->eyeRay(i, j, img->width(), img->height());
                ray.numBounces = 0;
                if (trace(hitInfo, ray))
                {
                    shadeResult += hitInfo.material->shade(ray, hitInfo, *this);
                }
            }
            shadeResult /= 16.0f;
            img->setPixel(i, j, shadeResult);
            shadeResult = Vector3(0.0f);
        }
        img->drawScanline(j);
        glFinish();
        printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
        fflush(stdout);
    }*/
    
    
    printf("Rendering Progress: 100.000 percent\n");
    debug("done Raytracing!\n");
    
    m_bvh.printStatistics();
}

void Scene::emitPhotons()
{
    //create photon maps
    initPhotonMaps();

	/* initialize random seed: */
	srand(time(NULL));
    
    //float r1 = distribution2(generator);
    //float r2 = distribution2(generator);
    Ray photRay;
    
    // loop over all of the lights, emit from each.
    Lights::const_iterator lightIter;
    for (lightIter = m_lights.begin(); lightIter != m_lights.end(); lightIter++)
    {
        //find reflected vector, given normal and incident light direction
        PointLight* pLight = *lightIter;
        
        float x;
        float y;
        float z;
        int numPhotons = 0;
        Photon phot;
        
        //start power all white with wattage of light
        /*float* power = new float[3];
        power[0] = pLight->wattage();
        power[1] = pLight->wattage();
        power[2] = pLight->wattage();*/
        
        //find suitable direction to emit photons
        while (numPhotons < MAX_PHOTONS)
        {
            phot.flag = 0.0f;
            do
            {
                //x = distribution(generator);
                //y = distribution(generator);
                //z = distribution(generator);
				x = 2 * ((float)rand() / RAND_MAX) - 1;
				y = 2 * ((float)rand() / RAND_MAX) - 1;
				z = 2 * ((float)rand() / RAND_MAX) - 1;
                //std::cout<<x<<" "<<y<<" "<<z<< " "<<std::endl;

            } while((x*x + y*y + z*z)> 1.0f);

			//printf("x: %f, y: %f, z: %f\n", x, y, z);
            
            //construct initial photon
            photRay.d = Vector3(x,y,z);
			photRay.d.normalize();
            photRay.o = pLight->position();
            HitInfo photonHit;
			//FIGURE OUT WHERE TO DO SCALING
            phot.power[0] = pLight->wattage() / MAX_PHOTONS;
            phot.power[1] = pLight->wattage() / MAX_PHOTONS;
            phot.power[2] = pLight->wattage() / MAX_PHOTONS;

            tracePhoton(photonHit, photRay, phot, 0);
            
            numPhotons++;
        }
        //std::cout<<"numPhotons "<< numPhotons<<std::endl;

    }
}

bool Scene::tracePhoton(HitInfo& photonHit, Ray& photRay, Photon& photon, unsigned int numBounces)
{
    
    //trace photon like its a ray
    if (trace(photonHit, photRay))
    {
        float pos[3] = { photonHit.P.x, photonHit.P.y, photonHit.P.z };
        float dir[3] = { photRay.d.x, photRay.d.y, photRay.d.z };
        
        
        //store photon when the hitpoint is diffuse
        if (dynamic_cast<const Lambert*>(photonHit.material) && photon.flag == 1.0f){
            globalMap->store(photon.power, pos, dir);
            causticMap->store(photon.power, pos, dir);
            //printf("store diffuse and specular\n");
			//std::cerr << "Caustic #photons stored: " << causticMap->stored_photons << std::endl;
        }
        else if(dynamic_cast<const Lambert*>(photonHit.material)){
            //printf("store diffuse\n");
            globalMap->store(photon.power, pos, dir);
            //std::cerr<<"Lambert #photons stored: "<<globalMap->stored_photons<<std::endl;
        }
        
        //Russian roulette - Decide whether to reflect or to terminate
        float dr = photonHit.material->k_d.x;
        float dg = photonHit.material->k_d.y;
        float db = photonHit.material->k_d.z;
        float sr = photonHit.material->k_s.x;
        float sg = photonHit.material->k_s.y;
        float sb = photonHit.material->k_s.z;
        
        float Pr = std::max(dr+sr, std::max(dg+sg, db+sb));
        float Pd = ( (dr+dg+db)/(dr+dg+db+sr+sg+sb) ) * Pr;
        float Ps = Pr - Pd;
        //float dec = distribution2(generator);
		float randNum = (float)rand() / RAND_MAX;
        
        //std::cout<<"Dec: "<<dec<<std::endl;
        
        //std::cout<<"Pr: "<<Pr<<std::endl;
        //std::cout<<"Pd: "<<Pd<<std::endl;
        //std::cout<<"Ps: "<<Ps<<std::endl;
        
        //std::cout<<"sr: "<<sr<<" sg: "<<sg<<" sb: "<<sb<<std::endl;

        
        //diffuse reflection
        if(randNum < Pd && numBounces < 10)
		{
            //printf("Diffuse reflection, numBounces = %d\n", numBounces);
            Vector3 reflected = photRay.d - 2.0f * (dot(photonHit.N, photRay.d)) * photonHit.N;
            Ray difrefl(photonHit.P,reflected.normalize());
            photon.power[0] = photon.power[0]*(dr/Pd);
            photon.power[1] = photon.power[1]*(dg/Pd);
            photon.power[2] = photon.power[2]*(db/Pd);
            //std::cout<<"Dg: "<<dg<<std::endl;

            //std::cout<<"Power: "<<photon.power[1]<<std::endl;
            //printf("pathtrace");

            return tracePhoton(photonHit,difrefl,photon,numBounces+1);
            
        }
        //specular reflection
        else if(randNum < Ps + Pd && numBounces < 10)
		{
            //printf("Specular reflection\n");
            Vector3 reflected = photRay.d - 2.0f * (dot(photonHit.N, photRay.d)) * photonHit.N;
            Ray difrefl(photonHit.P,reflected);
            photon.power[0] = photon.power[0]*(sr/Ps);
            photon.power[1] = photon.power[1]*(sg/Ps);
            photon.power[2] = photon.power[2]*(sb/Ps);
            photon.flag = 1.0f;
            return tracePhoton(photonHit,difrefl,photon, numBounces+1);
            
        }
        //Absortion
        else
		{
            //printf("absorption\n");
            return true;
            
        }
    }
    return false;
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    return m_bvh.intersect(minHit, ray, tMin, tMax);
}

bool Scene::initPhotonMaps()
{
    globalMap = new Photon_map(1000000);
    //volumeMap = new Photon_map(1000);
    causticMap = new Photon_map(1000000);
    
    return true;
}
