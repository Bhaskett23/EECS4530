#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <strstream>
using namespace std;

float *readOBJFile (string filename, int &nbrTriangles) {
	/*
	 * I'm going to use vectors to hold the items I read in.
	 * this is not terribly efficient but I'm striving for simplicity here
	 * rather than efficiency.
	 */
	vector<float *> vertices;
	vector<int *> triangles;
	string currentLine, lineType;
	istrstream *lineRdr;
	float *vertex;
	int *triangle;

	nbrTriangles = 0;
	/*
	 * Open the file -- routine returns null if it fails
	 */
	ifstream infile;
	infile.open(filename.c_str());
	if (infile.fail()) {
		cerr << "Failed to open \"" << filename << "\" for input\n";
		return NULL;
	}
	/*
	 * Put an dummy vertex in the zero spot in the vector.  Then I don't
	 * need to adjust the indices in the OBJ file.
	 */
	vertex = new float[4];
	vertex[0] = vertex[1] = vertex[2] = 0.0f;
	vertex[3] = 1.0f;
	vertices.push_back(vertex); // put an empty entry in 0 so that we don't need to adjust
	
	while (!infile.fail() && !infile.eof()) { // read data in...
		getline(infile, currentLine);
		if (infile.fail() || infile.eof()) break;
		/*
                 * read the data using an istrstream.  Only vertices and 
		 * faces are used at this point.
		 */
		lineRdr = new istrstream(currentLine.c_str());
		*lineRdr >> lineType;
		if (lineType == "v") { // process the vertex
			vertex = new float[4];
			*lineRdr >> vertex[0] >> vertex[1] >> vertex[2];
			vertex[3] = 1.0f;
			vertices.push_back(vertex);
		} else if (lineType == "f" ) { // process the face
			triangle = new int [3];
			*lineRdr >> triangle[0] >> triangle[1] >> triangle[2];
			triangles.push_back(triangle);
		}
	}
	/*
	 * Build an array to hold the coordinates.  Remember everything
	 * is a triangle.
	 */
	float * triangleVertices = new float [triangles.size()*3 * 4];
	nbrTriangles = triangles.size();
	for (int i = 0; i < triangles.size(); i++) {
		triangle = triangles[i];
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 4; k++) {
				triangleVertices[i * 12 + j * 4 + k] = (vertices[triangle[j]])[k];
			}
		}
		delete [] triangle;
	}
	/*
	 * Clean up....
	 */
	for (int i = 0; i < vertices.size(); i++) {
		delete [] vertices[i];
	}
	return triangleVertices;
}
