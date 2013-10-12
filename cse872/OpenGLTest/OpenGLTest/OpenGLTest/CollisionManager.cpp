#include "stdafx.h"
#include "CollisionManager.h"


CCollisionManager::CCollisionManager()
{
}


CCollisionManager::~CCollisionManager()
{
}

void CCollisionManager::AddCube( CCube* cube )
{
   m_cubes.push_back( cube );
}

void CCollisionManager::DetectCollisions()
{
   m_collisionsArray.clear();

   for( int i=0; i<m_cubes.size(); i++ )
   {
      for( int j=0; j<m_cubes.size(); j++ )
      {
         if( i != j )
         {
            vector<vec3> hitPoints;
            int numHitPoints = 0;
            float penetrationBox = 0.0f;
            vec3 hitNormalBox;
            
            bool hitBox = CubeCubeCollisionCheck( m_cubes[i], m_cubes[j], hitPoints, numHitPoints, penetrationBox, hitNormalBox );

            if (hitBox)
            {
               for (int k=0; k<numHitPoints; k++)
               {
                  AddCollision( m_cubes[i], m_cubes[j], hitPoints[k], hitNormalBox, penetrationBox );
               }
            }
         }
      }
   }
}

BOOL CCollisionManager::CubeCubeCollisionCheck( CCube* cube1, CCube* cube2, vector<vec3> &hitPoints, int &numHitPoints, float &penetration, vec3 &hitNormal )
{
   // Simple bounding sphere check first
   float len = (cube1->m_radius +c ube2->m_radius);
   if ( D3DXVec3LengthSq(&(cube2->m_c - cube1->m_c)) > (len*len)  )
   {
      return false;
   }

   BOOL hit = true;
   float p = 10000.0f;
   vec3 h;

   hit &= SpanIntersect(cube1, cube2, cube1->m_u[0], &p, &h);
   hit &= SpanIntersect(cube1, cube2, cube1->m_u[1], &p, &h);
   hit &= SpanIntersect(cube1, cube2, cube1->m_u[2], &p, &h);
                                           
   hit &= SpanIntersect(cube1, cube2, cube2->m_u[0], &p, &h);
   hit &= SpanIntersect(cube1, cube2, cube2->m_u[1], &p, &h);
   hit &= SpanIntersect(cube1, cube2, cube2->m_u[2], &p, &h);
                               
   hit &= SpanIntersect(cube1, cube2, cross(cube1->m_u[0], cube2->m_u[0]), &p, &h);
   hit &= SpanIntersect(cube1, cube2, cross(cube1->m_u[0], cube2->m_u[1]), &p, &h);
   hit &= SpanIntersect(cube1, cube2, cross(cube1->m_u[0], cube2->m_u[2]), &p, &h);
                                                                
   hit &= SpanIntersect(cube1, cube2, cross(cube1->m_u[1], cube2->m_u[0]), &p, &h);
   hit &= SpanIntersect(cube1, cube2, cross(cube1->m_u[1], cube2->m_u[1]), &p, &h);
   hit &= SpanIntersect(cube1, cube2, cross(cube1->m_u[1], cube2->m_u[2]), &p, &h);
                                                                
   hit &= SpanIntersect(cube1, cube2, cross(cube1->m_u[2], cube2->m_u[0]), &p, &h);
   hit &= SpanIntersect(cube1, cube2, cross(cube1->m_u[2], cube2->m_u[1]), &p, &h);
   hit &= SpanIntersect(cube1, cube2, cross(cube1->m_u[2], cube2->m_u[2]), &p, &h);

   numHitPoints = 0;

   //if (penetration && hitNormal && hit)
   //{
      CalculateHitPoint(  box0, 
         box1,
         p,
         h,
         hitPoints,
         numHitPoints);
      *penetration = p;
      *hitNormal = -h;
   //}
   return hit;
}

void CCollisionManager::AddCollision( CCube* cube1, CCube* cube2, vec3 &point, vec3 &normal, float penetration)
{
   stCollisionPoints* cp = NULL;
   for (int i=0; i<m_collisionsArray.size(); i++)
   {
      if (m_collisionsArray[i].box0==cube1 &&
         m_collisionsArray[i].box1==cube2)
      {
         cp = &m_collisionsArray[i];
         break;
      }
   }

   // We've not found one, hence add it to our list, with the data
   // and return
   if ( cp == NULL )
   {
      int numCols = m_collisionsArray.size();

      stCollisionPoints cpNew;
      cpNew.box0		   = cube1;
      cpNew.box1		   = cube2;
      cpNew.numPoints   = 1;

      stPoint pNew;
      pNew.normal	      = normal;
      pNew.point	      = point;
      pNew.pos0		   = cube1->m_c;
      pNew.pos1		   = cube2->m_c;
      pNew.pen		      = penetration;
      cpNew.points.push_back( pNew );

      m_collisionsArray.push_back( cpNew );

      return;
   }

   stPoint p;
   p.normal	      = normal;
   p.point	      = point;
   p.pos0		   = cube1->m_c;
   p.pos1		   = cube2->m_c;
   p.pen		      = penetration;
   cp->points.push_back( p );
}