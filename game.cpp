#include "includes.h"
#include "sound.h"

#define PI 3.141592653589
#define LEN 8192

using namespace std;

double dim=100,Ex,Ey,Ez,lx,ly,lz,ux,uy,uz,hx,hy,hz,Hx,Hy,Hz;
int fov=20;
double asp=1;
int score=0;
int keyPressed =0;
int toggleAxis=1;
int toggleValues=1;
int toggleCameraView=0;
int th=0;   	//azimuthal
int ph=0;	//elevation
bool keys[256];
int objId=0;
int objSlices=16;
int objStacks=16;
int test=0;
int mouseX=0;
float angle,toggleDay=true;
Terrain* _terrain;
GLuint textureID1,textureID2,textureID3;
GLUquadric *sun,*moon;
bool fly=false;
bool heli=true,morning=false;
bool wr=true;
float dayR,dayG,dayB,skyR,skyG,skyB;
float sunX=200.0,sunY=33.5,sunAngle=90.0,moonX,moonY;
int level=5;
int count=0;
int time_left;
int level_req=FOSS_TOTAL;
int start_time=5;
int time_frac=1000;
int hung=0;

class Fossil{
    bool collected;
    public:
    float x,y,z;
    Fossil(float x1,float y1);
    void draw(float color[3]){
        drawFossil(x,y,z,angle,color);
    }
    bool isCollected()
    {
        return collected;
    }
    void collect(float x1,float z1)
    {
        if(!collected&&dis(x,z,x1,z1)<1){
            alSourcefv(Sources[COIN],AL_POSITION, SourcesPos[COIN]);
            alSourcePlay(Sources[COIN]);
            score+=1;
            collected=true;
        }
    }
    void dropped()
    {
        score-=1;
        collected=false;
    }
};

Fossil::Fossil(float x1,float z1){
    x=x1;z=z1;
    y=_terrain->getHeight((int)x,(int)z)+1.0;
    collected=false;
}

vector<Fossil> fossils;

