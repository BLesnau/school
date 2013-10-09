#version 140
// Input vertex data, different for all executions of this shader.
uniform mat4 mPVM;
in vec4 vPosition;
in vec4 vColor;
out vec4 color;

void main(){
	color = vColor;
	gl_Position = mPVM * vPosition;
}

