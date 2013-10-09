
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "ShaderTest7.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	m_bTimer = false;
	m_nTimer = 0;
	m_fT = 0.f;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CShaderWnd)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_OPERATION_TIMER, &CChildView::OnOperationTimer)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CShaderWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

const int NumVertices = 36;
typedef  vec4  point4;typedef  vec4  color4;
point4  points[NumVertices];color4  colors[NumVertices];
vec3 normals[NumVertices];

const int  TextureSize  = 64;

// Texture objects and storage for texture image
GLuint textures[2];
GLubyte image[TextureSize][TextureSize][3];
GLubyte image2[TextureSize][TextureSize][3];
vec2   tex_coords[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertex_positions[8] = {
	point4( -0.5, -0.5,  0.5, 1.0 ),
	point4( -0.5,  0.5,  0.5, 1.0 ),
	point4(  0.5,  0.5,  0.5, 1.0 ),
	point4(  0.5, -0.5,  0.5, 1.0 ),
	point4( -0.5, -0.5, -0.5, 1.0 ),
	point4( -0.5,  0.5, -0.5, 1.0 ),
	point4(  0.5,  0.5, -0.5, 1.0 ),
	point4(  0.5, -0.5, -0.5, 1.0 )
};

// RGBA colors
color4 vertex_colors[8] = {
	color4( 0.0, 0.0, 0.0, 1.0 ),  // black
	color4( 1.0, 0.0, 0.0, 1.0 ),  // red
	color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
	color4( 0.0, 1.0, 0.0, 1.0 ),  // green
	color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
	color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
	color4( 1.0, 1.0, 1.0, 1.0 ),  // white
	color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};

// quad() generates two triangles for each face and assigns colors to the vertices
int Index = 0;  // global variable indexing into VBO arrays
void quad(int a, int b, int c, int d) {
	vec3 u = vec3(vertex_positions[b]-vertex_positions[a]);
	vec3 v = vec3(vertex_positions[c]-vertex_positions[b]);
	
	vec3 normal = normalize(cross(u,v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertex_positions[a]; 
	tex_coords[Index] = vec2(0.f, 0.f);
	Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[b]; points[Index] = vertex_positions[b]; 
	tex_coords[Index] = vec2(0.f, 1.f);
	Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[c]; points[Index] = vertex_positions[c]; 
	tex_coords[Index] = vec2(1.f, 1.f);
	Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertex_positions[a]; 
	tex_coords[Index] = vec2(0.f, 0.f);
	Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[c]; points[Index] = vertex_positions[c]; 
	tex_coords[Index] = vec2(1.f, 1.f);
	Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[d]; points[Index] = vertex_positions[d]; 
	tex_coords[Index] = vec2(1.f, 0.f);
	Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void
	colorcube() {

		quad( 1, 0, 3, 2 );
		quad( 2, 3, 7, 6 );
		quad( 3, 0, 4, 7 );
		quad( 6, 5, 1, 2 );
		quad( 4, 5, 6, 7 );
		quad( 5, 4, 0, 1 );
}


void CChildView::InitGL()
{
	// Create a checkerboard pattern
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			GLubyte c;
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			image[i][j][0]  = c;
			image[i][j][1]  = c;
			image[i][j][2]  = c;
			image2[i][j][0] = c;
			image2[i][j][1] = 0;
			image2[i][j][2] = c;
		}
	}

	colorcube();

	m_program = LoadShaders( "ShaderWnd/vertex.glsl", "ShaderWnd/fragment.glsl" );


	static const vec4 g_vertex_buffer_data[] = { 
		vec4(-1.0f, -1.0f, 0.0f, 1.0f),
		vec4(1.0f, -1.0f, 0.0f, 1.0f),
		vec4(0.0f,  0.5f, 0.0f, 1.0f)
	};

	static const vec4 g_color_buffer_data[] = { 
		vec4(1.0f, 0.0f, 0.0f, 1.0f),
		vec4(0.0f, 1.0f, 0.0f, 1.0f),
		vec4(0.0f, 0.0f, 1.0f, 1.0f)
	};

	//GLuint vertexbuffer;
	glUseProgram(m_program);

	GLuint vao;
	glGenVertexArrays( 1, &vao);
	glBindVertexArray( vao);

	/*glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);*/


	glGenTextures(2, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize,TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureSize,TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, textures[0]);
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, textures[1]);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) +  sizeof(tex_coords) +        sizeof(normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,                sizeof(points), value_ptr(points[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), value_ptr(normals[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals), sizeof(tex_coords), value_ptr(tex_coords[0]));


	point4 light_position (-5.f, 5.f, -5.f, 0.f);
	color4 light_ambient (0.2f, 0.2f, 0.2f, 1.f);
	color4 light_diffuse (1.f, 1.f, 1.f, 1.f);
	color4 light_specular (1.f, 1.f, 1.f, 1.f);

	color4 material_ambient(.3f, .6f, .3f, 1.f);
	color4 material_diffuse (0.3f, .6f, 0.3f, 1.f);
	color4 material_specular (1.f, 1.f, 1.f, 1.f);
	float material_shininess = 100.0f;

	color4 ambient_product = light_ambient*material_ambient;
	color4 diffuse_product = light_diffuse*material_diffuse;
	color4 specular_product = light_specular*material_specular;

	glUniform4fv(glGetUniformLocation(m_program, "AmbientProduct"), 1, value_ptr(ambient_product));
	glUniform4fv(glGetUniformLocation(m_program, "DiffuseProduct"), 1, value_ptr(diffuse_product));
	glUniform4fv(glGetUniformLocation(m_program, "SpecularProduct"), 1, value_ptr(specular_product));
	glUniform4fv(glGetUniformLocation(m_program, "LightPosition"), 1, value_ptr(light_position));
	glUniform1f(glGetUniformLocation(m_program, "Shininess"), material_shininess);

	// set up vertex arrays (after shaders are loaded)
	GLuint vPosition = glGetAttribLocation(m_program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	GLuint vNormal = glGetAttribLocation(m_program, "vNormal");
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	GLuint vTex = glGetAttribLocation(m_program, "vTexCoord");
		glEnableVertexAttribArray(vTex);
		glVertexAttribPointer(vTex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)+sizeof(normals)));

	// Set the value of the fragment shader texture sampler variable
	//   ("texture") to the the appropriate texture unit. In this case,
	//   zero, for GL_TEXTURE0 which was previously set by calling
	//   glActiveTexture().
	glUniform1i( glGetUniformLocation(m_program, "diffuse_mat"), 0);


	m_nPVM = glGetUniformLocation(m_program, "mPVM");
	m_nVM = glGetUniformLocation(m_program, "mVM");

	glClearColor(1.f,1.f,1.f,1.f);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
}

void CChildView::RenderGL()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// Clear the screen

	glUniformMatrix4fv(m_nPVM, 1, GL_FALSE, value_ptr(m_mPVM));
	glUniformMatrix4fv(m_nVM, 1, GL_FALSE, value_ptr(m_mVM));

	glUniform1f(glGetUniformLocation(m_program,"t"), m_fT);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0,  NumVertices); 
}

