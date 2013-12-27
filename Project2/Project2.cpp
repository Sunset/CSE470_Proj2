#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "glut.h"

#include <fstream>
using namespace std;

#include "loaders.h"
#include "mesh.h"

#define MIN2(a,b) (((a) < (b))?(a):(b))
#define MAX2(a,b) (((a) > (b))?(a):(b))

// window parameters
GLint winId;
int position_x		= 0;
int position_y		= 0;
int width			= 800;
int height			= 800;

// controling parameters
int mouse_button;
int mouse_x		= 0;
int mouse_y		= 0;
float scale		= 1.0;
float x_angle	= 0.0;
float y_angle	= 0.0;

// perspective projection parameters
GLfloat fieldofview = 45.0;
GLfloat aspect      = 1.0;
GLfloat nearPlane   = 0.1;
GLfloat farPlane    = 100.0;

// lighting parameters
GLfloat AmbientLight[]		= { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat DiffuseLight[]		= { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat SpecularLight[]		= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPos[]			= {-50.0f,50.0f,100.0f,1.0f};

GLfloat glowColor[]			= {0.0,0.0,1.0};

//chrome parameters
GLfloat AmbientMaterial[]	= {0.25f,0.25f,0.25f,1.0f};
GLfloat DiffuseMaterial[]	= {0.4f,0.4f,0.4f,1.0f};
GLfloat SpecularMaterial[]	= { 0.774597f,0.774597f,0.774597f,1.0f};
GLfloat ShininessMaterial[]	= {76.8};
//silver parameters
GLfloat SilverAmbientMaterial[]	= {0.19225f, 0.19225f, 0.19225f,  1.0f};
GLfloat SilverDiffuseMaterial[]	= {0.50754f, 0.50754f, 0.50754f,  1.0f};
GLfloat SilverSpecularMaterial[]	= { 0.508273f, 0.508273f, 0.508273f, 1.0f};
GLfloat SilverShininessMaterial[]	= {51.2};
//turquoise parameters
GLfloat TurquoiseAmbientMaterial[]	= {0.1, 0.18725, 0.1745, 0.8};
GLfloat TurquoiseDiffuseMaterial[]	= {0.396, 0.74151, 0.69102, 0.8};
GLfloat TurquoiseSpecularMaterial[]	= { 0.297254, 0.30829, 0.306678, 0.8};
GLfloat TurquoiseShininessMaterial[]	= {12.8};
//black Rubber parameters
GLfloat BRAmbientMaterial[]	= {0.02, 0.02, 0.02, 1.0};
GLfloat BRDiffuseMaterial[]	= {0.01, 0.01, 0.01, 1.0};
GLfloat BRSpecularMaterial[]	= { 0.4, 0.4, 0.4, 1.0};
GLfloat BRShininessMaterial[]	= {10.0};
//Gold material
GLfloat GoldAmbientMaterial[]	= {0.24725, 0.1995, 0.0745, 1.0};
GLfloat GoldDiffuseMaterial[]	= {0.75164, 0.60648, 0.22648, 1.0};
GLfloat GoldSpecularMaterial[]	= {0.628281, 0.555802, 0.366065, 1.0};
GLfloat GoldShininessMaterial[]	= {51.2};
//Bronze material
GLfloat BronzeAmbientMaterial[]	= {0.2125, 0.1275, 0.054, 1.0};
GLfloat BronzeDiffuseMaterial[]	= {0.714, 0.4284, 0.18144, 1.0};
GLfloat BronzeSpecularMaterial[]	= {0.393548, 0.271906, 0.166721, 1.0};
GLfloat BronzeShininessMaterial[]	= {25.6};
//green plastic
GLfloat GPAmbientMaterial[]	= {0.0,1.0,0.0,0.5};
GLfloat GPDiffuseMaterial[]	= {0.1,0.35,0.1};
GLfloat GPSpecularMaterial[]	= {0.45,0.55,0.45};
GLfloat GPShininessMaterial[]	= {1.0};
// scene parameters
Vec3f bbxMin, bbxMax, bbxCenter;
//FULL INTENSE LIGHT 
GLfloat IntenseAmbientLight[]		= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat IntenseDiffuseLight[]		= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat IntenseSpecularLight[]		= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat IntenseLightPos[]			= {-bbxCenter.x, -bbxCenter.y, -bbxCenter.z,1.0f};
//character parameters
float speed=0.9f;
float rotateLegDog=0.0f;//rotate legs
float dogRotate = 0.0f;//rotate dog
float positionX,positionY,positionZ=0.0f;
bool lighting = false;
bool isGlow = false;
bool isRotate = false;
//object pointer to quadric
GLUquadric* quadratic=gluNewQuadric();

enum {
  MENU_FLAT= 1,
  MENU_SMOOTH=2,
  MENU_EXIT
};
// mesh model
Mesh* model = 0;
Mesh* plane= new Mesh;
// models
static const char* modelfile[] = {
	"al.obj"
};
// angle of rotation for the camera direction
float camera_y=0.0f;
/* 
 * Facilities for drawing models. The functions below show:
 * - How to use ILM(Industrial Light & Magic)'s math library
 * - Mesh data structure
 * - How to draw the mesh in immediate mode
 */
/* 
 * Compute the bounding box of the model
 */
void calc_bbox(Vec3f& bbxmin, Vec3f& bbxmax)
{
	vector<Vec3f>& v = model->m_v;

	if(v.empty()) return;		

	bbxmax = bbxmin= v[0];

	for(unsigned int i = 1;i < v.size();i++)
	{	
		bbxmin.x = MIN2(bbxmin.x, v[i].x);
		bbxmin.y = MIN2(bbxmin.y, v[i].y);
		bbxmin.z = MIN2(bbxmin.z, v[i].z);

		bbxmax.x = MAX2(bbxmax.x, v[i].x);
		bbxmax.y = MAX2(bbxmax.y, v[i].y);
		bbxmax.z = MAX2(bbxmax.z, v[i].z);
	}
}
/* 
 * Load a model from a file
 */
void load_model(const char* name)
{
	if(model) delete model;
	model = ObjLoader::load(name);
	calc_bbox(bbxMin, bbxMax);
	bbxCenter = (bbxMin+bbxMax)*0.5f;
}
/* 
 * Draw the model in immediate mode
 */
void draw_model()
{
	// data references for convenience purpose
	vector<int>& vi = model->m_vi;
	vector<int>& ni = model->m_ni;
	vector<int>& ti = model->m_ti;
	vector<Vec3f>& v = model->m_v;
	vector<Vec3f>& n = model->m_n;
	vector<Vec2f>& t = model->m_texc;

	// drawing in immediate mode
	glBegin(GL_TRIANGLES);

	for(unsigned int i = 0;i < vi.size();i++)
	{
		// normals and texture coordiates is not used.
		// to achieve lighting, per vertex normal has to be attached
		// *** Note: normals are sometimes absent from the data.You have to create
		// *** them yourself

		/*
		if(!n.empty() && !ni.empty())
			glNormal3fv(&n[ni[i]].x);

		if(!t.empty() && !ti.empty())
			glTexCoord2fv(&t[ti[i]].x);
		*/

		// assign some funny color to each vertex
		Vec3f offset = (v[vi[i]] - bbxCenter);;
		glColor3f(fabs(sin(offset.x)), fabs(cos(offset.y)), fabs(offset.z));

		glVertex3fv(&v[vi[i]].x);
	}

	glEnd();
}
// ================================================================================
// Main program functions
void renderBitmapString(float x, float y, float z, char *string)
{
   char *c;
   
   glRasterPos3f(x, y, z);   // fonts position
   for(c=string; *c != '\0'; c++)
     glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
}
void shut()
{
	if(model) delete model;
}
void modelMenu (int menuId)
{
	load_model(modelfile[menuId]);

	// adjust the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	farPlane = nearPlane+4.0f*(bbxMax-bbxMin).length();
	gluPerspective(fieldofview, aspect, nearPlane, farPlane);

	// adjust the viewing matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float cz = 2.0f*(bbxMax.z - bbxMin.z);

	/*gluLookAt(0.0f, 0.0f, cz, 
		0.0f, 0.0f, cz - 1.0f, 
		0.0f, 1.0f, 0.0f);8*/

	glutPostRedisplay();
}
void UI(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_UP:
		bbxCenter.z -= (float(1)/height)*360.0;
		break;
	case GLUT_KEY_DOWN:
		bbxCenter.z += (float(1)/width)*360.0;
		break;
	case GLUT_KEY_LEFT:
		y_angle -= (float(1)/height)*360.0;
		break;
	case GLUT_KEY_RIGHT:
		y_angle +=(float(1)/height)*360.0;
		break;
	}
	glutPostRedisplay();
}
void menu(int menuId)
{
	switch(menuId)
	{
	case MENU_FLAT:
		printf("%s", "flat");
		glShadeModel(GL_FLAT);
		break;
	case MENU_SMOOTH:
		printf("%s", "smooth");
		glShadeModel(GL_SMOOTH);
		break;
	case MENU_EXIT:
		exit(0);
		break;
	}
	glutPostRedisplay();
}
int glutMenus (void)
{	
	int subMenu_models = glutCreateMenu(menu);
	glutAddMenuEntry("flat-shading", MENU_FLAT);
	glutAddMenuEntry("smooth-shading", MENU_SMOOTH);
	int MainMenu=glutCreateMenu(menu);
	glutAddSubMenu("Shading", subMenu_models);
	glutAddMenuEntry("Quit", MENU_EXIT);	
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
	return MainMenu;
}
//initializing the plane parameters once
void chrome()
{
	//material effect
	glMaterialfv(GL_FRONT,GL_AMBIENT, AmbientMaterial);
	glMaterialfv(GL_FRONT,GL_DIFFUSE, DiffuseMaterial);
	glMaterialfv(GL_FRONT,GL_SPECULAR,SpecularMaterial);
	glMaterialfv(GL_FRONT,GL_SHININESS,ShininessMaterial);
}
void silverMaterial()
{
	//material effect
	glMaterialfv(GL_FRONT,GL_AMBIENT, SilverAmbientMaterial);
	glMaterialfv(GL_FRONT,GL_DIFFUSE, SilverDiffuseMaterial);
	glMaterialfv(GL_FRONT,GL_SPECULAR,SilverSpecularMaterial);
	glMaterialfv(GL_FRONT,GL_SHININESS,SilverShininessMaterial);
}
void TurquoiseMaterial()
{
	//material effect
	glMaterialfv(GL_FRONT,GL_AMBIENT, TurquoiseAmbientMaterial);
	glMaterialfv(GL_FRONT,GL_DIFFUSE, TurquoiseDiffuseMaterial);
	glMaterialfv(GL_FRONT,GL_SPECULAR,TurquoiseSpecularMaterial);
	glMaterialfv(GL_FRONT,GL_SHININESS,TurquoiseShininessMaterial);
}
void BRMaterial()
{
	glMaterialfv(GL_FRONT,GL_AMBIENT, BRAmbientMaterial);
	glMaterialfv(GL_FRONT,GL_DIFFUSE, BRDiffuseMaterial);
	glMaterialfv(GL_FRONT,GL_SPECULAR,BRSpecularMaterial);
	glMaterialfv(GL_FRONT,GL_SHININESS,BRShininessMaterial);
}
void GoldMaterial()
{
	glMaterialfv(GL_FRONT,GL_AMBIENT, GoldAmbientMaterial);
	glMaterialfv(GL_FRONT,GL_DIFFUSE, GoldDiffuseMaterial);
	glMaterialfv(GL_FRONT,GL_SPECULAR,GoldSpecularMaterial);
	glMaterialfv(GL_FRONT,GL_SHININESS,GoldShininessMaterial);
}
void BronzeMaterial()
{
	glMaterialfv(GL_FRONT,GL_AMBIENT, BronzeAmbientMaterial);
	glMaterialfv(GL_FRONT,GL_DIFFUSE, BronzeDiffuseMaterial);
	glMaterialfv(GL_FRONT,GL_SPECULAR,BronzeSpecularMaterial);
	glMaterialfv(GL_FRONT,GL_SHININESS,BronzeShininessMaterial);
}
void GPMaterial()
{
	glMaterialfv(GL_FRONT,GL_AMBIENT, GPAmbientMaterial);
	glMaterialfv(GL_FRONT,GL_DIFFUSE, GPDiffuseMaterial);
	glMaterialfv(GL_FRONT,GL_SPECULAR,GPSpecularMaterial);
	glMaterialfv(GL_FRONT,GL_SHININESS,GPShininessMaterial);
}
void planeCreation(){
	//index
	plane->m_vi.push_back(0);
	plane->m_vi.push_back(1);
	plane->m_vi.push_back(2);
	plane->m_vi.push_back(0);
	plane->m_vi.push_back(2);
	plane->m_vi.push_back(3);
	//verticies
	plane->m_v.push_back(V3f(-5,-2,5));
	plane->m_v.push_back(V3f(5,-2,5));	
	plane->m_v.push_back(V3f(5,-2,-5));
	plane->m_v.push_back(V3f(-5,-2,-5));
}
//initializing the ball sphere parmeters once
void fullIntenseLight()
{	
	if(lighting==true)
	{
		glLightfv(GL_LIGHT0, GL_AMBIENT, IntenseAmbientLight);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, IntenseDiffuseLight);
		glLightfv(GL_LIGHT0, GL_SPECULAR, IntenseSpecularLight);
		glEnable(GL_LIGHT0);
	}
	if(lighting==false)
	{
		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
		glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
		glEnable(GL_LIGHT0);	
	}
}

