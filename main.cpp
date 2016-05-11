#include <math.h>
#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"

#include "PointLight.h"
#include "Sphere.h"
#include "TriangleMesh.h"
#include "Triangle.h"
#include "Lambert.h"
#include "Plastic.h"
#include "Mirror.hpp"
#include "StoneMat.hpp"
#include "MiroWindow.h"
#include "Tests.hpp"
#include "Glass.hpp"

void
makeSpiralScene()
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(512, 512);
    
    // set up the camera
    g_camera->setBGColor(Vector3(1.0f, 1.0f, 1.0f));
    g_camera->setEye(Vector3(-5, 2, 3));
    g_camera->setLookAt(Vector3(0, 0, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);

    // create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(-3, 15, 3));
    light->setColor(Vector3(1, 1, 1));
    light->setWattage(1000);
    g_scene->addLight(light);

    // create a spiral of spheres
    Material* mat = new Lambert(Vector3(0.0f, 0.6f, 1.0f));;
    const int maxI = 100;
    const float a = 0.15f;
    for (int i = 1; i < maxI; ++i)
    {
        float t = i/float(maxI);
        float theta = 4*PI*t;
        float r = a*theta;
        float x = r*cos(theta);
        float y = r*sin(theta);
        float z = 2*(2*PI*a - r);
        Sphere * sphere = new Sphere;
        sphere->setCenter(Vector3(x,y,z));
        sphere->setRadius(r/10);
        sphere->setMaterial(mat);
        g_scene->addObject(sphere);
    }
    
    // let objects do pre-calculations if needed
    g_scene->preCalc();
}


void
makeBunnyScene()
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(128, 128);
    
    // set up the camera
    g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
    g_camera->setEye(Vector3(-2, 3, 5));
    g_camera->setLookAt(Vector3(-.5, 1, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);

    // create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(-3, 15, 3));
    light->setColor(Vector3(1, 1, 1));
    light->setWattage(500);
    g_scene->addLight(light);

    Material* mat = new Lambert(Vector3(1.0f));

    TriangleMesh * bunny = new TriangleMesh;
    bunny->load("bunny.obj");
    
    // create all the triangles in the bunny mesh and add to the scene
    for (int i = 0; i < bunny->numTris(); ++i)
    {
        Triangle* t = new Triangle;
        t->setIndex(i);
        t->setMesh(bunny);
        t->setMaterial(mat); 
        g_scene->addObject(t);
    }
    
    // create the floor triangle
    TriangleMesh * floor = new TriangleMesh;
    floor->createSingleTriangle();
    floor->setV1(Vector3(  0, 0,  10));
    floor->setV2(Vector3( 10, 0, -10));
    floor->setV3(Vector3(-10, 0, -10));
    floor->setN1(Vector3(0, 1, 0));
    floor->setN2(Vector3(0, 1, 0));
    floor->setN3(Vector3(0, 1, 0));
    
    Triangle* t = new Triangle;
    t->setIndex(0);
    t->setMesh(floor);
    t->setMaterial(mat); 
    g_scene->addObject(t);
    
    // let objects do pre-calculations if needed
    g_scene->preCalc();
}

void
makeTeapotScene()
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;
    
    g_image->resize(128, 128);
    
    // set up the camera
    g_camera->setBGColor(Vector3(0.0f, 0.2f, 0.6f));
    g_camera->setEye(Vector3(2, 3, 6));
    g_camera->setLookAt(Vector3(0, 0, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);
    
    // create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(-3, 15, 3));
    light->setColor(Vector3(1.0f, 1.0f, 1.0f));
    light->setWattage(500);
    g_scene->addLight(light);
    
    Plastic* teapotMat1 = new Plastic();
    
    teapotMat1->getSpecularCmpnt()->setBackgroundColor(Vector3(0.0f, 0.2f, 0.6f));
    
    printf("specular exponent: %f\n",
           teapotMat1->getSpecularCmpnt()->getShinyExp());
    
    TriangleMesh * teapot1 = new TriangleMesh;
    teapot1->load("teapot.obj");
    
    // create all the triangles in the bunny mesh and add to the scene
    for (int i = 0; i < teapot1->numTris(); ++i)
    {
        Triangle* t = new Triangle;
        t->setIndex(i);
        t->setMesh(teapot1);
        t->setMaterial(teapotMat1);
        g_scene->addObject(t);
    }
    
    teapot1->translate(Vector3(2.0f, 0.0f, -2.0f));
    
    Mirror* teapotMat2 = new Mirror();
    
    teapotMat2->getSpecularCmpnt()->setBackgroundColor(Vector3(0.0f, 0.2f, 0.6f));
    
    TriangleMesh * teapot2 = new TriangleMesh;
    teapot2->load("teapot.obj");
    
    // create all the triangles in the bunny mesh and add to the scene
    for (int i = 0; i < teapot2->numTris(); ++i)
    {
        Triangle* t = new Triangle;
        t->setIndex(i);
        t->setMesh(teapot2);
        t->setMaterial(teapotMat2);
        g_scene->addObject(t);
    }
    
    teapot2->translate(Vector3(-2.0f, 0.0f, 2.0f));
    
    Glass* teapotMat3 = new Glass(0.0f, 0.0f, 1.0f);
    
    teapotMat3->setBackgroundColor(Vector3(0.0f, 0.2f, 0.6f));
    
    TriangleMesh * teapot3 = new TriangleMesh;
    teapot3->load("teapot.obj");
    
    // create all the triangles in the bunny mesh and add to the scene
    for (int i = 0; i < teapot3->numTris(); ++i)
    {
        Triangle* t = new Triangle;
        t->setIndex(i);
        t->setMesh(teapot3);
        t->setMaterial(teapotMat3);
        g_scene->addObject(t);
    }
    
    teapot3->translate(Vector3(0.0f, 0.0f, 3.0f));
    
    //Create material for floor
    StoneMat* floorMat = new StoneMat();

    // create the floor triangle
    TriangleMesh * floor = new TriangleMesh;
    floor->createSingleTriangle();
    floor->setV1(Vector3(  0, 0,  10));
    floor->setV2(Vector3( 10, 0, -10));
    floor->setV3(Vector3(-10, 0, -10));
    floor->setN1(Vector3(0, 1, 0));
    floor->setN2(Vector3(0, 1, 0));
    floor->setN3(Vector3(0, 1, 0));
    
    Triangle* t = new Triangle;
    t->setIndex(0);
    t->setMesh(floor);
    t->setMaterial(floorMat);
    g_scene->addObject(t);
    
    // let objects do pre-calculations if needed
    g_scene->preCalc();
}



int
main(int argc, char*argv[])
{
    Tests tester;
    
    tester.testTriangleIntersection();
    
    // create a scene
    makeTeapotScene();

    MiroWindow miro(&argc, argv);
    miro.mainLoop();

    return 0; // never executed
}

