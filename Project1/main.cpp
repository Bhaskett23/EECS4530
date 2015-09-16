#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "vgl.h"
#include "vmath.h"
#include "LoadShaders.h"
using namespace std;

float *readOBJFile(string filename, int &nbrTriangles);

/*
* This is an example of drawing an Icosahedron that we worked on in class.
* The icosahedron is read from data in the application and not from the
* file (it was copied from the file).  It is really just a revision of
* the element draw example we worked on earlier.
*
* Author:  Jerry Heuring
* Date:    October 6, 2014
*
* Bugs:
*
*
* Revisions:
*	10/7/2014 -- added a command line option to choose between using
*                glDrawElements and glDrawArray.
*
*
*/
#define BUFFER_OFFSET(x)  ((const void*) (x))

/*
* Arrays to store the indices/names of the Vertex Array Objects and
* Buffers.  Rather than using the books enum approach I've just
* gone out and made a bunch of them and will use them as needed.
*
* Not the best choice I'm sure.
*/

GLuint vertexBuffers[10], arrayBuffers[10], elementBuffers[10];

/*
* Global variables
*   The location for the transformation and the current rotation
*   angle are set up as globals since multiple methods need to
*   access them.
*/

GLuint transformLocation, rotationLocation;
float  rotationAngle;
bool   elements;
int    nbrTriangles;

/*
* Timer routine -- when called it increments the angle by 1 degree,
* tells Glut to redraw the screen, and sets a timer for 1/30 of a
* second later.
*/
void timer(int value) {
	rotationAngle += 1.0f;
	if (rotationAngle > 360) {
		rotationAngle = rotationAngle - 360;
	}
	glutPostRedisplay();
	glutTimerFunc(1000 / 30, timer, 1);
}

