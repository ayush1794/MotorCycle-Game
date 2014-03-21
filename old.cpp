#include "includes.h"

#define PI 3.141592653589
#define LEN 8192

using namespace std;

double dim=100,Ex,Ey,Ez;
int fov=4;
double asp=1;
int keyPressed =0;
int mouseX,mouseY=0;
int toggleAxis=0;
int toggleValues=1;
bool tilting=false;
int th=0;   	//azimuthal
int ph=0;	//elevation
bool keys[256];
int objId=0;
int objSlices=16;
int objStacks=16;
int test=0;
Terrain* _terrain;

class Bike{
	float YAW,PITCH,ROLL;
	float bike_x,bike_y,bike_z;
	float vel,vel_x,vel_y,vel_z,acc_x,acc_y,acc_z,tyre_rot,tyre_turn;
	public:
		Bike();
		void draw(void){		
			drawBike(bike_x, bike_y, bike_z,
				BIKE_LEN, BIKE_HIGH, BIKE_WID,
				ROLL, YAW, PITCH, 
				tyre_rot,tyre_turn);
			printAt(5,5,"%f",ROLL);
		}
		void speedup()
		{
			if(vel<10.0){
				vel+= ( ( BIKE_ACC - ( FRIC_CONST * G * cosine(PITCH) ) ) / 40);	//should increase by 5 over 40 frames
			}
			else vel=10.0;
			tyre_rot-=(vel/TYRE_RADIUS);
		}
		void turn_left()
		{
			if(tyre_turn<=45.0)
				tyre_turn+=0.5;
			ROLL = RAD2DEG(atan( tan(DEG2RAD(-1*tyre_turn)) * (vel) ));
		}
		void turn_right()
		{
			if(tyre_turn>=(-45.0))
				tyre_turn-=0.5;
			ROLL = RAD2DEG(atan( tan(DEG2RAD(-1*tyre_turn)) * (vel) ));
		}
		void tilt_left()
		{
			ROLL-=1.0;
		}
		void tilt_right()
		{
			ROLL+=1.0;
		}
		void stablize()
		{
			if(tyre_turn>0)
				tyre_turn-=1.0;
			else
				tyre_turn+=1.0;
			ROLL = RAD2DEG(atan( tan(DEG2RAD(-1*tyre_turn)) * (vel) ));
			/*if(ROLL>0)
				ROLL--;
			else
				ROLL++;*/
		}
		void brakes()
		{
			if(vel>0)
				vel-=BIKE_ACC/800;
			else
				vel=0;


		}
		void slowdown()
		{
			if(vel>0)
				vel-=( ( FRIC_CONST * G * cosine(PITCH) ) / 8000 );
			else
				vel=0;
		}
		void setpos()
		{
			if(vel>0)
				YAW+=(tyre_turn*vel);
			vel_x=vel*cosine(YAW);
			vel_z=vel*sine(-1*YAW);
			bike_x+=vel_x;
			bike_y+=vel_y;
			bike_z+=vel_z;
		}
};

Bike::Bike(){
	YAW=PITCH=ROLL=0;
	bike_x=bike_z=BIKE_INIT;
	bike_y=-4.0;
	vel_x=vel_y=vel_z=acc_x=acc_y=acc_z=0;
	tyre_rot=tyre_turn=0;
}

Bike bike;

void drawAxis()
{
	if(toggleAxis){
		double len=5.0;
		glColor3f(1.0,1.0,1.0);
		glBegin(GL_LINES);
		glVertex3d(0,0,0);
		glVertex3d(len,0,0);
		glVertex3d(0,0,0);
		glVertex3d(0,len,0);
		glVertex3d(0,0,0);
		glVertex3d(0,0,len);
		glEnd();

		glRasterPos3d(len,0,0);
		print("X");
		glRasterPos3d(0,len,0);
		print("Y");
		glRasterPos3d(0,0,len);
		print("Z");
	}
}

