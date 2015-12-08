#include "HierarchicalObject.h"
#include "SimpleObject.h"
#include "CubeObject.h"
#include <cstdlib>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "LoadShaders.h"

using namespace std;

list<CubeObject *> toplevelObjects;

/*
* Still need init to load in programs and hopefully set up the basics of the
* objects in the program.
*/
void init() 
{
	/*
	* establish objects with programs.
	*/
	/*
	*  Use the Books code to load in the shaders.
	*/
	ShaderInfo shaders[] = 
	{
		{ GL_VERTEX_SHADER, "vshader31.glsl" },
		{ GL_FRAGMENT_SHADER, "fshader31.glsl" },
		{ GL_NONE, NULL }
	};
	GLuint programID = LoadShaders(shaders);
	glUseProgram(programID);
	toplevelObjects.push_back(new CubeObject(programID));
}

void display() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto obj : toplevelObjects) {
		obj->draw();
	}
	glFlush();
	glutSwapBuffers();
}

void Moving(int key, int x, int y)
{
	for (CubeObject* obj : toplevelObjects) 
	{
		switch (key)
		{
		case GLUT_KEY_UP:
			obj->moveUp();
			break;
		case GLUT_KEY_DOWN:
			obj->moveDown();
			break;
		case GLUT_KEY_LEFT:
			obj->moveLeft();
			break;
		case GLUT_KEY_RIGHT:
			obj->moveRight();
			break;
		}	

		obj->draw();
	}
	glutPostRedisplay();
}

int main(int argCount, char * argValues[]) 
{
	glutInit(&argCount, argValues);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Test Window");
	glutInitContextVersion(3, 1);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glewInit();
	init();
	glutDisplayFunc(display);
	glutSpecialFunc(Moving);
	glutMainLoop();
	return EXIT_SUCCESS;
}