class Bike{
    float prev_height;
    float YAW,PITCH,ROLL;
    float bike_x,bike_y,bike_z,front_tyrex,front_tyrez,back_tyrex,back_tyrez;
    float vel,vel_x,vel_y,vel_z,acc_x,acc_y,acc_z,tyre_rot,tyre_turn;
    public:
    Bike();
    void speedup()
    {
        if(vel<1.5){
            vel+= ( ( BIKE_ACC - ( FRIC_CONST * G * cosine(PITCH) ) ) / 40);	//should increase by 5 over 40 frames
        }
        else vel=1.5;
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
    float getVel()
    {
        return vel;
    }
    void brakes()
    {
        if(vel>0)
            vel-=BIKE_ACC/80;
        else
            vel+=BIKE_ACC/80;
    }
    void slowdown()
    {
        if(vel!=0)
            vel=vel + ( G * sine(-1*PITCH) /100 ) - (vel/abs(vel))* ( ( FRIC_CONST * G * cosine(PITCH) ) / 400 );
        if(vel>1.5)
            vel=1.5;
    }
    void flying()
    {
        vel_y -= G/400;
    }
    void checkFly()
    {
        if(fly==false)
        {
            if(_terrain->getHeight((int)bike_z,(int)bike_z)<prev_height)
            {
                fly=true;
            }
        }
        else
        {
            if(bike_y<=_terrain->getHeight((int)bike_x,(int)bike_z))
            { 
                fly=false;
            }
        }
        prev_height=_terrain->getHeight((int)bike_z,(int)bike_z);
    }
    void setpos()
    {
        tyre_rot-=(vel/TYRE_RADIUS);
        YAW+=(tyre_turn*vel/5);
        vel_x=vel*cosine(YAW)*cosine(PITCH);
        vel_z=vel*sine(-1*YAW)*cosine(PITCH);
        if(fly==false)
            vel_y=vel*sine(PITCH);
        else
            flying();
        bike_x+=vel_x;
        if(bike_x>399.87)
            bike_x=399.87;
        else if(bike_x<0)
            bike_x=0.0;
        bike_y+=vel_y;
        if(fly==false)
        {
            if(bike_y<_terrain->getHeight((int)bike_x,(int)bike_z) || bike_y>_terrain->getHeight((int)bike_x,(int)bike_z) )bike_y =_terrain->getHeight((int)bike_x,(int)bike_z);
        }
        else
        {
            if(bike_y<_terrain->getHeight((int)bike_x,(int)bike_z))bike_y =_terrain->getHeight((int)bike_x,(int)bike_z);
        }
        //	bike_y+=vel_y;
        bike_z+=vel_z;
        if(bike_z>398.87)
            bike_z=398.87;
        else if(bike_z<0)
            bike_z=0.0;
    }
    float getpos(int cor)
    {
        switch(cor)
        {
            case 'x' : return bike_x;
                       break;
            case 'y' : return bike_y;
                       break;
            case 'z' : return bike_z;
                       break;
        }
    }
    float getOrient(int cor)
    {
        switch(cor)
        {
            case 'x' : return ROLL;
                       break;
            case 'y' : return YAW;
                       break;
            case 'z' : return PITCH;
                       break;
        }
    }
    void setFrontTyre()
    {
        front_tyrex=( cosine(-1*YAW)*cosine(0) )*(BIKE_LEN/2) - ( cosine(ROLL)*cosine(-1*YAW)*sine(0) + sine(-1*YAW)*sine(ROLL) )*BIKE_HIGH/2 
            + bike_x;

        front_tyrez= (sine(-1*YAW)*cosine(0))*(BIKE_LEN/2) + ( cosine(-1*YAW)*sine(ROLL) - cosine(ROLL)*sine(-1*YAW)*sine(0) )*BIKE_HIGH/2 
            + bike_z;
    }
    void setBackTyre()
    {
        back_tyrex=( cosine(-1*YAW)*cosine(0) )*(-1*BIKE_LEN/2) - ( cosine(ROLL)*cosine(-1*YAW)*sine(0) + sine(-1*YAW)*sine(ROLL) )*BIKE_HIGH/2 
            + bike_x;

        back_tyrez= (sine(-1*YAW)*cosine(0))*(-1*BIKE_LEN/2) + ( cosine(-1*YAW)*sine(ROLL) - cosine(ROLL)*sine(-1*YAW)*sine(0) )*BIKE_HIGH/2 
            + bike_z;
    }
    void getHelmetUp()
    {
        ux=  ( cosine(-1*YAW)*cosine(PITCH) )*(0) - ( cosine(ROLL)*cosine(-1*YAW)*sine(PITCH) + sine(-1*YAW)*sine(ROLL) )*BIKE_HIGH ;

        uy=  ( cosine(PITCH)*cosine(ROLL) )*BIKE_HIGH  ;

        uz=  ( cosine(-1*YAW)*sine(ROLL) - cosine(ROLL)*sine(-1*YAW)*sine(PITCH) )*BIKE_HIGH ;
    }
    void lookFromBack()
    {
        Ex=( cosine(-1*YAW)*cosine(PITCH) )*(-10) - ( cosine(ROLL)*cosine(-1*YAW)*sine(PITCH) + sine(-1*YAW)*sine(ROLL) )*BIKE_HIGH/0.3 
            + bike_x;

        Ey= sine(PITCH)*(BIKE_LEN) + ( cosine(PITCH)*cosine(ROLL) )*BIKE_HIGH/0.3
            + bike_y;

        Ez= (sine(-1*YAW)*cosine(PITCH))*(-10) + ( cosine(-1*YAW)*sine(ROLL) - cosine(ROLL)*sine(-1*YAW)*sine(PITCH) )*BIKE_HIGH/0.3
            + bike_z;
    }
    void lookFromTyre()
    {
        Ex=( cosine(-1*YAW)*cosine(PITCH) )*(BIKE_LEN/2) - ( cosine(ROLL)*cosine(-1*YAW)*sine(PITCH) + sine(-1*YAW)*sine(ROLL) )*BIKE_HIGH/0.8 
            + bike_x;

        Ey= sine(PITCH)*(BIKE_LEN/2) + ( cosine(PITCH)*cosine(ROLL) )*BIKE_HIGH/0.8
            + bike_y;

        Ez= (sine(-1*YAW)*cosine(PITCH))*(BIKE_LEN/2) + ( cosine(-1*YAW)*sine(ROLL) - cosine(ROLL)*sine(-1*YAW)*sine(PITCH) )*BIKE_HIGH/0.8
            + bike_z;
    }
    void lookingFromTyre()
    {

        lx=( cosine(-1*YAW)*cosine(PITCH) )*(BIKE_LEN/2+1) - ( cosine(ROLL)*cosine(-1*YAW)*sine(PITCH) + sine(-1*YAW)*sine(ROLL) )*BIKE_HIGH/0.8 
            + bike_x;

        ly= sine(PITCH)*(BIKE_LEN/2+1) + ( cosine(PITCH)*cosine(ROLL) )*BIKE_HIGH/0.8
            + bike_y;

        lz= (sine(-1*YAW)*cosine(PITCH))*(BIKE_LEN/2+1) + ( cosine(-1*YAW)*sine(ROLL) - cosine(ROLL)*sine(-1*YAW)*sine(PITCH) )*BIKE_HIGH/0.8
            + bike_z;
    }
    void getHeadLight()
    {
        Hx=( cosine(-1*YAW)*cosine(PITCH) )*(BIKE_LEN/2) - ( cosine(ROLL)*cosine(-1*YAW)*sine(PITCH) + sine(-1*YAW)*sine(ROLL) )*BIKE_HIGH/2
            + bike_x;

        Hy= sine(PITCH)*(BIKE_LEN/2) + ( cosine(PITCH)*cosine(ROLL) )*BIKE_HIGH/2
            + bike_y;

        Hz= (sine(-1*YAW)*cosine(PITCH))*(BIKE_LEN/2) + ( cosine(-1*YAW)*sine(ROLL) - cosine(ROLL)*sine(-1*YAW)*sine(PITCH) )*BIKE_HIGH/2
            + bike_z;
    }
    void headlightDirection()
    {
        hx=( cosine(-1*YAW)*cosine(PITCH) )*(BIKE_LEN/2) - ( cosine(ROLL)*cosine(-1*YAW)*sine(PITCH) + sine(-1*YAW)*sine(ROLL) )*(BIKE_HIGH/2) 
            + bike_x;

        hy= sine(PITCH)*(BIKE_LEN/2) + ( cosine(PITCH)*cosine(ROLL) )*(BIKE_HIGH/2)  
            + bike_y;

        hz= (sine(-1*YAW)*cosine(PITCH))*(BIKE_LEN/2) + ( cosine(-1*YAW)*sine(ROLL) - cosine(ROLL)*sine(-1*YAW)*sine(PITCH) )*(BIKE_HIGH/2)
            + bike_z;
    }
    void lookFromHelmet()
    {
        Ex=( cosine(-1*YAW)*cosine(PITCH) )*(-1*BIKE_LEN/1.5) - ( cosine(ROLL)*cosine(-1*YAW)*sine(PITCH) + sine(-1*YAW)*sine(ROLL) )*BIKE_HIGH/0.4 
            + bike_x;

        Ey= sine(PITCH)*(-1*BIKE_LEN/1.5) + ( cosine(PITCH)*cosine(ROLL) )*BIKE_HIGH/0.4
            + bike_y;

        Ez= (sine(-1*YAW)*cosine(PITCH))*(-1*BIKE_LEN/1.5) + ( cosine(-1*YAW)*sine(ROLL) - cosine(ROLL)*sine(-1*YAW)*sine(PITCH) )*BIKE_HIGH/0.4
            + bike_z;
    }
    void lookingFromHelmet()
    {

        lx=( cosine(-1*YAW)*cosine(PITCH) )*(BIKE_LEN/10+1.2) - ( cosine(ROLL)*cosine(-1*YAW)*sine(PITCH) + sine(-1*YAW)*sine(ROLL) )*BIKE_HIGH/0.6
            + bike_x;

        ly= sine(PITCH)*(BIKE_LEN/10+1.2) + ( cosine(PITCH)*cosine(ROLL) )*BIKE_HIGH/0.6
            + bike_y;

        lz= (sine(-1*YAW)*cosine(PITCH))*(BIKE_LEN/10+1.2) + ( cosine(-1*YAW)*sine(ROLL) - cosine(ROLL)*sine(-1*YAW)*sine(PITCH) )*BIKE_HIGH/0.6
            + bike_z;
    }
    void getPitch()
    {
        setFrontTyre();
        setBackTyre();
        PITCH=RAD2DEG(atan( (_terrain->getHeight((int)front_tyrex,(int)front_tyrez) - _terrain->getHeight((int)back_tyrex,(int)back_tyrez)) / BIKE_LEN ));

    }
    void draw(void){
        if(fly==false)
            getPitch();
        drawBike(bike_x, bike_y, bike_z,
                BIKE_LEN, BIKE_HIGH, BIKE_WID,
                ROLL, YAW, PITCH, 
                tyre_rot,tyre_turn);
    }
    void reset()
    {
        YAW=PITCH=ROLL=0;
        bike_x=bike_z=BIKE_INIT;
        bike_y=-8.0;
        vel_x=vel_y=vel_z=acc_x=acc_y=acc_z=0;
        tyre_rot=tyre_turn=0;
    }
};

Bike::Bike(){
    YAW=PITCH=ROLL=0;
    bike_x=bike_z=BIKE_INIT;
    bike_y=-8.0;
    vel_x=vel_y=vel_z=acc_x=acc_y=acc_z=0;
    tyre_rot=tyre_turn=0;
}

Bike bike;

void drawValues()
{
    if(toggleValues){
        glColor3f(1,1,1);
        printAt(5,5,"%f",bike.getVel());
    }
}

void setCamera()
{
    switch(toggleCameraView)
    {
        case 0 : {
                     heli=true;
                     Ex= -2*dim*sine(th)*cosine(ph);
                     Ey=  2*dim	*sine(ph);
                     Ez=  2*dim*cosine(th)*cosine(ph);
                     gluLookAt(Ex,Ey,Ez, bike.getpos('x'),bike.getpos('y'),bike.getpos('z'), 0,cosine(ph),0);
                 }
                 break;
        case 1 : {
                     heli=false;
                     fov=45;
                     bike.lookFromHelmet();
                     bike.lookingFromHelmet();
                     bike.getHelmetUp();
                     gluLookAt(Ex,Ey,Ez,
                             lx,ly,lz,
                             ux,uy,uz);
                 }
                 break;
        case 2 : {
                     heli=false;
                     fov=45;
                     bike.lookFromTyre();
                     bike.lookingFromTyre();
                     bike.getHelmetUp();
                     gluLookAt(Ex,Ey,Ez,
                             lx,ly,lz,
                             ux,uy,uz);
                 }
                 break;
        case 3 : {
                     heli=false;
                     fov=45;
                     Ex=bike.getpos('x');
                     Ey=bike.getpos('y')+40;
                     Ez=bike.getpos('z');
                     gluLookAt(Ex,Ey,Ez,
                             Ex,Ey-10,Ez,
                             cosine(bike.getOrient('y')),0,-1*sine(bike.getOrient('y')));
                 }
                 break;
        case 4 : {
                     heli=false;
                     fov=45;
                     bike.lookFromBack();
                     gluLookAt(Ex,Ey,Ez,
                             bike.getpos('x'),bike.getpos('y'),bike.getpos('z'),
                             0.0,1.0,0.0);
                 }
                 break;
        case 5 : {
                     heli=true;
                     lx=Ex+20*cosine(th);ly=Ey;lz=Ez+20*sine(th);
                     gluLookAt(Ex,Ey,Ez,
                             lx,ly,lz,
                             0.0,1.0,0.0);
                 }
    }
}

void changeSunLight()
{
    if(morning)
    {
        dayG+=0.001;
        dayB+=0.001;
        skyG+=0.001;
        skyB+=0.002;
        if(dayG>=0.5)
            morning=!morning;
    }
    else
    {
        dayG-=0.001;
        dayB-=0.001;
        skyG-=0.001;
        skyB-=0.002;
        if(dayG<=0.0)
            morning=!morning;
    }
    if(dayB>0.25)
        glEnable(GL_LIGHT0);
    else
        glDisable(GL_LIGHT0);
}

void drawTerrain()
{
    GLfloat ambientColor[] = {dayR, dayG, dayB, 0.0f};
    glClearColor(skyR,skyG,skyB,1.0);
    changeSunLight();
    if(toggleDay)
        glDisable(GL_LIGHT1);
    else
        glEnable(GL_LIGHT1);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
    GLfloat lightColor0[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat lightColor1[] = {0.5f, 0.5f, 0.0f, 1.0f};
    GLfloat lightPos0[] = {sunX, sunY, 200.0, 0.0f};
    GLfloat lightPos1[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat heading[] = {1.0f,0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    glPushMatrix();
    glTranslatef(bike.getpos('x'),bike.getpos('y'),bike.getpos('z'));
    glRotatef(bike.getOrient('y'),0.0,1.0,0.0);
    glRotatef(bike.getOrient('z'),0.0,0.0,1.0);
    glRotatef(bike.getOrient('x'),1.0,0.0,0.0);
    glLightfv(GL_LIGHT1,GL_POSITION,lightPos1);
    glLightfv(GL_LIGHT1,GL_AMBIENT,lightColor1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,heading);
    glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,0.2);
    glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION,0.01);
    glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,0.001);
    glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,40.0);
    glPopMatrix();


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,textureID1);
    for(int z = 0; z < _terrain->length() - 1; z++) {
        //Makes OpenGL draw a triangle at every three consecutive vertices
        glBegin(GL_TRIANGLE_STRIP);
        for(int x = 0; x < _terrain->width(); x++) {
            Vec3f normal = _terrain->getNormal(x, z);
            glNormal3f(normal[0], normal[1], normal[2]);
            glTexCoord2f(x%2,(x+1)%2);
            glVertex3f(x, _terrain->getHeight(x, z), z);
            normal = _terrain->getNormal(x, z + 1);
            glNormal3f(normal[0], normal[1], normal[2]);
            glTexCoord2f((x+1)%2,x%2);
            glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void drawStream()
{
    glColor3f(0.0,0.3,0.7);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex3f(0.0,-8.0,0.0);
    glVertex3f(0.0,-8.0,-10.0);
    glVertex3f(400.0,-8.0,-10.0);
    glVertex3f(400.0,-8.0,0.0);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(0.0,-8.0,400.0);
    glVertex3f(0.0,-8.0,410.0);
    glVertex3f(400.0,-8.0,410.0);
    glVertex3f(400.0,-8.0,400.0);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(0.0,-8.0,-10.0);
    glVertex3f(-10.0,-8.0,-10.0);
    glVertex3f(-10.0,-8.0,410.0);
    glVertex3f(0.0,-8.0,410.0);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(400.0,-8.0,-10.0);
    glVertex3f(410.0,-8.0,-10.0);
    glVertex3f(410.0,-8.0,410.0);
    glVertex3f(400.0,-8.0,410.0);
    glEnd();
    glPopMatrix();
}

void drawSun()
{
    sunX=300*cosine(sunAngle)+200;
    sunY=300*sine(sunAngle)-6.5;
    glPushMatrix();
    float emission[]={0.1,0.1,0.1,1.0};
    glEnable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT,GL_EMISSION,emission);
    glBindTexture(GL_TEXTURE_2D,textureID2);
    glTranslatef(sunX,sunY,200.0);
    gluQuadricTexture(sun,1);
    gluSphere(sun,20,200,200);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawMoon()
{
    moonX=-300*cosine(sunAngle)+200;
    moonY=-300*sine(sunAngle)-6.5;
    sunAngle+=(9.0/25.0);
    if(sunAngle>=360.0)
    {
        sunAngle=0.0;
        alSourcefv(Sources[ROOSTER],AL_POSITION, SourcesPos[ROOSTER]);
        alSourcePlay(Sources[ROOSTER]);
        wr=true;
    }
    if(sunAngle>180.0&&wr==true)
    {
            alSourcefv(Sources[WOLF],AL_POSITION, SourcesPos[WOLF]);
            alSourcePlay(Sources[WOLF]);
            wr=false;
    }
    glPushMatrix();
    float emission[]={0.1,0.1,0.1,1.0};
    glEnable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT,GL_EMISSION,emission);
    glBindTexture(GL_TEXTURE_2D,textureID3);
    glTranslatef(moonX,moonY,200.0);
    gluQuadricTexture(moon,1);
    gluSphere(moon,15,200,200);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void display2()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();				// Loads the 4X4 Identity matrix.
	printAt(5,70,"GAME OVER");
	glutDisplayFunc(display2);
}


void display3()
{
	if(level==1)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glLoadIdentity();                           // Loads the 4X4 Identity matrix.
		printAt(5,90,"Level %d Finished",6-level);
		printAt(5,70,"You Have Successfully Finished the game");
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
		glEnable(GL_DEPTH_TEST);
		glLoadIdentity();				// Loads the 4X4 Identity matrix.
		printAt(5,90,"Level %d Finished",6-level);
		printAt(5,70,"Press r to go to the Next Level");
		hung=1;
	}
	glutDisplayFunc(display3);
}

