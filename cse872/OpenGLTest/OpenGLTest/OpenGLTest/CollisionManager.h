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
   void ApplyImpulses( float dt );

   static float Clamp(float v, float min, float max);

private:
   BOOL CubeCubeCollisionCheck( CCube* cube1, CCube* cube2, vector<vec3> &hitPoints, int &numHitPoints, float &penetration, vec3 &hitNormal );
   void AddCollision( CCube* cube1, CCube* cube2, vec3 &point, vec3 &normal, float penetration);
   BOOL SpanIntersect( CCube* cube1, CCube* cube2, vec3 &axisc, float* minPenetration = NULL, vec3* axisPenetration = NULL, float* pen = NULL );
   void CalculateInterval( CCube* cube, const vec3 &axis, float &min, float &max );
   void CalculateHitPoint( CCube* cube1, CCube* cube2, const float penetration, vec3 &hitNormal, vector<vec3> &hitPoints, int &numHitPoints );
   int GetNumHitPoints( CCube* cube, vec3 &hitNormal, float penetration, vec3 verts[8], int* vertIndexs);
   void ClipFaceFaceVerts(vec3* verts0, int* vertIndexs0, vec3* verts1, int* vertIndexs1, vec3* vertsX, int* numVertsX);
   void ClosestPointLineLine( const vec3* verts0, /* 2 verts */ const vec3* verts1, /* 2 verts */ vec3* vertsX, int &numVertX);
   void SortVertices(vec3* verts0, int* vertIndexs0);
   void ClipLinePlane( const vec3* verts0, /* 2 verts */ const int* vertIndexs0, CCube* cube1, const vec3* verts1, /* 4 verts */ int* vertIndexs1, CCube* cube2, vec3* vertsX, int &numVertX);
   void ClosestPtPointOBB(const vec3& point, CCube* cube, vec3* closestP);
   BOOL VertInsideFace(vec3* verts0, vec3 &p0, float planeErr = 0.0f);
  
   vector<CCube*> m_cubes;
   vector<stCollisionPoints> m_collisionsArray;
};
