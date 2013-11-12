#version 140
// Input vertex data, different for all executions of this shader.
in vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

out vec3 fN;
out vec3 fE;
out vec3 fL;
out vec2 texCoord;

uniform vec4 LightPosition;
uniform mat4 mPVM;
uniform mat4 mVM;
uniform float t;

void main(){

   // Transform vertex  position into eye coordinates
   vec3 pos = (mVM*vPosition).xyz;

   // Transform vertex normal into eye coordinates
   fN = (mVM*vec4(vNormal, 0.0)).xyz;
        
   fE = -pos;
   fL = LightPosition.xyz - pos;
   
   texCoord = vTexCoord;
   gl_Position = vPosition;
   gl_Position.w = (1. + 0.1*sin(t));
   gl_Position = mPVM * gl_Position;
}