void display(){
    double angle;
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();				// Loads the 4X4 Identity matrix.

    //Camera View
    setCamera();
	
    count+=1;
	time_left=start_time-count/time_frac;
	if(time_left==0)
		display2();
	else if(score>=level_req)
		display3();

    drawTerrain();
    drawSun();
    drawMoon();
    bike.draw();

    for(int i=0;i<fossils.size();i++)
    {
        if(fossils[i].isCollected()==false)
        {
            float emission[3][4]={{0.3,0.0,0.0,0.0},{0.0,0.3,0.0,0.0},{0.0,0.0,0.3,0.0}};
            float color[3][3]={{1,0,0},{0,1,0},{0,0,1}};
            //glMaterialfv(GL_FRONT,GL_EMISSION,emission[i%3]);
            fossils[i].draw(color[i%3]);
        }
    }

    drawStream();
    drawValues();
	printAt(5,30,"FOSSILS LEFT: %d",level_req-score);
	printAt(5,50,"DAYS LEFT: %d",time_left);
    glFlush();
    glutSwapBuffers();
}

void project(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov,asp,dim/1000,10*dim);
    glMatrixMode(GL_MODELVIEW);
}

void reshape(int width, int height){
    asp = (height > 0) ? (double)width/height :1;
    glViewport(0,0,width,height);
    project();
}