/*
* Init will set up our array objects, buffers, and shaders.
* The shaders are hard coded right now.  I'm really tempted to
* build something into the main program to handle a command line
* argument for the shaders and pass the result into here to make
* it easy to change shaders.
*/
void init() {

	/*
	* our data is static so that it doesn't disappear with the stack.  Also, the
	* data for the Icosahedron is being stored in indexed polygon form.  This saves
	* me from retyping it.  I'm going to update it to hold it in both vertex
	* form as well but then we will need to select which form to use.
	*/

	static float icosahedronVertices[] = {
		0.0f, 0.0f, 0.0f, 1.0f,
		0.000000f, -1.000000f, 0.000000f, 1.0f,
		0.723600f, -0.447215f, 0.525720f, 1.0f,
		-0.276385f, -0.447215f, 0.850640f, 1.0f,
		-0.894425f, -0.447215f, 0.000000f, 1.0f,
		-0.276385f, -0.447215f, -0.850640f, 1.0f,
		0.723600f, -0.447215f, -0.525720f, 1.0f,
		0.276385f, 0.447215f, 0.850640f, 1.0f,
		-0.723600f, 0.447215f, 0.525720f, 1.0f,
		-0.723600f, 0.447215f, -0.525720f, 1.0f,
		0.276385f, 0.447215f, -0.850640f, 1.0f,
		0.894425f, 0.447215f, 0.000000f, 1.0f,
		0.000000f, 1.000000f, 0.000000f, 1.0f
	};


	static float icosahedronColors[] = {
		0.0, 0.0, 0.0, 1.0, // Black
		1.0, 0.0, 0.0, 1.0, // Red
		1.0, 1.0, 0.0, 1.0, // Yellow
		0.0, 1.0, 0.0, 1.0, // Green
		0.0, 0.0, 1.0, 1.0, // Blue
		1.0, 0.0, 1.0, 1.0, // Magenta
		1.0, 1.0, 1.0, 1.0, // White
		0.0, 1.0, 1.0, 1.0,  // Cyan
		0.0, 0.0, 0.0, 1.0, // Black
		1.0, 0.0, 0.0, 1.0, // Red
		1.0, 1.0, 0.0, 1.0, // Yellow
		0.0, 1.0, 0.0, 1.0, // Green
		0.0, 0.0, 1.0, 1.0, // Blue
	};

	static GLuint icosahedronTriangles[] = {
		1, 2, 3,
		2, 1, 6,
		1, 3, 4,
		1, 4, 5,
		1, 5, 6,
		2, 6, 11,
		3, 2, 7,
		4, 3, 8,
		5, 4, 9,
		6, 5, 10,
		2, 11, 7,
		3, 7, 8,
		4, 8, 9,
		5, 9, 10,
		6, 10, 11,
		7, 11, 12,
		8, 7, 12,
		9, 8, 12,
		10, 9, 12,
		11, 10, 12
	};

	float *fileTriangles;
	/*
	* Building an array of only vertices that will work with the OpenGL
	* call for glDrawArray.  In this case we are reading it in using
	* the ReadOBJ routine.
	*/
	fileTriangles = readOBJFile("icodos.obj", nbrTriangles);
	/*
	* I'm using wide lines so that they are easier to see on the screen.
	*/
	glLineWidth(4.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/*
	* Create a vertex array for the buffers,
	* generate an array buffer to copy the
	* vertex data and the color data into.
	*/

	glGenVertexArrays(1, vertexBuffers);  // generates ID for VAO
	glBindVertexArray(vertexBuffers[0]);  // Binds it for use
	glGenBuffers(1, arrayBuffers);        // Generates ID vor Buffer
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[0]); // Binds it for use
	glBufferData(GL_ARRAY_BUFFER,
		nbrTriangles * 3 * 4 * sizeof(float),
		fileTriangles,
		GL_STATIC_DRAW);  // Copies data from fileTriangles into buffer.

	/*
	*  Use the Books code to load in the shaders.
	*/
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "pass.vert" },
		{ GL_FRAGMENT_SHADER, "pass.frag" },
		{ GL_NONE, NULL }
	};
	GLuint program = LoadShaders(shaders);
	glUseProgram(program);
	/*
	* Set up variables into the shader programs (Note:  We need the
	* shaders loaded and built into a program BEFORE we do this)
	*/
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE,
		0, BUFFER_OFFSET(0));

	transformLocation = glGetUniformLocation(program, "transform");

	rotationLocation = glGetUniformLocation(program, "rotation");
	/*
	* Enable depth testing to remove hidden surfaces.
	*/
	glEnable(GL_DEPTH_TEST);
}

/*
* display sets the current transformation and draws the elements
* in the vertex array.  It does clear the color and depth information
* before it does this.
*/

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vmath::mat4 currentMatrix = vmath::scale(0.12f) * vmath::rotate(rotationAngle, 0.0f, 1.0f, 0.0f);
	glUniformMatrix4fv(transformLocation, 1, GL_TRUE, currentMatrix);
	glBindVertexArray(vertexBuffers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[0]);
	glDrawArrays(GL_TRIANGLES, 0, nbrTriangles * 3);
	glFlush();
}

/*
* This is the basic main program.  It initializes things and
* sets up the window.
*/
int main(int argCount, char *argValues[]) {
	glutInit(&argCount, argValues);

	// Check for a flag to determine if we are using glDrawElements or 
	// glDrawArray.  I'm looking for either --elements or --array.
	// The default is the elements.
	elements = true;
	for (int argNbr = 1; argNbr < argCount; argNbr++) {
		if (argValues[argNbr] == "--elements") {
			cout << "Drawing using glDrawElements" << endl;
			elements = true;
		}
		else if (argValues[argNbr] == "--array") {
			elements = false;
		}
	}

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Icosahedron Sample");

	glutInitContextVersion(3, 1);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glewInit();
	init();
	glutDisplayFunc(display);
	glutTimerFunc(1000 / 30, timer, 1);
	glutMainLoop();
	return 0;
}