void CChildView::CleanGL()
{

}

void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
	case 'r':
	case 'R':
			ResetMatrix();
			break;
	case '1':
		glUniform1i( glGetUniformLocation(m_program, "diffuse_mat"), 0);
		InvalidateRect(NULL,FALSE);
		break;
	case '2':
		glUniform1i( glGetUniformLocation(m_program, "diffuse_mat"), 1);
		InvalidateRect(NULL,FALSE);
		break;
	}

	CShaderWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CChildView::OnOperationTimer()
{
	if (m_bTimer)
	{
		m_bTimer = false;
		if (m_nTimer)
			KillTimer(m_nTimer);
	} else
	{
		m_bTimer = true;
		m_fT = 0.f;
		m_nTimer = SetTimer(1, 40, NULL);
	}
}


void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	m_fT += 0.04f;
	InvalidateRect(NULL,FALSE);

	CShaderWnd::OnTimer(nIDEvent);
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bTimer)
	{
		//suspend timer
		if (m_nTimer)
		KillTimer(m_nTimer);
		m_nTimer = 0;
	}
	CShaderWnd::OnLButtonDown(nFlags, point);
}


void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bTimer)
	{
		m_nTimer=SetTimer(1, 40, NULL);
	}
	CShaderWnd::OnLButtonUp(nFlags, point);
}