void resetSettings()
{
    score=keyPressed=0;
    toggleAxis=1;
    toggleValues=1;
    toggleCameraView=0;
    th=ph=mouseX=0;
    toggleDay=true;
    fly=false;
    heli=true;morning=false;
    wr=true;
    sunX=200.0;sunY=33.5;sunAngle=90.0;
    bike.reset();
    fossils.clear();
    initFossils();
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
    else if(key == 'n' || key =='N')
    {
        toggleDay = !toggleDay;
    }
    else if(key=='1')
        toggleCameraView=1;
    else if(key=='0')
        toggleCameraView=0;
    else if(key=='2')
        toggleCameraView=2;
    else if(key=='3')
        toggleCameraView=3;
    else if(key=='4')
        toggleCameraView=4;
    else if(key=='5'){
        fov=10;
        Ex=-50.0;
        Ey=6.0;
        Ez=200.0;
        th=0;
        toggleCameraView=5;
    }
    else if(key =='v'||key=='V')
        toggleValues=1-toggleValues;
    else if(key=='+' && key <179)
        fov++;
    else if(key=='-' &&key>1)
        fov--;
	else if(hung==1 && key=='r')
	{
		resetSettings();
		hung=0;
		count=0;
		level_req=level_req-level;
		level-=1;
		start_time=level;
		time_frac=1000;
		score=0;
		glutDisplayFunc(display);
	}

    project();
    glutPostRedisplay();
}


