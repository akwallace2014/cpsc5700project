#include "SceneObject.h"
#include "GLXtras.h"
#include "Mesh.h"

SceneObject::SceneObject(std::string objectName, std::string meshFilePath) {
	name = objectName;
	meshFile = meshFilePath;
}

void SceneObject::loadMeshFile() {
	if (!ReadAsciiObj(meshFile.c_str(), points, triangles, &normals, &uvs)) {
		printf("Failed to read .obj file for %s (enter any key to continue)\n", name.c_str());
		getchar();
	}

	printf("%s: %i vertices, %i triangles, %i normals, %i uvs\n", name.c_str(), points.size(), triangles.size(), normals.size(), uvs.size());
	Normalize(points, 0.8f);
}

void SceneObject::initVertexBuffer() {
	glGenVertexArrays(1, &vertexArray);
	glGenBuffers(1, &vertexBuffer);
	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	int sizePoints = points.size() * sizeof(vec3), sizeNormals = normals.size() * sizeof(vec3), sizeUvs = uvs.size() * sizeof(vec2);
	glBufferData(GL_ARRAY_BUFFER, sizePoints + sizeNormals + sizeUvs, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizePoints, points.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizePoints, sizeNormals, normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizePoints + sizeNormals, sizeUvs, uvs.data());
}

size_t SceneObject::pointsSize() {
	return points.size();
}

size_t SceneObject::trianglesSize() {
	return triangles.size();
}

size_t SceneObject::normalsSize() {
	return normals.size();
}

size_t SceneObject::uvsSize() {
	return uvs.size();
}

const void* SceneObject::trianglesStart() {
	return &triangles[0];
}

void SceneObject::shutdown() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vertexBuffer);
}

GLuint SceneObject::getVertexArray() {
	return vertexArray;
}