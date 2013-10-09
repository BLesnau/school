#version 140
// Input vertex data, different for all executions of this shader.
uniform mat4 mPVM;
in vec4 vPosition;
in vec4 vColor;
out vec4 color;
uniform float t;

void main(){
	color = vColor;
	gl_Position = mPVM * vPosition;
	gl_Position.x += t;
}

/*
#version 140
// Input vertex data, different for all executions of this shader.
in vec4 vPosition;
in vec3 vNormal;
out vec4 color;

uniform mat4 mPVM;
uniform mat4 mVM;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

void main()
{
   // Transform vertex  position into eye coordinates
   vec3 pos = (mVM*vPosition).xyz;
        
   vec3 L = normalize(LightPosition.xyz - pos);
   vec3 E = normalize(-pos);
   vec3 H = normalize(L + E);

   // Transform vertex normal into eye coordinates
   vec3 N = normalize(mVM*vec4(vNormal, 0.0)).xyz;

	// Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;
    float Kd = max(dot(L, N), 0.0);
    vec4  diffuse = Kd*DiffuseProduct;
    float Ks = pow(max(dot(N, H), 0.0), Shininess);
    vec4  specular = Ks * SpecularProduct;
    if(dot(L, N) < 0.0) 
        specular = vec4(0.0, 0.0, 0.0, 1.0);

    color = ambient + diffuse + specular;
    color.a = 1.0;

	gl_Position = mPVM * vPosition;
}
*/