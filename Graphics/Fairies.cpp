/**
Fairies.cpp
Alisa Wallace, CPSC 5700 FQ21
*/

#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include "GLXtras.h"
#include <time.h>
#include <vector>
#include "Mesh.h"
#include "Camera.h"

// TODO - update to account for multiple fairies
vector<float> movement = { 0.0, 0.0 };
vector<bool> moveForward = { true, false };

// camera object allows us to use all the handy methods in the camera class
int winW = 750, winH = 750;
Camera camera((float)winW / winH, vec3(0, 0, 0), vec3(0, 0, -5));

struct Object {
    GLuint vertexBuffer, shaderProgram;
    std::vector<vec3> points;           // 3D mesh vertices
    std::vector<int3> triangles;        // triplets of vertex indices
    std::vector<vec3> normals;          // surface normals
    std::vector<vec2> uvs;              // uv coordinates

};

struct Object fairy;
struct Object rock;

void loadMesh(struct Object &obj, std::string filePath) {
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

void adjustMovement(int ID) {
    // which direction are we moving in?
    if (moveForward[ID]) {
        movement[ID] += 0.003;
        if (movement[ID] >= 1.0f) {
            // we've reached the edge, reverse
            moveForward[ID] = !moveForward[ID];
        }
    }
    else {
        movement[ID] -= 0.003;
        if (movement[ID] <= -1.0f) {
            // we've reached opposite edge, reverse
            moveForward[ID] = !moveForward[ID];
        }
    }
}

void drawFairy() {
    glBindBuffer(GL_ARRAY_BUFFER, fairy.vertexBuffer);
    VertexAttribPointer(fairy.shaderProgram, "point", 3, 0, (void*)0);
    VertexAttribPointer(fairy.shaderProgram, "normal", 3, 0, (void*)(fairy.points.size() * sizeof(vec3)));

    glUseProgram(fairy.shaderProgram);
    adjustMovement(0);
    mat4 translation = Translate(movement[0], movement[0], 0);
    mat4 scale = Scale(0.1, 0.1, 0.1);
    mat4 rotation = RotateY(30.0f);
    SetUniform(fairy.shaderProgram, "modelview", translation * camera.modelview * rotation * scale);
    SetUniform(fairy.shaderProgram, "persp", camera.persp);

    // render triangles
    glDrawElements(GL_TRIANGLES, 3 * fairy.triangles.size(), GL_UNSIGNED_INT, &fairy.triangles[0]);
    glFlush();
}

void drawRock() {
    glBindBuffer(GL_ARRAY_BUFFER, rock.vertexBuffer);
    VertexAttribPointer(rock.shaderProgram, "point", 3, 0, (void*)0);
    VertexAttribPointer(rock.shaderProgram, "normal", 3, 0, (void*)(rock.points.size() * sizeof(vec3)));

    glUseProgram(rock.shaderProgram);
    mat4 translation = Translate(0.0f, -0.5f, -0.5f);
    mat4 scale = Scale(0.5, 0.5, 0.5);
    mat4 rotation = RotateY(30.0f);
    SetUniform(rock.shaderProgram, "modelview", translation * camera.modelview * rotation * scale);
    SetUniform(rock.shaderProgram, "persp", camera.persp);
    SetUniform(rock.shaderProgram, "lightPosition", vec3(movement[0], movement[0], -5.00f));
    SetUniform(rock.shaderProgram, "viewPosition", camera.GetTran());

    // render triangles
    glDrawElements(GL_TRIANGLES, 3 * rock.triangles.size(), GL_UNSIGNED_INT, &rock.triangles[0]);
    glFlush();
}

void Resize(GLFWwindow* window, int width, int height) {
    camera.Resize(winW = width, winH = height);
    glViewport(0, 0, winW, winH);
}

void Display() {
    // clear background
    glClearColor(0.2f, 0.2f, 0.2f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    drawFairy();
    drawRock();
}

void initShader(struct Object &obj, std::string vertexShaderPath, std::string fragShaderPath) { 
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
    glfwSetWindowSizeCallback(w, Resize);
    glfwSetKeyCallback(w, Keyboard);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    printf("GL version: %s\n", glGetString(GL_VERSION));

    loadMesh(fairy, "./Healing_Fairy.obj");
    loadMesh(rock, "./Rock.obj");

    initShader(fairy, "./FairyVertexShader.glsl", "./LightSourceFragmentShader.glsl");
    initShader(rock, "./RockVertexShader.glsl", "./RockFragmentShader.glsl");
 
    PrintGLErrors();

    glViewport(0, 0, winW, winH);

    glfwSwapInterval(1); // ensure no generated frame backlog
    
    // event loop
    while (!glfwWindowShouldClose(w)) {
        Display();
        glfwSwapBuffers(w);
        glfwPollEvents();
    }
    glfwDestroyWindow(w);
    glfwTerminate();
}