void drawValues()
{
	if(toggleValues){
		glColor3f(1,1,1);
		printAt(5,5, "View Angle (th,ph)=(%d,%d)",th,ph);
	}
}

void drawTerrain()
{
	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	glColor3f(0.3f, 0.9f, 0.0f);
	for(int z = 0; z < _terrain->length() - 1; z++) {
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < _terrain->width(); x++) {
			Vec3f normal = _terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z), z);
			normal = _terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}
}

void display(){
	double angle;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();				// Loads the 4X4 Identity matrix.

/*
	glRotated(ph,1,0,0);
	glRotated(th,0,1,0);
*/

	Ex= -2*dim*sine(th)*cosine(ph);
	Ey=  2*dim	*sine(ph);
	Ez=  2*dim*cosine(th)*cosine(ph);
	
	gluLookAt(Ex,Ey,Ez, 0,0,0, 0,cosine(ph),0);
	
	drawTerrain();
	bike.draw();
	 
	drawAxis();
	drawValues();
	glFlush();
	glutSwapBuffers();
}

void project(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,asp,dim/4,dim*4);
	//glOrtho(-dim*w2h,dim*w2h,-dim,dim,-dim,dim);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void reshape(int width, int height){
	asp = (height > 0) ? (double)width/height :1;
	glViewport(0,0,width,height);
	project();
}


void winKey(unsigned char key,int x,int y){
	int currModifiers = glutGetModifiers();
	keys[key]=true;
	/* Exit on Ecs */
	if(key==27)exit(0);
	else if(key =='x'|| key == 'X')
	{
		toggleAxis=1-toggleAxis;
	}
	else if(key =='v'||key=='V')
		toggleValues=1-toggleValues;
	else if(key=='+' && key <179)
		fov++;
	else if(key=='-' &&key>1)
		fov--;

	project();
	glutPostRedisplay();
}

void funkey(int key,int x,int y){
	int currModifiers = glutGetModifiers();
	if(key==GLUT_KEY_RIGHT)th+=5;
	if(key==GLUT_KEY_LEFT)th-=5;
	if(key==GLUT_KEY_UP)ph+=5;
	if(key==GLUT_KEY_DOWN)ph-=5;

	th %=360;
	ph %=360;

	glutPostRedisplay();
}

void windowMenu(int value){
	winKey((unsigned char)value,0,0);
}

void upKeyBoard(unsigned char key, int x,int y)
{
	keys[key]=false;
}

void update(int value)
{
	if(keys['w']==true||keys['W']==true)
		bike.speedup();
	else if(keys['s']==true||keys['S']==true)
		bike.brakes();
	else
		bike.slowdown();
	
	if(keys['a']==true||keys['A']==true)
	       	bike.tilt_left();
	else if(keys['d']==true||keys['D']==true) 
		bike.tilt_right();
	/*else{
		bike.stablize();
	}*/

	if(keys['l']==true)
		bike.turn_right();
	else if(keys['j']==true)
		bike.turn_left();
	else{
		bike.stablize();
	}
	bike.setpos();	

	glutPostRedisplay();
	glutTimerFunc(10,update,0);
}

Terrain* loadTerrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * (y * image->width + x)];
			float h = height * ((color / 255.0f) - 0.5f);
			t->setHeight(x, y, h);
		}
	}
	
	delete image;
	t->computeNormals();
	return t;
}

void cleanup() {
	delete _terrain;
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

int main(int argc, char ** argv)
{
	for(int i=0;i<256;i++)
		keys[i]=false;
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("Get Inspired...");
	initRendering();
	
	_terrain = loadTerrain("try.bmp", 8);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(winKey);
	glutKeyboardUpFunc(upKeyBoard);
	glutSpecialFunc(funkey);

	glutCreateMenu(windowMenu);
	glutAddMenuEntry("Toggle axes [a]",'a');
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutTimerFunc(100,update,0);
	glutMainLoop();
	return 0;
}
