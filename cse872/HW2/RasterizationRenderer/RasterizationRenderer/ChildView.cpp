
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "RasterizationRenderer.h"
#include "ChildView.h"
#include <fstream>
#include <vector>
#include "TextureDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	m_nXres = 0;
	m_nYres = 0;

	m_Display = NULL;
	m_Render = NULL;
}

CChildView::~CChildView()
{
	if (m_Display)
		delete m_Display;
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_COMMAND(ID_TESTS_APP1, &CChildView::OnTestsApp1)
	ON_COMMAND(ID_TESTS_APP2, &CChildView::OnTestsApp2)
	ON_COMMAND(ID_TESTS_APP3, &CChildView::OnTestsApp3)
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (!m_Display)
		return;

	if (!m_Display->open)
		return;

	static HBITMAP hBitmap ;
	HDC            hdc, hdcMem ;
	hdc = dc.GetSafeHdc();

	LONG lBmpSize = m_Display->xres*m_Display->yres*3;
	BITMAPINFO bmpInfo={0};
	memset(&bmpInfo.bmiHeader,0,sizeof(BITMAPINFOHEADER));
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biHeight = m_Display->yres;
	bmpInfo.bmiHeader.biWidth = m_Display->xres;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biSizeImage = m_Display->xres*m_Display->yres*3;

	SetStretchBltMode(hdc, COLORONCOLOR);

	CWindowDC nullDC(NULL);
	UINT * pPixels = 0;
	hBitmap = CreateDIBSection(hdc, (BITMAPINFO*) &bmpInfo, DIB_RGB_COLORS,(void **) &pPixels, NULL, 0);
	

	memcpy(pPixels, m_Display->image, lBmpSize);


	hdcMem = CreateCompatibleDC (hdc) ;
	SelectObject (hdcMem, hBitmap) ;


	StretchBlt(hdc, 0, 0, m_nXres, m_nYres, hdcMem, 0, 0, m_Display->xres, m_Display->yres,SRCCOPY);

	DeleteDC (hdcMem); 
	DeleteObject (hBitmap) ;
}



void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	
	m_nXres = cx;
	m_nYres = cy;
}

void CChildView::Resize(int x, int y)
{
	HWND hwnd = AfxGetMainWnd()->GetSafeHwnd();
	RECT winRC, clientRC;
	::GetWindowRect(hwnd, &winRC);
	GetClientRect(&clientRC);
	int dx = x - (clientRC.right-clientRC.left);
	int dy = y - (clientRC.bottom-clientRC.top);

	::SetWindowPos(hwnd, 0, 0, 0, winRC.right-winRC.left + dx, winRC.bottom-winRC.top + dy, SWP_NOMOVE|SWP_NOZORDER);
}


void CChildView::OnTestsApp1()
{
	//Test for the display API
	int		i, j; 
	int		xRes, yRes;
	GzDisplayClass disp_class;	/* display parameters */ 
	int		status; 
 
	 
	status = 0; 

	if (m_Display)
		delete m_Display;
 
	/* 
	 * initialize the display and the renderer 
	 */ 
	m_Display = new GzDisplay(GZ_RGBAZ_DISPLAY, 512, 512); 
 
	status |= m_Display->GetParams(&xRes, &yRes, &disp_class); 

	Resize(xRes, yRes);

	status |= m_Display->Init(); /* init for new frame */
 
	if (status) return; 
 
	std::ifstream file("data/rects", std::ios::in);

	int	ulx, uly, lrx, lry, r, g, b;
	while( file >> ulx >> uly >> lrx >> lry >> r >> g >> b) {
		for (j = uly; j <= lry; j++) {
		  for (i = ulx; i <= lrx; i++) {
		    m_Display->Put(i, j, r, g, b, 1, 0);
		  }
		}
	} 
	
	file.close();

	m_Display->Flush();	/* write out or update display */

	Invalidate();
}

using namespace std;
GzMatrix	scale = 
{ 
	3.25f,	0.0f,	0.0f,	0.0f, 
	0.0f,	3.25,	0.0f,	-3.25f, 
	0.0f,	0.0f,	3.25f,	3.5f, 
	0.0f,	0.0f,	0.0f,	1.0f 
}; 

GzMatrix	rotateX = 
{ 
	1.0f,	0.0f,	0.0f,	0.0f, 
	0.0f,	.7071f,	.7071f,	0.0f, 
	0.0f,	-.7071f,	.7071f,	0.0f, 
	0.0f,	0.0f,	0.0f,	1.0f
}; 

GzMatrix	rotateY = 
{ 
	.866f,	0.0f,	-0.5f,	0.0f, 
	0.0f,	1.0f,	0.0f,	0.0f, 
	0.5f,	0.0f,	.866f,	0.0f, 
	0.0f,	0.0f,	0.0f,	1.0f 
}; 

