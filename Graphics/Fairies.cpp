/**
* Fairies.cpp - Flying fairy lighting animation
* Alisa Wallace, CPSC 5700 FQ21 Final Project
* Seattle University
*/

#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include "GLXtras.h"
#include <time.h>
#include <vector>
#include "Mesh.h"
#include "Camera.h"
#include "Misc.h"
#include <math.h>
#include "MovingObject.h"

int winW = 750, winH = 750;
Camera camera((float)winW / winH, vec3(0, 0, 0), vec3(0, 0, -5));

const int NUM_FAIRIES = 5;

struct SceneObject {
    GLuint vertexBuffer, shaderProgram;
    int textureID;
    std::vector<vec3> points;           // 3D mesh vertices
    std::vector<int3> triangles;        // triplets of vertex indices
    std::vector<vec3> normals;          // surface normals
    std::vector<vec2> uvs;              // uv coordinates
};

struct SceneObject fairy;
struct SceneObject rock;

//vector<vec2> targets = { {0.0000f, 0.0000f}, {1.0000f, 1.0000f}, {0.0000f, 1.0000f}, {-1.0000f, -1.0000f}, {1.0000f, 0.0000f}, {0.0000f, -1.0000f}, {-1.0000f, 1.0000f}};
//
//vector<int> turningPoints = {1, 0, -1, 1, 0, -1, 0 }; 

vector<vec2> targets = { {0.0f, 0.0f},
    {1.0f, 0.2f},
    {0.8f, 0.4f},
    {-0.3f, 0.9f},
    {-1.0f, -0.6f},
    {-0.8f, -0.9f},
    {0.5f, -0.2f},
    {0.8f, -0.4f},
    {1.0f, -0.6f},
    {0.3f, -1.0f},
    {-0.4f, -0.3f},
    {-0.3f, -0.1f} };

vector<int> turningPoints = { 0, -1, 0, 0, 1, 0, 0, -1, 0, 0, 1, 0 };

std::vector<MovingObject*> movingFairies;
vec3 fairyPositions[5];

void initializeFairies() {
    // disperse the fairies' starting locations across the flight path
    for (int i = 0, j = 0; j < NUM_FAIRIES; ++j, i += 2) {
        MovingObject* fairy = new MovingObject(targets, turningPoints, i);
        movingFairies.push_back(fairy);
        fairyPositions[i] = fairy->position();
    }
}

void cleanup() {
    for (int i = 0; i < NUM_FAIRIES; i++) {
        delete movingFairies.at(i);
    }
}

void loadMesh(struct SceneObject &obj, std::string filePath) {
    if (!ReadAsciiObj(filePath.c_str(), obj.points, obj.triangles, &(obj.normals), &(obj.uvs))) {
        printf("Failed to read .obj file for fairy (enter any key to continue)\n");
        getchar();
    }
    printf("Fairy: %i vertices, %i triangles, %i normals, %i uvs\n", obj.points.size(), obj.triangles.size(), obj.normals.size(), obj.uvs.size());
    Normalize(obj.points, 0.8f);

    glGenBuffers(1, &obj.vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, obj.vertexBuffer);
    int sizePoints = obj.points.size() * sizeof(vec3), sizeNormals = obj.normals.size() * sizeof(vec3), sizeUvs = obj.uvs.size() * sizeof(vec2);
    glBufferData(GL_ARRAY_BUFFER, sizePoints + sizeNormals + sizeUvs, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizePoints, obj.points.data());
    glBufferSubData(GL_ARRAY_BUFFER, sizePoints, sizeNormals, obj.normals.data());
    glBufferSubData(GL_ARRAY_BUFFER, sizePoints + sizeNormals, sizeUvs, obj.uvs.data());
}

void loadTexture(struct SceneObject& obj, std::string filePath) {
    obj.textureID = LoadTexture(filePath.c_str(), 0);
}

void updateFairyPositions() {
    for (int i = 0; i < NUM_FAIRIES; i++) {
        fairyPositions[i] = movingFairies.at(i)->position();
    }
}

void drawFairy(MovingObject* movingFairy) {
    glBindBuffer(GL_ARRAY_BUFFER, fairy.vertexBuffer);
    VertexAttribPointer(fairy.shaderProgram, "point", 3, 0, (void*)0);
    VertexAttribPointer(fairy.shaderProgram, "normal", 3, 0, (void*)(fairy.points.size() * sizeof(vec3)));

    glUseProgram(fairy.shaderProgram);
    movingFairy->adjustMovement();
    SetUniform(fairy.shaderProgram, "modelview", movingFairy->translationMatrix() * camera.modelview  *  movingFairy->rotationMatrix()* movingFairy->scaleMatrix());
    SetUniform(fairy.shaderProgram, "persp", camera.persp);

    // render triangles
    glDrawElements(GL_TRIANGLES, 3 * fairy.triangles.size(), GL_UNSIGNED_INT, &fairy.triangles[0]);
    glFlush();
}

