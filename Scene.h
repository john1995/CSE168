#ifndef CSE168_SCENE_H_INCLUDED
#define CSE168_SCENE_H_INCLUDED

#include "Miro.h"
#include "Object.h"
#include "photonmap.h"
#include "PointLight.h"
#include "BVH.h"
//#include <random>

class Camera;
class Image;

class Scene
{
public:
    void addObject(Object* pObj)        {m_objects.push_back(pObj);}
    const Objects* objects() const      {return &m_objects;}

    void addLight(PointLight* pObj)     {m_lights.push_back(pObj);}
    const Lights* lights() const        {return &m_lights;}

    void preCalc();
    void openGL(Camera *cam);

    void raytraceImage(Camera *cam, Image *img);
    bool trace(HitInfo& minHit, const Ray& ray,
               float tMin = 0.0f, float tMax = MIRO_TMAX) const;
    
    void emitPhotons();
    
    bool initPhotonMaps();

protected:
    Objects m_objects;
    BVH m_bvh;
    Lights m_lights;
    Photon_map *globalMap, *volumeMap, *causticMap;
    const int MAX_PHOTONS = 1000;
};

extern Scene * g_scene;

#endif // CSE168_SCENE_H_INCLUDED
