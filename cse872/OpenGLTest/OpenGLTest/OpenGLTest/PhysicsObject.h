#pragma once

//#include "ShaderWnd/external/glm/gtc/quaternion.hpp"
#include "Helpers.h"

class CPhysicsObject
{
public:
   struct State
   {
      // primary
      point3 position;
      vec3 momentum;

      // secondary
      vec3 velocity;

      // constant
      float mass;
      float inverseMass;

      void recalculate()
      {
         velocity = momentum * inverseMass;
      }
   };

   struct Derivative
   {
      vec3 velocity;
      vec3 force;
   };

   CPhysicsObject();
   ~CPhysicsObject();
};