void drawRock() {
    glBindBuffer(GL_ARRAY_BUFFER, rock.vertexBuffer);
    VertexAttribPointer(rock.shaderProgram, "point", 3, 0, (void*)0);
    VertexAttribPointer(rock.shaderProgram, "normal", 3, 0, (void*)(rock.points.size() * sizeof(vec3)));
    int pSize = rock.points.size() * sizeof(vec3), nSize = rock.normals.size() * sizeof(vec3);
    VertexAttribPointer(rock.shaderProgram, "uv", 2, 0, (void*)(pSize + nSize));

    glUseProgram(rock.shaderProgram);
    mat4 translation = Translate(0.0f, -0.6f, -0.6f);
    mat4 scale = Scale(0.75, 0.75, 0.75);
    mat4 rotation = RotateX(-15.0f);
    SetUniform(rock.shaderProgram, "textureImage", 0);
    SetUniform(rock.shaderProgram, "modelview", translation * camera.modelview * rotation * scale);
    SetUniform(rock.shaderProgram, "persp", camera.persp);
    SetUniform(rock.shaderProgram, "numLights", NUM_FAIRIES);
    updateFairyPositions();
    SetUniform(rock.shaderProgram, "lights[0]", fairyPositions[0]);
    SetUniform(rock.shaderProgram, "lights[1]", fairyPositions[1]);
    SetUniform(rock.shaderProgram, "lights[2]", fairyPositions[2]);
    SetUniform(rock.shaderProgram, "lights[3]", fairyPositions[3]);
    SetUniform(rock.shaderProgram, "lights[4]", fairyPositions[4]);
    SetUniform(rock.shaderProgram, "viewPosition", camera.GetTran());

    // render triangles
    glBindTexture(GL_TEXTURE_2D, rock.textureID);
    glActiveTexture(GL_TEXTURE0 + 0);
    glDrawElements(GL_TRIANGLES, 3 * rock.triangles.size(), GL_UNSIGNED_INT, &rock.triangles[0]);
    glFlush();
}

void Resize(GLFWwindow* window, int width, int height) {
    camera.Resize(winW = width, winH = height);
    glViewport(0, 0, winW, winH);
}

void Display() {
    glClearColor(0.0f, 0.0f, 0.02f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (int i = 0; i < NUM_FAIRIES; i++) {
        drawFairy(movingFairies.at(i));
    }
    
    drawRock();
}

void initShader(struct SceneObject &obj, std::string vertexShaderPath, std::string fragShaderPath) { 
    obj.shaderProgram = LinkProgramViaFile(vertexShaderPath.c_str(), fragShaderPath.c_str());
    if (!obj.shaderProgram)
        printf("can't init shader program\n");
}

// application

void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void ErrorGFLW(int id, const char* reason) {
    printf("GFLW error %i: %s\n", id, reason);
}

int main() {

    glfwSetErrorCallback(ErrorGFLW);
    if (!glfwInit())
        return 1;
    
    GLFWwindow* w = glfwCreateWindow(winW, winH, "Fairies", NULL, NULL);
    if (!w) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(w);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwSetWindowSizeCallback(w, Resize);
    glfwSetKeyCallback(w, Keyboard);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    printf("GL version: %s\n", glGetString(GL_VERSION));

    loadMesh(fairy, "./Healing_Fairy.obj");
    loadMesh(rock, "./Rock.obj");
    loadTexture(rock, "./RockGrey.tga");

    initShader(fairy, "./FairyVertexShader.glsl", "./LightSourceFragmentShader.glsl");
    initShader(rock, "./RockVertexShader.glsl", "./RockFragmentShader.glsl");
    PrintGLErrors();

    glViewport(0, 0, winW, winH);

    glfwSwapInterval(1); // ensure no generated frame backlog
    
    initializeFairies();


    // event loop
    while (!glfwWindowShouldClose(w)) {
        Display();
        glfwSwapBuffers(w);
        glfwPollEvents();
    }
    cleanup();
    glfwDestroyWindow(w);
    glfwTerminate();
}