/* to be filled in by the app if it sets camera params */
GzCamera	camera;  

void CChildView::OnTestsApp2()
{
	//Testing rasterization implementation
	GzToken		nameListTriangle[3]; 	/* vertex attribute names */
	GzPointer	valueListTriangle[3]; 		/* vertex attribute pointers */
	GzCoord		vertex;	/* vertex position coordinates */ 
	GzCoord		normal;	/* vertex normals */ 
	GzTextureIndex	uv;		/* vertex texture map indices */ 
	vector<GzCoord>		*vertexList;	/* VBO for vertex position coordinates */ 
	vector<GzCoord>		*normalList;	/* VBO for vertex normals */ 
	vector<GzTextureIndex>	*uvList;	/* VBO for vertex texture map indices */ 
	int		status; 
	int xRes, yRes;
	GzDisplayClass disp_class;	/* display parameters */ 

	if (m_Display)
		delete m_Display;
	m_Display = new GzDisplay(GZ_RGBAZ_DISPLAY, 512, 512); 
	
	status |= m_Display->GetParams(&xRes, &yRes, &disp_class); 

	Resize(xRes, yRes);

	status = 0; 
	/* 
	 * initialize the display and the renderer 
	 */ 

 	if (m_Render)
		delete m_Render;
	m_Render = new GzRender(GZ_Z_BUFFER_RENDER, m_Display); 

	/* set up app-defined camera if desired, else use camera defaults */
	camera.position[X] = 13.2f;      
	camera.position[Y] = -8.7f;
	camera.position[Z] = -14.8f;

	camera.lookat[X] = 0.8f;
	camera.lookat[Y] = 0.7f;
	camera.lookat[Z] = 4.5f;

	camera.worldup[X] = -0.2f;
	camera.worldup[Y] = 1.0f;
	camera.worldup[Z] = 0.0f;

	camera.FOV = 53.7f;              /* degrees */

	status |= m_Render->PutCamera(&camera); 

	status |= m_Render->Begin();

#if 1	/* set up model matrices if desired */
	status |= m_Render->PushMatrix(scale);  
	status |= m_Render->PushMatrix(rotateY); 
	status |= m_Render->PushMatrix(rotateX); 
#endif

	if (status) 
		return; 

	/* 
	* Walk through the list of triangles, set color 
	* and pass vert info to render/scan convert each triangle 
	*/ 
	static _TCHAR BASED_CODE szFilter[] = TEXT("Triangle Files (*.asc)|*.asc|All Files (*.*)|*.*||");

	CFileDialog dlg(FALSE, TEXT(".asc"), NULL, 0, szFilter, NULL);
	if(dlg.DoModal() != IDOK)
   {
      return;
   }

   // Open the file

   std::ifstream file(dlg.GetPathName(), std::ios::in);

   vertexList = new vector<GzCoord>;
   normalList = new vector<GzCoord>;
   uvList = new vector<GzTextureIndex>;

   char		dummy[256]; 
   while( file>> dummy) { 	/* construct VBO */
	   for (int i = 0; i < 3; i++) {
			file>> vertex[0] >> vertex [1] >> vertex [2]
				>> normal[0] >> normal[1] >> normal[2]
				>> uv[0] >> uv[1];
			vertexList->push_back(vertex);
			normalList->push_back(normal);
			uvList->push_back(uv);
	   }
   }

   GzToken         nameListAttributes[3];       /* type names */
   GzPointer       valueListAttributes[3];      /* shader pointers */
 
   extern void simple_vertex_shader(int numInput, GzToken *typeInput, GzPointer *inList, int &numOutput, GzToken *&typeOutput, GzPointer *&outList, GzRender *);
   extern void simple_fragment_shader(int numInput, GzToken *typeInput, GzPointer *inList, int &numOutput, GzToken *&typeOutput, GzPointer *&outList, GzRender*);

   valueListAttributes[0] = (GzPointer)simple_vertex_shader; 
   nameListAttributes[0] = (GzToken) GZ_VERTEX_SHADER; 
   valueListAttributes[1] = (GzPointer) simple_fragment_shader; 
   nameListAttributes[1] = (GzToken) GZ_FRAGMENT_SHADER; 
   m_Render->PutAttribute(2, nameListAttributes, valueListAttributes); 

	 /* 
	 * Tokens associated with triangle vertex values 
	 */ 
	nameListTriangle[0] = GZ_POSITION; /* define vert coordinates only */
	nameListTriangle[1] = GZ_NORMAL;	 
	/* 
	* Set the value pointers to the first vertex of the 	
	* triangle, then feed it to the renderer 
	*/ 
	valueListTriangle[0] = (GzPointer) vertexList; 
	valueListTriangle[1] = (GzPointer) normalList;
	m_Render->DrawTriangles(vertexList->size()/3,2, nameListTriangle, valueListTriangle);

	m_Display->Flush();
	
	delete vertexList;
	delete normalList;
	delete uvList;

	delete m_Render;
	m_Render = NULL;
	Invalidate();
}

