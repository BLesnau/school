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
      for( int j=i; j<m_cubes.size(); j++ )
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

void CCollisionManager::ApplyImpulses( float dt )
{
   for (int i=0; i<m_collisionsArray.size(); i++)
   {
      stCollisionPoints* cp = &m_collisionsArray[ i ];

      CCube* cube1				= cp->box0;
      CCube* cube2				= cp->box1;
      int numPoints			= cp->numPoints;

      for (int k=0; k<numPoints; k++)
      {

         vec3 hitPoint = cp->points[k].point;
         vec3 normal	 = cp->points[k].normal;
         float penDepth		 = cp->points[k].pen;

         CCube::AddCollisionImpulse(	cube1, 
            cube2, 
            hitPoint, 
            normal, 
            dt,
            penDepth);
      }
   }
}

BOOL CCollisionManager::CubeCubeCollisionCheck( CCube* cube1, CCube* cube2, vector<vec3> &hitPoints, int &numHitPoints, float &penetration, vec3 &hitNormal )
{
   // Simple bounding sphere check first
   float len = (cube1->m_radius + cube2->m_radius);
   auto squaredLength = length(cube2->m_c - cube1->m_c);
   squaredLength *= squaredLength;
   if ( squaredLength > (len*len)  )
   {
      return false;
   }

   BOOL hit = true;
   float p = 10000.0f;
   vec3 h;

   vec3 m_u3_1[3];
   m_u3_1[0] = vec3( cube1->m_u[0] );
   m_u3_1[1] = vec3( cube1->m_u[1] );
   m_u3_1[2] = vec3( cube1->m_u[2] );
   vec3 m_u3_2[3];
   m_u3_2[0] = vec3( cube2->m_u[0] );
   m_u3_2[1] = vec3( cube2->m_u[1] );
   m_u3_2[2] = vec3( cube2->m_u[2] );

   hit &= SpanIntersect(cube1, cube2, m_u3_1[0], &p, &h);
   hit &= SpanIntersect(cube1, cube2, m_u3_1[1], &p, &h);
   hit &= SpanIntersect(cube1, cube2, m_u3_1[2], &p, &h);
   
   hit &= SpanIntersect(cube1, cube2, m_u3_2[0], &p, &h);
   hit &= SpanIntersect(cube1, cube2, m_u3_2[1], &p, &h);
   hit &= SpanIntersect(cube1, cube2, m_u3_2[2], &p, &h);
                               
   hit &= SpanIntersect(cube1, cube2, cross(m_u3_1[0], m_u3_2[0]), &p, &h);
   hit &= SpanIntersect(cube1, cube2, cross(m_u3_1[0], m_u3_2[1]), &p, &h);
   hit &= SpanIntersect(cube1, cube2, cross(m_u3_1[0], m_u3_2[2]), &p, &h);
                                                                       
   hit &= SpanIntersect(cube1, cube2, cross(m_u3_1[1], m_u3_2[0]), &p, &h);
   hit &= SpanIntersect(cube1, cube2, cross(m_u3_1[1], m_u3_2[1]), &p, &h);
   hit &= SpanIntersect(cube1, cube2, cross(m_u3_1[1], m_u3_2[2]), &p, &h);
                                                                       
   hit &= SpanIntersect(cube1, cube2, cross(m_u3_1[2], m_u3_2[0]), &p, &h);
   hit &= SpanIntersect(cube1, cube2, cross(m_u3_1[2], m_u3_2[1]), &p, &h);
   hit &= SpanIntersect(cube1, cube2, cross(m_u3_1[2], m_u3_2[2]), &p, &h);

   numHitPoints = 0;

   if ( hit )
   {
      CalculateHitPoint(  cube1, 
         cube2,
         p,
         h,
         hitPoints,
         numHitPoints);
      penetration = p;
      hitNormal = -h;
   }
   return hit;
}

BOOL CCollisionManager::SpanIntersect( CCube* cube1, 
                                      CCube* cube2,
                                      vec3 &axisc,
                                      float* minPenetration /*= NULL*/,
                                      vec3* axisPenetration /*= NULL*/,
                                      float* pen /*= NULL*/ )
{
   vec3 axis = axisc;

   auto squaredLength = length(axis);
   squaredLength *= squaredLength;
	float lq = squaredLength;
	if (lq <= 0.02f)
	{
		if (pen) *pen = 100000.0f;
		return true;
	}

	axis = normalize( axis );

	float mina, maxa;
	float minb, maxb;
	CalculateInterval( cube1, axis, mina, maxa );
	CalculateInterval( cube2, axis, minb, maxb );

	float lena = maxa - mina;
	float lenb = maxb - minb;

	float minv = std::min( mina, minb );
	float maxv = std::max( maxa, maxb );
	float lenv = maxv - minv;

	if ( lenv > (lena + lenb) )
	{
		// Collision
		return false;
	}

	float penetration = (lena + lenb) - lenv;

	if (pen)
	{
		*pen = penetration;
	}

	if (minPenetration && axisPenetration)
	{
		if (penetration < *minPenetration)
		{
			*minPenetration  = penetration;
			*axisPenetration = axis;

			// BoxA pushes BoxB away in the correct Direction
			if (minb < mina)
			{
				*axisPenetration *= -1;
			}
		}
	}

	// Collision
	return TRUE;
}

