#ifndef __CUBEOBJECT_H__
#define __CUBEOBJECT_H__
#include "HierarchicalObject.h"
#include <GL/glew.h>
#include <GL/glut.h>

class CubeObject : public HierarchicalObject {
private:
	int computeSize(GLfloat *buf);
	void addSubBuffer(GLfloat *data, int dataSize, int attribLoc, int &offset, int count = 4);
	void buildObject(GLfloat *vertices, int vertexSize, int vertexAttribLoc,
		GLfloat *normals, int normalsSize, int normalAttribLoc,
		GLfloat *colors, int colorsSize, int colorsAttribLoc,
		GLuint & vao, GLuint &bao);
	vmath::vec3 postion;


protected:
	GLuint programID, vaobjectID, baobjectID;
	int triangleCount;
public:
	CubeObject(GLuint programID);
	void draw();
	void draw(vmath::mat4 transform);
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();


};

#endif