GzLight	light1 = { {-0.7071f, 0.7071f, 0.f}, {0.5f, 0.5f, 0.9f} };
GzLight	light2 = { {0.f, -0.7071f, -0.7071f}, {0.9f, 0.2f, 0.3f} };
GzLight	light3 = { {0.7071f, 0.0f, -0.7071f}, {0.2f, 0.7f, 0.3f} };
GzLight	ambientlight = { {0.f, 0.f, 0.f}, {0.3f, 0.3f, 0.3f} };

GzColor specularCoefficient = { 0.3f, 0.3f, 0.3f };
GzColor ambientCoefficient = { 0.1f, 0.1f, 0.1f };
GzColor diffuseCoefficient = {0.7f, 0.7f, 0.7f};

int tex_fun(float u,float v, GzColor& color);	/* image texture function */
int ptex_fun_crossed(float u,float v, GzColor& color);	/* procedural texture function */
int ptex_fun_vert(float u,float v, GzColor& color);
int ptex_fun_horiz(float u,float v, GzColor& color);

void CChildView::OnTestsApp3()
{
	TextureDlg dlg;
	if (dlg.DoModal() != IDOK) 
		return;

	int nTextureFunction= dlg.m_nTexture;
	int nPhongShading = dlg.m_nShaders;

	GzToken		nameListTriangle[3]; 	/* vertex attribute names */
	GzPointer		valueListTriangle[3]; 	/* vertex attribute pointers */
	GzToken		nameListShader[9]; 	/* shader attribute names */
	GzPointer       valueListShader[9];		/* shader attribute pointers */
	GzToken         nameListLights[10];		/* light info */
	GzPointer       valueListLights[10];
	GzCoord		vertex;	/* vertex position coordinates */ 
	GzCoord		normal;	/* vertex normals */ 
	GzTextureIndex	uv;		/* vertex texture map indices */ 
	vector<GzCoord>		*vertexList;	/* VBO for vertex position coordinates */ 
	vector<GzCoord>		*normalList;	/* VBO for vertex normals */ 
	vector<GzTextureIndex>	*uvList;	/* VBO for vertex texture map indices */ 

	char			dummy[256]; 
	float			specpower;

	int		xRes, yRes;
	GzDisplayClass disp_class;	/* display parameters */ 
	int		status; 
 
	status = 0; 
	/* 
	 * initialize the display and the renderer 
	 */ 
	if (m_Display)
		delete m_Display;
	m_Display = new GzDisplay(GZ_RGBAZ_DISPLAY, 512, 512); 
 
	status |= m_Display->GetParams(&xRes, &yRes, &disp_class); 
	Resize(xRes, yRes);
	 
	status |= m_Display->Init (); 
 
	if (m_Render)
		delete m_Render;
	m_Render = new GzRender(GZ_Z_BUFFER_RENDER, m_Display); 

	if (1) {   /* set up app-defined camera if desired, else use rend.h defaults */

		camera.position[X] = 13.2f;      
		camera.position[Y] = -8.7f;
		camera.position[Z] = -14.8f;

		camera.lookat[X] = 0.8f;
		camera.lookat[Y] = 0.7f;
		camera.lookat[Z] = 4.5f;

		camera.worldup[X] = -0.2f;
		camera.worldup[Y] = 1.0f;
		camera.worldup[Z] = 0.0f;

		camera.FOV = 53.7f;              /* degrees */

		status |= m_Render->PutCamera(&camera);
	}

	m_Render->Begin(); 

/* 
  renderer is ready for frame --- define lights and shader at start of frame 
*/

    /*
        * Tokens associated with light parameters
        */
    nameListLights[0] = GZ_DIRECTIONAL_LIGHT;
    valueListLights[0] = (GzPointer)&light1;
    nameListLights[1] = GZ_DIRECTIONAL_LIGHT;
    valueListLights[1] = (GzPointer)&light2;
    nameListLights[2] = GZ_DIRECTIONAL_LIGHT;
    valueListLights[2] = (GzPointer)&light3;
	status |= m_Render->PutAttribute(3, nameListLights, valueListLights);

    nameListLights[0] = GZ_AMBIENT_LIGHT;
    valueListLights[0] = (GzPointer)&ambientlight;
	status |= m_Render->PutAttribute(1, nameListLights, valueListLights);

    /*
    * Uniform variables associated with shading 
    */
	nameListShader[0] = GZ_DIFFUSE_COEFFICIENT;
	valueListShader[0] = &diffuseCoefficient;
    nameListShader[1]  = GZ_AMBIENT_COEFFICIENT;
    valueListShader[1] = (GzPointer) &ambientCoefficient;
    nameListShader[2]  = GZ_SPECULAR_COEFFICIENT;
    valueListShader[2] = (GzPointer) &specularCoefficient;
    nameListShader[3]  = GZ_DISTRIBUTION_COEFFICIENT;
    specpower = 32;
    valueListShader[3] = (GzPointer)&specpower;

	nameListShader[4] = GZ_TEXTURE_MAP;
	if (nTextureFunction == 0)
		valueListShader[4] = NULL;
	else if (nTextureFunction == 1)
		valueListShader[4] = (GzPointer) tex_fun;
	else if (nTextureFunction == 2)
		valueListShader[4] = (GzPointer) (ptex_fun_crossed);
   else if (nTextureFunction == 3)
      valueListShader[4] = (GzPointer) (ptex_fun_vert);
   else
      valueListShader[4] = (GzPointer) (ptex_fun_horiz);

	status |= m_Render->PutAttribute(5, nameListShader, valueListShader);
	status |= m_Render->PushMatrix(scale);  
	status |= m_Render->PushMatrix(rotateY); 
	status |= m_Render->PushMatrix(rotateX); 

	if (status) return; 

	/* 
	* Tokens associated with triangle vertex values 
	*/ 
	nameListTriangle[0] = GZ_POSITION; 
	nameListTriangle[1] = GZ_NORMAL; 
	nameListTriangle[2] = GZ_TEXTURE_INDEX;
	 
	/* 
	* Walk through the list of triangles, set color 
	* and pass vert info to render/scan convert each triangle 
	*/ 
	static _TCHAR BASED_CODE szFilter[] = TEXT("Triangle Files (*.asc)|*.asc|All Files (*.*)|*.*||");

	CFileDialog fdlg(FALSE, TEXT(".asc"), NULL, 0, szFilter, NULL);
	if(fdlg.DoModal() != IDOK)
   {
      return;
   }

   //UpdateWindow();

   // Open the file

   std::ifstream file(fdlg.GetPathName(), std::ios::in);

   vertexList = new vector<GzCoord>;
   normalList = new vector<GzCoord>;
   uvList = new vector<GzTextureIndex>;

   while( file>> dummy) { 	/* construct VBO */
	   for (int i = 0; i < 3; i++) {
			file>> vertex[0] >> vertex [1] >> vertex [2]
				>> normal[0] >> normal[1] >> normal[2]
				>> uv[0] >> uv[1];
			vertexList->push_back(vertex);
			normalList->push_back(normal);
			uvList->push_back(uv);
	   }
   }

   GzToken         nameListAttributes[3];       /* type names */
   GzPointer       valueListAttributes[3];      /* shader pointers */
 
   extern void simple_vertex_shader(int numInput, GzToken *typeInput, GzPointer *inList, int &numOutput, GzToken *&typeOutput, GzPointer *&outList, GzRender *);
   extern void simple_fragment_shader(int numInput, GzToken *typeInput, GzPointer *inList, int &numOutput, GzToken *&typeOutput, GzPointer *&outList, GzRender*);
   extern void gouraud_vertex_shader(int numInput, GzToken *typeInput, GzPointer *inList, int &numOutput, GzToken *&typeOutput, GzPointer *&outList, GzRender *);
   extern void phong_fragment_shader(int numInput, GzToken *typeInput, GzPointer *inList, int &numOutput, GzToken *&typeOutput, GzPointer *&outList, GzRender*);
   extern void phong_vertex_shader(int numInput, GzToken *typeInput, GzPointer *inList, int &numOutput, GzToken *&typeOutput, GzPointer *&outList, GzRender *);
  
   nameListAttributes[0] = (GzToken) GZ_VERTEX_SHADER; 
   nameListAttributes[1] = (GzToken) GZ_FRAGMENT_SHADER; 
   if (nPhongShading == 0) {
	   valueListAttributes[0] = (GzPointer) gouraud_vertex_shader; 
	   valueListAttributes[1] = (GzPointer) simple_fragment_shader; 
   } else {
	   valueListAttributes[0] = (GzPointer) phong_vertex_shader; 
	   valueListAttributes[1] = (GzPointer) phong_fragment_shader; 
   }
   m_Render->PutAttribute(2, nameListAttributes, valueListAttributes); 

	/* 
	* Set the VBOs	*/ 
	valueListTriangle[0] = (GzPointer) vertexList; 
	valueListTriangle[1] = (GzPointer) normalList;
	valueListTriangle[2] = (GzPointer) uvList;
	m_Render->DrawTriangles(vertexList->size()/3, 3, nameListTriangle, valueListTriangle);

	m_Display->Flush();
	
	delete vertexList;
	delete normalList;
	delete uvList;

	delete m_Render;
	m_Render = NULL;
	Invalidate();

}