void CCollisionManager::CalculateInterval( CCube* cube, const vec3 &axis, float &min, float &max )
{
   float x = cube->m_e.x;
   float y = cube->m_e.y;
   float z = cube->m_e.z;
   vec3 Vertex[8] = 
   {
      vec3(   x,    y,   z	),
      vec3(  -x,    y,	  z	),
      vec3(   x,   -y,	  z	),
      vec3(  -x,   -y,	  z	),
      
      vec3(   x,    y,	 -z	),
      vec3(  -x,    y,	 -z	),
      vec3(   x,   -y,	 -z	),
      vec3(  -x,   -y,	 -z	)
   };

   for (int i=0; i<8; i++)
   {
      Vertex[i] = vec3(vec4(Vertex[i].x, Vertex[i].y, Vertex[i].z, 1) * cube->m_matWorld);
   }

   min = max = dot(Vertex[0], axis);
   for (int i=0; i<8; i++)
   {
      float d = dot(Vertex[i], axis);

      if (d<min) min = d;
      if (d>max) max = d;
   }
}

void CCollisionManager::CalculateHitPoint(  CCube* cube1, 
                                          CCube* cube2,
                                          const float penetration,
                                          vec3 &hitNormal,
                                          vector<vec3> &hitPoints,
                                          int &numHitPoints )
{
   vec3 verts0[8];
	int vertIndex0[8];
	vec3 norm0 = hitNormal;
	int numVerts0 = GetNumHitPoints( cube1,
									 norm0,
									 penetration,
									 verts0,
									 vertIndex0);

	vec3 verts1[8];
	int vertIndex1[8];
	vec3 norm1 = -hitNormal;
	int numVerts1 = GetNumHitPoints( cube2,
									 norm1,
									 penetration,
									 verts1,
									 vertIndex1);

	// This should never really happen!
	if (numVerts0==0 || numVerts1==0)
	{
		return;
	}


	int numVertsX		= numVerts0;
	vec3* vertsX  = verts0;

	if (numVerts0>=4 && numVerts1>=4)
	{
		static vec3 clipVerts[50];
		
		ClipFaceFaceVerts(	verts0, vertIndex0, 
							verts1, vertIndex1,
							clipVerts, &numVertsX);

		vertsX = clipVerts;
	}
	

	{
		if (numVerts1 < numVerts0)
		{
			numVertsX = numVerts1;
			vertsX    = verts1;
			hitNormal = -norm1;
		}

		if (numVerts1==2 && numVerts0==2)
		{
			static vec3 V[2];
			static int numV = 0;

			ClosestPointLineLine(verts0,
								 verts1,
								 V,		numV);

			vertsX = V;
			numVertsX = numV;
		}

		if (numVerts0==2 && numVerts1==4)
		{
			ClipLinePlane(	verts0,		vertIndex0,  cube1,
							verts1,		vertIndex1,  cube2,
							vertsX,		numVertsX);
		}

		if (numVerts0==4 && numVerts1==2)
		{
			ClipLinePlane(	verts1,		vertIndex1,   cube1,
							verts0,		vertIndex0,   cube2,
							vertsX,		numVertsX);
		}

		numHitPoints = numVertsX;
		for (int i=0; i<numVertsX; i++)
		{
         hitPoints.push_back( vertsX[i] );
		}
	}
}