void charManip(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'f':
		printf("%f ",positionX);
		positionX+=speed;
		break;
	case 'g':
		printf("%f ",positionX);
		positionX-=speed;
		
		break;
	case 'r'://
		positionX = 0.0f;
		positionY = 0.0f;
		positionZ = 0.0f;
		break;
	case 'l'://rotate right
		dogRotate-=30;
		//rotate
		break;
	case 'k'://rotate left
		dogRotate+=30;
		break;
	case 'c'://light
		if(lighting==false)
		{
			lighting=true;
			fullIntenseLight();
		}
		else
		{
			lighting=false;
			fullIntenseLight();
		}
		break;
	case 'a'://controlling elevelation of the plane
		bbxCenter.x-=1;
		break;
	case 's':
		bbxCenter.x+=1;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
void init()
{	
	// use black to clean background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	// always re-normalize normal (due to scale transform)
	glEnable(GL_NORMALIZE);

	// lighting (disabled)
	// To achieve lighting effect
	//	- enable lighting
	//	- setup lighting parameters
	//	- setup materials
	//	- attach normals to each vertex
	
	glDisable(GL_LIGHTING);
	 
	glEnable(GL_LIGHTING);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
	glEnable(GL_LIGHT0);	


	// smooth shading
	glShadeModel(GL_SMOOTH);

	// load the first model by default
	load_model(modelfile[0]);
	
	// setup projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	aspect = (double)width/(double)height;
	farPlane = nearPlane+4.0f*(bbxMax-bbxMin).length();
	gluPerspective(fieldofview, aspect, nearPlane, farPlane);

	// setup viewing matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float cz = 2.0f*(bbxMax.z - bbxMin.z);
	bbxCenter.z+=5;
	gluLookAt(0.0f, 0.0f, cz, 
		0.0f, 0.0f, cz - 1.0f, 
		0.0f, 1.0f, 0.0f);

	// set up menus
	glutMenus();
}
void createGround()
{	
	GPMaterial();
	glBegin(GL_TRIANGLES);

	for(unsigned int i = 0;i < plane->m_vi.size();i++)
	{
		//printf("%s","InForLoop");
		// assign some funny color to each vertex
		glNormal3f(0,1,0);
		glColor3f(0.0,0.0,1.0);
		glVertex3fv(&plane->m_v[plane->m_vi[i]].x);

	}
	//printf("%s","Hello");
	glEnd();
}
void glowingEyes()
{
	GLfloat nonGlowColor[] = {0.0,0.0,0.0};
	if(isGlow==true)
	{
		glMaterialfv(GL_FRONT,GL_EMISSION,glowColor);
	}
	else
		glMaterialfv(GL_FRONT,GL_EMISSION,nonGlowColor);
}
void draw_Head()
{
	//head
	gluSphere(quadratic,0.35,200,200);
	//nose
	glPushMatrix();
	isGlow=true;
	glowingEyes();
	BronzeMaterial();
	glTranslatef(0.3f,0.0f,0.0f);
	glColor3f(1.0f,1.0f,1.0f);
	glScalef(2.0f,1.0f,1.0f);
	gluSphere(quadratic,0.1,200,200);
	glPopMatrix();
	
	//eyeballs
	glPushMatrix();
	BRMaterial();
	glTranslatef(0.25f,0.2f,0.14f);
	gluSphere(quadratic,0.05,200,200);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.25f,0.2f,-0.14f);
	gluSphere(quadratic,0.05,200,200);
	glPopMatrix();
	
	//ears
	glPushMatrix();
	glTranslatef(0.05,0.2,0.2);
	glScalef(1.0,2.0,2.0);
	gluSphere(quadratic,0.08,200,200);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.05,0.2,-0.2);
	glScalef(1.0,2.0,2.0);
	gluSphere(quadratic,0.08,200,200);
	isGlow=false;
	glowingEyes();
	glPopMatrix();
}
void draw_leg()
{
	glColor3f(0.5,0.5,0.5);
	gluSphere(quadratic,0.2,200,200);	//upperleg
	glTranslatef(0.0f,-0.3f,-0.0f);
	gluSphere(quadratic,0.2,200,200);	//lower leg
}
void draw_tail()
{
	glColor3f(0.24f,0.24f,0.24f);
	gluSphere(quadratic,0.2,200,200);
}
void draw_torso()
{
	//drawing torso
	glColor3f(0.0f,1.0f,1.0f);
	chrome();
	gluSphere(quadratic,0.5,200,200);
	glColor3f(0.0f,1.0f,0.0f);
	
	//drawing leg
	glPushMatrix();
	silverMaterial();
	glRotatef(-cos(rotateLegDog)*5,0,0 ,1);
	glTranslatef(0.3f ,-0.45f, -0.3f);
	glRotatef(cos(rotateLegDog)*5,0,0 ,1);
	draw_leg();
	glPopMatrix();

	glPushMatrix();
	glRotatef(-cos(rotateLegDog)*5,0,0 ,1);
	glTranslatef(0.3f ,-0.45f, 0.3f);
	glRotatef(cos(rotateLegDog)*5,0,0 ,1);
	draw_leg();
	glPopMatrix();

	glPushMatrix();
	glRotatef(sin(rotateLegDog)*5,0,0 ,1);
	glTranslatef(-0.3f ,-0.45f, -0.3f);
	glRotatef(-sin(rotateLegDog)*5,0,0 ,1);
	draw_leg();
	glPopMatrix();

	glPushMatrix();
	glRotatef(sin(rotateLegDog)*5,0,0 ,1);
	glTranslatef(-0.3f ,-0.45f, 0.3f);
	glRotatef(-sin(rotateLegDog)*5,0,0 ,1);
	draw_leg();
	glPopMatrix();

	//draw head
	glPushMatrix();
	TurquoiseMaterial();
	glTranslatef(0.4f,0.4f,0.0f);
	draw_Head();
	glPopMatrix();
	
	//draw tail
	glPushMatrix();
	GoldMaterial();
	glTranslatef(-0.4f,0.3f,0.0f);
	draw_tail();
	glPopMatrix();
	
}
void draw_doggie()
{
	draw_torso();
}
void myIdle()
{
	positionX+= cos(dogRotate * M_PI/180);
	positionZ+= sin(-dogRotate* M_PI/180);
	rotateLegDog+=1;
	glutPostRedisplay();
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
		//moving the world away from camera
		glScalef(scale, scale, scale);
		glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
		glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
		glTranslatef(-bbxCenter.x, -bbxCenter.y, -bbxCenter.z);
		//draw_model();	
		GPMaterial();
		createGround();
		
		glTranslatef(positionX,positionY,positionZ);
		glRotatef(dogRotate,0,1,0);
		draw_doggie();
		
		//printf("%s","creatGround");
	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,width, 0, height);	

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	renderBitmapString(0.0, height-13.0f, 0.0f, "Use [Mouse Left Key] to rotate");	
	renderBitmapString(0.0, height-26.0f, 0.0f, "Use [Mouse Right Key] to scale");
	renderBitmapString(0.0, height-39.0f, 0.0f, "Press [Mouse Middle key] to summon out menu");
		
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	glutSwapBuffers();
}
void mouse(int button, int state, int x, int y)
{
	// button -- GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON.
	// state  -- GLUT_UP, GLUT_DOWN

	mouse_x = x;
	mouse_y = y;

	mouse_button = button;
}
void reshape(int x, int y)
{
    width  = x;
	height = y;

	if (height == 0)		// not divided by zero
		height = 1;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0,0,width,height);

	aspect = (float)width/(float)height;

	gluPerspective(fieldofview, aspect, nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);

}
void motion(int x, int y)
{
	if(mouse_button == GLUT_LEFT_BUTTON)
	{
		// compute the angle (0..360) around x axis, and y-axis
		y_angle += (float(x - mouse_x)/width)*360.0;
		x_angle += (float(y - mouse_y)/height)*360.0;

	}

	if(mouse_button == GLUT_RIGHT_BUTTON)
	{
		// scale
		scale += (y-mouse_y)/100.0;
			
		if(scale < 0.1) scale = 0.1;     // too small
		if(scale > 7)	scale = 7;		 // too big

	}

	mouse_x	= x;		// update current mouse position
	mouse_y	= y;
	glutPostRedisplay();

}
void main(int argc, char* argv[])
{
	glutInit(&argc,argv);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(position_x,position_y);
	glutInitWindowSize(width,height);
	winId = glutCreateWindow("Mesh Viewer");
	
	
	//ballCreation();
	glutMenus();
	glutIdleFunc(myIdle);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutDisplayFunc(display);		// display function
	glutSpecialFunc(UI);
	glutKeyboardFunc(charManip);
	planeCreation();
	init();

	glutMainLoop();

	shut();
}