void funkey(int key,int x,int y){
    int currModifiers = glutGetModifiers();
    if(heli==true)
    {
        if(key==GLUT_KEY_RIGHT)
        {
            Ex=Ex+sine(-1*th);
            Ez=Ez+cosine(th);
        }
        if(key==GLUT_KEY_LEFT)
        {
            Ex=Ex-sine(-1*th);
            Ez=Ez-cosine(th);
        }
        if(key==GLUT_KEY_UP)
        {
            Ex=Ex+cosine(th);
            Ez=Ez+sine(th);
        }
        if(key==GLUT_KEY_DOWN)
        {
            Ex=Ex-cosine(th);
            Ez=Ez-sine(th);
        }
    }

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

void mouseWheel(int button, int state, int x,int y)
{
    if(heli==true)
    {
        if(button ==3)
            fov-=1;
        else if(button ==4)
            fov+=1;
    }
    project();
    glutPostRedisplay();
}

void dragRotate(int x,int y)
{
    if(heli==true){
        if(x<mouseX)
            th-=1;
        else
            th+=1;
        mouseX=x;
    }

}

void update(int value)
{
    if(fly==false){
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
        else if(keys['l']==true)
            bike.turn_right();
        else if(keys['j']==true)
            bike.turn_left();
        else{
            bike.stablize();
        }
    }
    bike.checkFly();
    bike.setpos();

    for(int i=0;i<fossils.size();i++)
    {
        fossils[i].collect(bike.getpos('x'),bike.getpos('z'));
    }

    angle+=1.0;
    if(angle>360.0)
        angle=1.0;

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

void initFossils()
{
    //For now showing only twenty fossils --Phase 1.
    glPushMatrix();
    for(int i=0;i<100;i++)
    {
        Fossil *temp = new Fossil((float)(rand()%400),(float)(rand()%400));
        fossils.push_back(*temp);
    }
    glPopMatrix();
}

void initRendering() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
}

int main(int argc, char ** argv)
{
    printf("Controls--\n\nCamera Views -\nDebug Mode 0\nFront tyre 1\nFront View   2\nTop View  3\nBack View 4\nHelicopter 5\n\nAcceleration w\nBrakes       s\nHandle left  j\nHandle right l\nHeadLight n\n      ");
    for(int i=0;i<256;i++)
        keys[i]=false;
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE|GLUT_DEPTH);
    glutInitWindowSize(500,500);
    glutCreateWindow("Get Inspired...");
    glutReshapeFunc(reshape);
    initRendering();
    init_sound();
    skyR=0.0;skyG=0.5;skyB=1.0;
    dayR=0.5;dayG=0.5;dayB=0.5;

    _terrain = loadTerrain("try1.bmp", 16);
    textureID1=loadBMP_custom("./terrain.bmp");
    textureID2=loadBMP_custom("./sun.bmp");
    textureID3=loadBMP_custom("./moon.bmp");
    initFossils();

    sun=gluNewQuadric();
    moon=gluNewQuadric();
    glutDisplayFunc(display);
    glutKeyboardFunc(winKey);
    glutKeyboardUpFunc(upKeyBoard);
    glutMouseFunc(mouseWheel);
    glutMotionFunc(dragRotate);
    glutSpecialFunc(funkey);

    glutCreateMenu(windowMenu);
    glutAddMenuEntry("Toggle axes [a]",'a');
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutTimerFunc(100,update,0);
    glutMainLoop();
    return 0;
}