int CCollisionManager::GetNumHitPoints( CCube* cube,
                                       vec3 &hitNormal,
                                       float penetration,
                                       vec3 verts[8],
                                       int* vertIndexs)
{
   float x = cube->m_e.x;
   float y = cube->m_e.y;
   float z = cube->m_e.z;
   vec3 Vertex[8] = 
   {
      vec3(  -x,   -y,   -z	),
      vec3(   x,   -y,	  -z	),
      vec3(   x,   -y,	   z	),
      vec3(  -x,   -y,	   z	),
      
      vec3(  -x,    y,	  -z	),
      vec3(   x,    y,	  -z	),
      vec3(   x,    y,	   z	),
      vec3(  -x,    y,	   z	)
   };

   for (int i=0; i<8; i++)
   {
      Vertex[i] = vec3(vec4(Vertex[i].x, Vertex[i].y, Vertex[i].z, 1) * cube->m_matWorld);
   }

   vec3 planePoint = Vertex[0];
   float maxdist = dot(Vertex[0], hitNormal);

   for (int i=0; i<8; i++)
   {
      float d = dot(Vertex[i], hitNormal);
      if (d > maxdist)
      {
         maxdist = d;
         planePoint = Vertex[i];
      }
   }

   // Plane Equation (A dot N) - d = 0;

   float d = dot(planePoint, hitNormal);
   d -= penetration + 0.01f;


   int numVerts = 0;
   for (int i=0; i<8; i++)
   {
      float side = dot(Vertex[i], hitNormal) - d;

      if ( side > 0 )
      {
         verts[numVerts] = Vertex[i];
         vertIndexs[numVerts] = i;
         numVerts++;
      }
   }

   return numVerts;
}

void CCollisionManager::ClipFaceFaceVerts(vec3* verts0,
                                          int* vertIndexs0,
                                          vec3* verts1,
                                          int* vertIndexs1,
                                          vec3* vertsX,
                                          int* numVertsX)
{
   SortVertices(verts0, vertIndexs0);
   SortVertices(verts1, vertIndexs1);

   // Work out the normal for the face
   vec3 v0 = verts0[1] - verts0[0];
   vec3 v1 = verts0[2] - verts0[0];
   vec3 n  = cross(v1, v0);
   n = normalize(n);

   // Project all the vertices onto a shared plane, plane0
   vec3 vertsTemp1[4];
   for (int i=0; i<4; i++)
   {
      vertsTemp1[i] = verts1[i] + (n * dot(n, verts0[0]-verts1[i]));
   }

   static vec3 temp[50];
   int numVerts = 0;

   for (int c=0; c<2; c++)
   {
      vec3* vertA = vertsTemp1;
      vec3* vertB = verts0;
      if (c==1)
      {
         vertA = verts0;
         vertB = vertsTemp1;
      }

      // Work out the normal for the face
      vec3 v0 = vertA[1] - vertA[0];
      vec3 v1 = vertA[2] - vertA[0];
      vec3 n  = cross(v1, v0);
      n = normalize(n);

      for (int i=0; i<4; i++)
      {
         vec3 s0 = vertA[i];
         vec3 s1 = vertA[(i+1)%4];
         vec3 sx = s0 + n*10.0f;

         // Work out the normal for the face
         vec3 sv0 = s1 - s0;
         vec3 sv1 = sx - s0;
         vec3 sn  = cross(sv1, sv0);
         sn = normalize(sn);

         float d = dot(s0, sn);


         for (int j=0; j<4; j++)
         {
            vec3 p0 = vertB[j];
            vec3 p1 = vertB[(j+1)%4]; // Loops back to the 0th for the last one

            float d0 = dot(p0, sn) - d;
            float d1 = dot(p1, sn) - d;

            // Check there on opposite sides of the plane
            if ( (d0 * d1) < 0.0f)
            {
               vec3 pX = p0   +   ( dot(sn, (s0-p0)) /  dot(sn, (p1-p0)) )  *  (p1 - p0);

               if (VertInsideFace(vertA, pX, 0.1f))
               {
                  temp[numVerts] = pX;
                  numVerts++;
               }
            }

            if (VertInsideFace(vertA, p0))
            {
               temp[numVerts] = p0;
               numVerts++;
            }
         }
      }
   }

   // Remove verts which are very close to each other
   for (int i=0; i<numVerts; i++)
   {
      for (int j=i; j<numVerts; j++)
      {
         if (i!=j)
         {
            auto squaredLength = length(temp[i] - temp[j]);
            squaredLength *= squaredLength;
            float dist = squaredLength;

            if (dist < 6.5f)
            {
               for (int k=j; k<numVerts; k++)
               {
                  temp[k] = temp[k+1];
               }
               numVerts--;
            }
         }
      }
   }

   *numVertsX = numVerts;
   for (int i=0; i<numVerts; i++)
   {
      vertsX[i] = temp[i];
   }
}

