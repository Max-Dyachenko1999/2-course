#include <iostream>
#include <GL/glut.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

using namespace std;

int angle = 45; 
double s;
bool flag = false; 


void init()
{

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);

	float lightAmb[4] = { 1.0, 0.0, 0.0, 1 };	// background color is Blue
	float lightDif[4] = { 0.0, 0.0, 1.0, 1 };	// reflected color is Red
	float lightPos[4] = { 1.0, 3.0, 3.0, 1 };	// Position in space
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);						// turn on the light source
	glEnable(GL_LIGHTING);						// enable the light
}

void display() 
{
	GLUquadricObj *quadObj;
	quadObj = gluNewQuadric();
	flag = true;									// we begin to draw
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 

	glPushMatrix();									// begin our drawing
	glRotatef(angle, 2, 1, 3);						// rotating matrix

	glTranslated(-1, 1, 0);							// we multiplie the current matrix by a translation matrix
	glRotated(90, 1, 0, 0);							
	gluQuadricDrawStyle(quadObj, GLU_FILL);			// Quadrics are rendered with polygon primitives. 
													// The polygons are drawn in a counterclockwise fashion with respect to their normals
	gluCylinder(quadObj, 0.3, 0.3, 2, 100, 100);	// we draw cylinder - the foundation of our dumbbell;

	glTranslated(0, 0, 0);  

	glutSolidSphere(0.6, 100, 100);					// drawing first sphere;

	glTranslated(0, 0, 2);  

	glutSolidSphere(0.6, 100, 100);					// drawing second sphere;
	glPopMatrix();									// end of drawing
	glutSwapBuffers();    
	flag = false;       
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);		// reconstruct the image output in the window with the other dimensions.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();								// read current matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75, GLfloat(1.33), 1, 20);		// set up a perspective projection matrix
	gluLookAt(0, 0, 5, 0, 0, 1, 0, 1, 0);			// The first 3 pts - position coordinates nalyudatelya eyes,
													// Second 3 pts - point coordinates are placed in the center of the screen,
													// Third 3 pts - direction vector defining the rotation stage.
}

void mytime(int value) 
{
	angle = (angle > 358) ? 0 : angle + 2;		// Calculation of the angle
	if (!flag) 
	{
		glutPostRedisplay();
	}
	else {
		std::cerr << "Your system is VERY SLOW!" << std::endl;
	}
	glutTimerFunc(60/s, mytime, 0);				// The following call
}


int main(int argc, char** argv)
{
	cout << "Enter a speed : " ;
	cin >> s;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	int glWin = glutCreateWindow("My dumbbell");
	init();
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape); 
	glutTimerFunc(60/s, mytime, 0);
	glutMainLoop(); 
	return 0;
}