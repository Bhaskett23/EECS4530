#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>
using namespace std;


/*
* This program is to read in elevation data from a NASA 1 degree file.
*
* Author :  Jerry Heuring
* Date:  November 7, 2013
*
* Warning:  Code is not finished.  It will read in the file.  You will
*           need to alter it or just tear out the stuff to read the file
*           if you want to use it.
*
*/

const int NBR_LINES = 1201;
const int NBR_TRIANGLES_PER_QUAD = 2;
const int NBR_FLOATS_PER_POINT = 3;
const int NBR_POINTS_PER_TRIANGLE = 3;

static float *fileData;

void readLineFromFile(istream& inFile, float *line){
	unsigned char inputline[NBR_LINES * 2];

	inFile.read((char *)inputline, NBR_LINES * 2);
	for (int i = 0, j = 0; j < NBR_LINES; i += 2, j++) {
		line[j] = float((unsigned int)((inputline[i] << 8) | inputline[i + 1]));
	}
	return;

}


/*
* Initialize still needs to load shaders and build program.
*/
void initialize(float *surfaceData, int numberOfPoints) {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	GLuint vao, buffer;
	GLuint transformationLocation;
	//glLineWidth(1.0);
	static const GLchar *vertexShader = "#version 330 core \n"
		""
		"uniform mat4 transformation;"
		"in vec4 Position;"
		""
		"out vec4 Color;"
		""
		"void main() {"
		"   Color = vec4(1.0, 1.0, 1.0, 1.0);"
		"	gl_Position = (transformation * Position)/Position.w;"
		"}"
		"";
	static const GLchar *fragmentShader = "#version 330 core \n"
		" "
		"in vec4 Color;"
		""
		"void main() {"
		"   gl_FragColor = Color; "
		"}"
		"";
	GLuint fragShaderID, vertShaderID, programID;
	GLint compiledOK;

	programID = glCreateProgram();

	vertShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLint vertexShaderLength = strlen(vertexShader);
	GLint fragmentShaderLength = strlen(fragmentShader);
	glShaderSource(vertShaderID, 1, (const GLchar * const *)&vertexShader, (const GLint *)&vertexShaderLength);
	glCompileShader(vertShaderID);
	glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &compiledOK);
	if (!compiledOK) {
		GLsizei length;
		glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &length);
		GLchar * log = new GLchar[length + 1];
		glGetShaderInfoLog(vertShaderID, length, &length, log);
		std::cerr << "Vertex Shader Compilation Failed: " << log << std::endl;
		delete[] log;
	}
	else {
		glAttachShader(programID, vertShaderID);
	}
	fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShaderID, 1, (const GLchar * const *)&fragmentShader, &fragmentShaderLength);
	glCompileShader(fragShaderID);
	glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &compiledOK);
	if (!compiledOK) {
		GLsizei length;
		glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &length);
		GLchar * log = new GLchar[length + 1];
		glGetShaderInfoLog(fragShaderID, length, &length, log);
		std::cerr << "Fragment Shader Compilation Failed: " << log << std::endl;
		delete[] log;
	}
	else {
		glAttachShader(programID, fragShaderID);
	}
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, numberOfPoints, surfaceData, GL_STATIC_DRAW);

}

void display() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 1200 * 1200 * 2);
	glutSwapBuffers();  // instead of glflush() in double buffering.
	
}

void setup(int argCount, char * argValues[])
{
	glutInit(&argCount, argValues);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);  // yes -- you want all three
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("Main Window");
	glutInitContextVersion(3, 1);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glewInit();
}

int main(int argCount, char * argValues[]) 
{
	setup(argCount, argValues);
	GLfloat triangles[] = { -10.0f, 0.0f, 10.0f, 0.0f, 0.0f, 10.0f, -10.0f, 0.0f, 0.0f,
		-10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 0.0f, 0.0f,
		10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 0.0f, 10.0f,
		-10.0f, 0.0f, -10.0f, 0.0f, 0.0f, -10.0f, -10.0f, 0.0f, 0.0f,
		-10.0f, 0.0f, 0.0f, 0.0f, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -10.0f, 10.0f, 0.0f, 0.0f,
		10.0f, 0.0f, 0.0f, 0.0f, 0.0f, -10.0f, 10.0f, 0.0f, -10.0f };

	initialize(fileData, 1200 * 1200 * 2 * 3);

	glutDisplayFunc(display);
	glutMainLoop();
	delete[] fileData;

	return EXIT_SUCCESS;
}