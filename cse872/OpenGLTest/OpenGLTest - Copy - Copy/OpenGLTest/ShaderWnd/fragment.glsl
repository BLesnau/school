#version 140

out vec4 fColor;
in vec4 color;
void main()
{

	// Output color = blue 
	//gl_FragColor = vec4(0,0,1,1);
	fColor = color; 
	//vec4(0,0,1,1);

}