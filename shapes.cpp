#include "includes.h"

void drawBike(float x, float y, float z,
        float len, float high, float wid,
        float x_deg, float y_deg, float z_deg,
        float tyre,float tyre_turn)
{
    y+=0.65;
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(y_deg,0.0,1.0,0.0);
    glRotatef(z_deg,0.0,0.0,1.0);
    glRotatef(x_deg,1.0,0.0,0.0);

    //back
    glColor3f(1.0,1.0,0.0);
    glBegin(GL_QUADS);
    glVertex3f(-1*len/2,high/2,wid/2);
    glVertex3f(-1*len/2,high/2,-1*wid/2);
    glVertex3f(0,-1*high/2,-1*wid/2);
    glVertex3f(0,-1*high/2,wid/2);
    glEnd();

    //front
    glColor3f(0.0,1.0,1.0);
    glBegin(GL_QUADS);
    glVertex3f(1*len/2,high/2,wid/2);
    glVertex3f(1*len/2,high/2,-1*wid/2);
    glVertex3f(0,-1*high/2,-1*wid/2);
    glVertex3f(0,-1*high/2,wid/2);
    glEnd();

    //headlight
    glPushMatrix();
    glColor3f(1.0,1.0,1.0);
    glTranslatef(len/1.5,high/2,0.0);
    glRotatef(tyre_turn, 0.0f, 1.0f, 0.0f);
    glRotatef(-90,0,1.0,0);
    glutSolidCone(0.2,0.2,20,30);
    glPopMatrix();

    //seat
    glColor3f(1.0,0.0,1.0);
    glBegin(GL_QUADS);
    glVertex3f(-1*len/2,high/2,wid/2);
    glVertex3f(-1*len/2,high/2,-1*wid/2);
    glVertex3f(1*len/2,high/2,-1*wid/2);
    glVertex3f(1*len/2,high/2,wid/2);
    glEnd();

    //side left
    glColor3f(0.0,0.5,1.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1*len/2,high/2,wid/2);
    glVertex3f(0,-1*high/2,wid/2);
    glVertex3f(1*len/2,high/2,wid/2);
    glEnd();

    //side right
    glColor3f(0.5,0.5,1.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1*len/2,high/2,-1*wid/2);
    glVertex3f(0,-1*high/2,-1*wid/2);
    glVertex3f(1*len/2,high/2,-1*wid/2);
    glEnd();

    //handle
    glPushMatrix();
    glColor3f(0.0,0.0,0.0);
    GLUquadricObj *quadratic1;
    quadratic1 = gluNewQuadric();
    glTranslatef(BIKE_LEN/2,BIKE_HIGH/2,-0.0);
    glRotatef(tyre_turn, 0.0f, 1.0f, 0.0f);
    gluCylinder(quadratic1,0.035f,0.035f,0.5f,32,32);
    glPopMatrix();
    glPushMatrix();
    glColor3f(0.0,0.0,0.0);
    GLUquadricObj *quadratic2;
    quadratic2 = gluNewQuadric();
    glTranslatef(BIKE_LEN/2,BIKE_HIGH/2,-0.0);
    glRotatef(tyre_turn+180, 0.0f, 1.0f, 0.0f);
    gluCylinder(quadratic2,0.035f,0.035f,0.5f,32,32);
    glPopMatrix();

    glColor3f(1.0,1.0,1.0);
    //Front Tyre
    glPushMatrix();
    glTranslatef( len/2 , -high/2 , 0.0 );
    glRotatef(tyre_turn,0.0,1.0,0.0);
    glRotatef(tyre,0.0,0.0,1.0);
    glutWireTorus(0.1,0.3,10,20);
    glPopMatrix();

    //Rear Tyre
    glPushMatrix();
    glTranslatef( -len/2 ,-high/2 ,0.0 );
    glRotatef(tyre,0.0,0.0,1.0);
    glutWireTorus(RIM_RADIUS,TYRE_RADIUS,10,20);
    glPopMatrix();


    glPopMatrix();

}

void drawFossil(float x,float y,float z,float angle,float color[3])
{	
    glPushMatrix();
    glColor3f(color[0],color[1],color[2]);
    glTranslatef(x,y,z);
    glRotatef(angle,0.0,1.0,0.0);
    glutSolidOctahedron();
    glPopMatrix();
}
