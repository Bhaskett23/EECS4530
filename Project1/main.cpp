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

const int NBR_LINES = 3601;
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
*
* Revisions:
*    September 22, 2016 -- Updated readFileData to calculate the extent of the
*                          data in the x, y, and z directions as well.
*/
float *readFileData(string filename){
	/* extent holds the minimum x, y, z and the maximum x, y, z */
	float extent[6];
	float *triangles = NULL;
	float *lastLine = NULL;
	float *currentLine = NULL;
	ifstream inFile;
	int linesRead, currentTriangle;
	/*
	* open the file for input.
	*/
	extent[0] = extent[2] = 0.0;
	extent[3] = extent[5] = 3600.0 * 30.0;
	extent[1] = 65535.0;
	extent[4] = -65536.0;
	inFile.open(filename.c_str());
	if (inFile.fail()) {
		cerr << "Unable to open file \"" << filename << "\" for input " << endl;
		return triangles;
	}
	/*
	* Create an array that is big enough to hold the triangles that we are going to create.
	*
	* Have used lots of defines here so that I can change between file types more
	* easily.
	*/
	triangles = new float[(NBR_LINES - 1)* (NBR_LINES - 1) * NBR_TRIANGLES_PER_QUAD *
		NBR_POINTS_PER_TRIANGLE * NBR_FLOATS_PER_POINT];
	/*
	* Hold two lines at a time.  the lastLine and the currentLine.  The
	* application will swap between them as necessary.
	*/
	lastLine = new float[NBR_LINES];
	currentLine = new float[NBR_LINES];
	/*
	* Check for a memory allocation error -- it can happen.
	*/
	if (triangles == NULL || lastLine == NULL || currentLine == NULL) {
		if (triangles != NULL) {
			delete[] triangles;
			triangles = NULL;
		}
		if (lastLine != NULL) {
			delete[] lastLine;
		}
		if (currentLine != NULL) {
			delete[] currentLine;
		}
		return triangles;
	}
	/*
	* Read in the lines from the file and connect triangles together...
	*/
	readLineFromFile(inFile, lastLine);
	currentTriangle = 0;
	linesRead = 1;
	int thirtyLinesRead = 30, thirtyLinesRead_minus_1 = 0;
	for (linesRead = 1; linesRead < NBR_LINES - 1; linesRead++) {
		readLineFromFile(inFile, currentLine);
		/*
		*Generate Triangles  -- two triangles per quad.
		*/
		int thirtyColumn = 0, thirtyColumn_plus_1 = 30;
		for (int column = 0; column < NBR_LINES; column++) {
			triangles[currentTriangle] = triangles[currentTriangle + 3] = triangles[currentTriangle + 15] = float(thirtyColumn);
			triangles[currentTriangle + 6] = triangles[currentTriangle + 9] = triangles[currentTriangle + 12] = float(thirtyColumn_plus_1);
			triangles[currentTriangle + 1] = lastLine[column];
			triangles[currentTriangle + 4] = triangles[currentTriangle + 16] = currentLine[column];
			triangles[currentTriangle + 7] = triangles[currentTriangle + 10] = lastLine[column + 1];
			triangles[currentTriangle + 13] = currentLine[column + 1];
			triangles[currentTriangle + 2] = triangles[currentTriangle + 8] = triangles[currentTriangle + 11] = float(thirtyLinesRead_minus_1);
			triangles[currentTriangle + 5] = triangles[currentTriangle + 14] = triangles[currentTriangle + 17] = float(thirtyLinesRead);
			currentTriangle += 18;
			extent[1] = min(extent[1], lastLine[column]);
			extent[4] = max(extent[4], lastLine[column]);
			thirtyColumn = thirtyColumn_plus_1;
			thirtyColumn_plus_1 += 30;
		}


		/*
		* Swap arrays -- do not want to reallocate them.
		*/
		float *temp;
		temp = lastLine;
		lastLine = currentLine;
		currentLine = temp;
		thirtyLinesRead_minus_1 = thirtyLinesRead;
		thirtyLinesRead += 30;
	}
	delete[] lastLine;
	delete[] currentLine;
	return triangles;
}

/*
* Initialize still needs to load shaders and build program.
*/
void initialize(float *surfaceData, int numberOfPoints) {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	GLuint vao, buffer;
	GLuint transformationLocation;

	static const GLchar *vertexShader = "#version 330 core"
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
	static const GLchar *fragmentShader = "#version 330 core"
		" "
		"in vec4 Color;"
		""
		"void main() {"
		"   frag_Color = Color; "
		"}"
		"";
	GLuint fragShaderID, vertShaderID, programID;
	GLint compiledOK;

	programID = glCreateProgram();

	vertShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLint vertexShaderLength = sizeof(vertexShader);
	GLint fragmentShaderLength = sizeof(fragmentShader);
	glShaderSource(vertShaderID, 1, (const GLchar * const *)vertexShader, (const GLint *)&vertexShaderLength);
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
	glShaderSource(fragShaderID, 1, (const GLchar * const *)fragmentShader, &fragmentShaderLength);
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

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3600 * 3600 * 2);
	glutSwapBuffers();  // instead of glflush() in double buffering.

}

void reshape(int x, int y) {
}


int main(int argCount, char * argValues[]) {
	float *fileData;

	fileData = readFileData("N36W112.hgt");
	if (fileData == NULL) {
		cerr << "Unable to read file or allocate memory -- exitting" << endl;
		exit(-1);
	}
	else {
		glutInit(&argCount, argValues);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);  // yes -- you want all three
		glutInitContextVersion(3, 1);
		glutInitContextProfile(GLUT_CORE_PROFILE);
		glewInit();

		glutInitWindowSize(600, 600);
		glutInitWindowPosition(20, 20);
		glutCreateWindow("Main Window");
		initialize(fileData, 3600 * 3600 * 2 * 3);
		glutDisplayFunc(display);
		glutMainLoop();
		delete[] fileData;
	}

	return EXIT_SUCCESS;
}