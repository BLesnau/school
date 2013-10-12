#pragma once

#include <vector>
#include "Cube.h"

using namespace std;

struct stPoint
{
   vec3 point;
   vec3 normal;
   float pen;
   vec3 pos0;
   vec3 pos1;
};

// Collision information
struct stCollisionPoints
{
   CCube* box0;
   CCube* box1;
   vector<stPoint> points;
   int			numPoints;
};

//struct stCollision
//{
//   void Clear();
//   void Add( CCube* box0,
//      CCube* box1,
//      vec3& point,
//      vec3& normal,
//      float pen);	
//};

class CCollisionManager
{
public:
   CCollisionManager();
   ~CCollisionManager();

   void AddCube(CCube* cube);
   void DetectCollisions();

private:
   BOOL CubeCubeCollisionCheck( CCube* cube1, CCube* cube2, vector<vec3> &hitPoints, int &numHitPoints, float &penetration, vec3 &hitNormal );
   void AddCollision( CCube* cube1, CCube* cube2, vec3 &point, vec3 &normal, float penetration);

   vector<CCube*> m_cubes;
   vector<stCollisionPoints> m_collisionsArray;
};

