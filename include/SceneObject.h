#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <glad.h>
#include "VecMat.h"
#include <string>
#include <vector>

class SceneObject {
public:
	SceneObject(std::string objectName, std::string meshFile);
	void loadMeshFile();
	void initVertexBuffer();
	size_t pointsSize();
	size_t trianglesSize();
	size_t normalsSize();
	size_t uvsSize();
	const void* trianglesStart();
	GLuint getVertexArray();
	void shutdown();

protected:
	std::string name;
	GLuint vertexBuffer, vertexArray;
	std::string meshFile;
	std::vector<vec3> points;           // 3D mesh vertices
	std::vector<int3> triangles;        // triplets of vertex indices
	std::vector<vec3> normals;          // surface normals
	std::vector<vec2> uvs;              // uv coordinates
};

#endif