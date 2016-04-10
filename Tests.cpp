//
//  Tests.cpp
//  
//
//  Created by Macklin Groot on 4/10/16.
//
//

#include "Tests.hpp"

void Tests::testTriangleIntersection()
{
    int numFailed = 0;
    
    Vector3 origin, direction;
    HitInfo hit;
    
    /* Test 1 - Ray pointing straight towards triangle*/
    origin.x = 0.0f; origin.y = 5.0f; origin.z = 0.0f;
    direction.x = 0.0f; direction.y = -1.0f; direction.z = 0.0f;
    
    Ray r1(origin, direction);
    
    // create the tri triangle
    TriangleMesh * tri = new TriangleMesh;
    tri->createSingleTriangle();
    tri->setV1(Vector3(0, 0,  10));
    tri->setV2(Vector3( 10, 0, -10));
    tri->setV3(Vector3(-10, 0, -10));
    tri->setN1(Vector3(0, 1, 0));
    tri->setN2(Vector3(0, 1, 0));
    tri->setN3(Vector3(0, 1, 0));
    
    Triangle* t = new Triangle;
    t->setIndex(0);
    t->setMesh(tri);
    
    bool intersection = t->intersect(hit, r1);
    
    if (intersection)
        printf("Triangle was intersected by ray 1!\n");
    else
        printf("Triangle was not intersected by ray 1!\n");
    
    //Print stats about hit
    printf("hit point: %f %f %f\ndistance from ray origin: %f\n"
           "normal of triangle: %f %f %f\n\n",
           hit.P[0], hit.P[1], hit.P[2],
           hit.t,
           hit.N[0], hit.N[1], hit.N[2]);
    
    /* Test 2 - Ray in opposite direction of triangle */
    
    direction.x = 0.0f; direction.y = 1.0f; direction.z = 0.0f;
    
    Ray r2(origin, direction);
    
    intersection = t->intersect(hit, r2);
    
    if (intersection)
        printf("Triangle was intersected by ray 2!\n");
    else
        printf("Triangle was not intersected by ray 2!\n");
    
    //Print stats about hit
    printf("hit point: %f %f %f\ndistance from ray origin: %f\n"
           "normal of triangle: %f %f %f\n\n",
           hit.P[0], hit.P[1], hit.P[2],
           hit.t,
           hit.N[0], hit.N[1], hit.N[2]);
    
    /* Test 3 - Ray parallel to triangle */
    
    direction.x = 1.0f; direction.y = 0.0f; direction.z = 0.0f;
    
    Ray r3(origin, direction);
    
    intersection = t->intersect(hit, r3);
    
    if (intersection)
        printf("Triangle 1 was intersected by ray 3!\n");
    else
        printf("Triangle 1 was not intersected by ray 3!\n");
    
    //Print stats about hit
    printf("hit point: %f %f %f\ndistance from ray origin: %f\n"
           "normal of triangle: %f %f %f\n\n",
           hit.P[0], hit.P[1], hit.P[2],
           hit.t,
           hit.N[0], hit.N[1], hit.N[2]);
    
    /* Test 4  - intersecting vertex of triangle */
    
    origin.x = 0.0f; origin.y = 5.0f; origin.z = 10.0f;
    direction.x = 0.0f; direction.y = -1.0f; direction.z = 0.0f;
    
    Ray r4(origin, direction);
    
    intersection = t->intersect(hit, r4);
    
    if (intersection)
        printf("Triangle 1 was intersected by ray 4!\n");
    else
        printf("Triangle 1 was not intersected by ray 4!\n");
    
    //Print stats about hit
    printf("hit point: %f %f %f\ndistance from ray origin: %f\n"
           "normal of triangle: %f %f %f\n\n",
           hit.P[0], hit.P[1], hit.P[2],
           hit.t,
           hit.N[0], hit.N[1], hit.N[2]);
    
    /* Test 5  - At an angle */
    
    tri->setV1(Vector3(0, 0,  0));
    tri->setV2(Vector3( 10, 0, -10));
    tri->setV3(Vector3(-10, 0, -10));
    
    
    origin.x = 0.0f; origin.y = 6.0f; origin.z = 0.0f;
    direction.x = 0.0f; direction.y = -6.0f; direction.z = -5.0f;
    
    Ray r5(origin, direction);
    
    intersection = t->intersect(hit, r5);
    
    if (intersection)
        printf("Triangle 1 was intersected by ray 5!\n");
    else
    {
        printf("Triangle 1 was not intersected by ray 5!\n");
        numFailed++;
    }
    
    //Print stats about hit
    printf("hit point: %f %f %f\ndistance from ray origin: %f\n"
           "normal of triangle: %f %f %f\n\n",
           hit.P[0], hit.P[1], hit.P[2],
           hit.t,
           hit.N[0], hit.N[1], hit.N[2]);

    
    if (numFailed == 0)
        printf("All triangle intersection tests passed!\n");
    else
        printf("Failed %d triangle intersection tests\n", numFailed);
    
    delete t;
}