void CCollisionManager::ClosestPointLineLine( const vec3*	verts0,	// 2 verts
                                               const vec3*	verts1, // 2 verts
                                               vec3* vertsX,
                                               int &numVertX)
{
   vec3 p1 = verts0[0];
   vec3 q1 = verts0[1];
   vec3 p2 = verts1[0];
   vec3 q2 = verts1[1];
   
   vec3 d1 = q1 - p1;
   vec3 d2 = q2 - p2;
   vec3 r  = p1 - p2;
   float a = dot(d1, d1);
   float e = dot(d2, d2);
   float f = dot(d2, r);

   const float epsilon = 0.00001f;

   float s, t;
   vec3 c1, c2;

   if (a <= epsilon && e <= epsilon)
   {
      s = t = 0.0f;
      c1 = p1;
      c2 = p2;

      vertsX[0] = c1;
      numVertX = 1;
      return;
   }

   if (a <= epsilon)
   {
      s = 0.0f;
      t = f / e;
      t = Clamp(t, 0.0f, 1.0f);
   }
   else
   {
      float c = dot(d1, r);
      if (e <= epsilon)
      {
         t = 0.0f;
         s = Clamp(-c/a, 0.0f, 1.0f);
      }
      else
      {
         float b = dot(d1, d2);
         float denom = a*e - b*b;

         if (denom != 0.0f)
         {
            s = Clamp((b*f - c*e) / denom, 0.0f, 1.0f);
         }
         else
         {
            s = 0.0f;
         }

         t = (b*s + f) / e;

         if (t < 0.0f)
         {
            t = 0.0f;
            s = Clamp(-c / a , 0.0f, 1.0f);
         }
         else if (t > 1.0f)
         {
            t = 1.0f;
            s = Clamp((b-c) / a, 0.0f, 1.0f);
         }
      }
   }

   c1 = p1 + d1 * s;
   c2 = p2 + d2 * t;

   vertsX[0] = (c1 + c2) * 0.5f;
   numVertX = 1;
}

void CCollisionManager::SortVertices(vec3* verts0, int* vertIndexs0)
{
   int faces[6][4] =
   {
      {4,0,3,7},
      {1,5,6,2},
      {0,1,2,3},
      {7,6,5,4},
      {5,1,0,4},
      {6,7,3,2}
   };

   int faceSet = -1;
   vec3 temp[4]; // New correct clockwise order

   // Work out which face to use
   for (int i=0; i<6 && faceSet==-1; i++)
   {
      int numFound = 0;
      for (int j=0; j<4; j++)
      {
         if (vertIndexs0[j]==faces[i][j])
         {
            temp[numFound] = verts0[j];
            numFound++;

            if (numFound==4)
            {
               faceSet = i;
               break;
            }
         }
      }
   }

   if (faceSet < 0)
   {
      int errorHasOccured = 1;
   }
   else
   {
      for (int i=0; i<4; i++)
      {
         verts0[i] = temp[i];
      }
   }
}

void CCollisionManager::ClipLinePlane(	const vec3*	verts0,	// 2 verts
                   const int*			vertIndexs0,
                   CCube*			cube1,
                   const vec3*	verts1, // 4 verts
                   int*				vertIndexs1,
                   CCube*			cube2,
                   vec3*		vertsX,
                   int				&numVertX)
{
   ClosestPtPointOBB(verts0[0], cube2, &vertsX[0]);
   ClosestPtPointOBB(verts0[1], cube2, &vertsX[1]);
   numVertX = 2;
}

void CCollisionManager::ClosestPtPointOBB(const vec3& point, 
                       CCube* cube,
                       vec3* closestP)
{
   vec3 d = point - vec3(cube->m_c);

   vec3 q = cube->m_c;

   for (int i=0; i<3; i++)
   {
      float dist = dot(d, vec3(cube->m_u[i]));

      if (dist >  cube->m_e[i]) dist =  cube->m_e[i];
      if (dist < -cube->m_e[i]) dist = -cube->m_e[i];

      q = q + (dist * vec3(cube->m_u[i]));
   }

   *closestP = q;
}

BOOL CCollisionManager::VertInsideFace(vec3* verts0, vec3 &p0, float planeErr /*= 0.0f*/)
{
   // Work out the normal for the face
   vec3 v0 = verts0[1] - verts0[0];
   vec3 v1 = verts0[2] - verts0[0];
   vec3 n  = cross(v1, v0);
   n = normalize(n);

   for (int i=0; i<4; i++)
   {
      vec3 s0 = verts0[i];
      vec3 s1 = verts0[(i+1)%4];
      vec3 sx = s0 + n*10.0f;

      // Work out the normal for the face
      vec3 sv0 = s1 - s0;
      vec3 sv1 = sx - s0;
      vec3 sn  = cross(sv1, sv0);
      sn = normalize(sn);

      float d  = dot(s0, sn);
      float d0 = dot(p0, sn) - d;

      // Outside the plane
      if (d0 > planeErr)
      {
         return FALSE;
      }
   }

   return TRUE;
}

float CCollisionManager::Clamp(float v, float min, float max)
{
   float res = v;
   res = v>max ? max : v;
   res = v<min ? min : v;
   return res;
};

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
   cp->numPoints++;
}