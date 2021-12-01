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
#include "Misc.h"
#include <math.h>


// camera object allows us to use all the handy methods in the camera class
int winW = 750, winH = 750;
Camera camera((float)winW / winH, vec3(0, 0, 0), vec3(0, 0, -5));

const int NUM_FAIRIES = 3;

struct Object {
    GLuint vertexBuffer, shaderProgram;
    int textureID;
    std::vector<vec3> points;           // 3D mesh vertices
    std::vector<int3> triangles;        // triplets of vertex indices
    std::vector<vec3> normals;          // surface normals
    std::vector<vec2> uvs;              // uv coordinates
};

struct Object fairy;
struct Object rock;

struct MovingFairy {
    // current coordinates
    float x;
    float y;
    // increments made to current position in order to reach target
    float xIncr;
    float yIncr;
    // target coordinates
    float targetX;
    float targetY;
    // total distance needed to move 
    float distX;
    float distY;
    // total distance moved
    float moveX;
    float moveY;
    // index of targets vector
    int currentPosition = 0;
    int targetPosition = 1;
    /*bool moveForward = true;
    bool moveUp = true;*/
};

vector<vec2> targets = { {0.0000f, 0.0000f}, {1.0000f, 1.0000f}, {0.0000f, 1.0000f}, {-1.0000f, -1.0000f}, {1.0000f, 0.0000f}, {0.0000f, -1.0000f}, {-1.0000f, 1.0000f}};

struct MovingFairy fairy1;

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

void loadTexture(struct Object& obj, std::string filePath) {
    obj.textureID = LoadTexture(filePath.c_str(), 0);
}

void updateTargets(MovingFairy &fairy) {
    vec2 currentPos = targets.at(fairy.currentPosition);
    fairy.x = currentPos.x;
    fairy.y = currentPos.y;

    vec2 firstTarget = targets.at(fairy.targetPosition);
    fairy.targetX = firstTarget.x;
    fairy.targetY = firstTarget.y;

    fairy.distX = fairy.targetX - fairy.x;
    fairy.distY = fairy.targetY - fairy.y;
    fairy.xIncr = fairy.distX * 0.002;
    fairy.yIncr = fairy.distY * 0.002;
    fairy.moveX = 0.0;
    fairy.moveY = 0.0;
    printf("\nTarget updated to %d, targetX = %f, targetY = %f, distX = %f, distY = %f, xIncr = %f, yIncr = %f\n", fairy.targetPosition, fairy.targetX, fairy.targetY, fairy.distX, fairy.distY, fairy.xIncr, fairy.yIncr);
}

void incrementPosition(MovingFairy& fairy) {
    
    ++fairy.currentPosition;
    if (fairy.currentPosition > targets.size() - 1) {
        fairy.currentPosition = 0;
    }

    ++fairy.targetPosition;
    if (fairy.targetPosition > targets.size() - 1) {
        fairy.targetPosition = 0;
    }
    
    /*vec2 newTarget = targets.at(fairy.targetPosition);
    fairy.targetX = newTarget.x;
    fairy.targetY = newTarget.y;*/
    updateTargets(fairy);
    
}

void adjustMovement(MovingFairy &fairy) {
    // adjust x direction
    //if (fairy.moveForward) {
    //    fairy.moveX += 0.002;
    //    if (fairy.moveX >= 1.1f) {
    //        // we've reached the edge, reverse
    //       fairy.moveForward = false;
    //    }
    //}
    //else {
    //    fairy.moveX -= 0.002;
    //    if (fairy.moveX <= -1.1f) {
    //        fairy.moveForward = true;
    //    }
    //}
    //// adjust y direction
    //if (fairy.moveUp) {
    //    fairy.moveY += 0.002;
    //    if (fairy.moveY >= 1.1f) {
    //        fairy.moveUp = false;
    //    }
    //}
    //else {
    //    fairy.moveY -= 0.002;
    //    if (fairy.moveY <= -1.1f) {
    //        fairy.moveUp = true;
    //    }
    //}
    
    fairy.x += fairy.xIncr;
    fairy.moveX += abs(fairy.xIncr);
    fairy.y += fairy.yIncr;
    fairy.moveY += abs(fairy.yIncr);

    vec2 startPos = targets.at(fairy.currentPosition);

    // if we've met or exceeded the target position, switch to the next position 
    if ((abs(fairy.moveX) > abs(fairy.targetX - startPos.x))) {
        incrementPosition(fairy);
    }

    
}

void drawFairy() {
    glBindBuffer(GL_ARRAY_BUFFER, fairy.vertexBuffer);
    VertexAttribPointer(fairy.shaderProgram, "point", 3, 0, (void*)0);
    VertexAttribPointer(fairy.shaderProgram, "normal", 3, 0, (void*)(fairy.points.size() * sizeof(vec3)));

    glUseProgram(fairy.shaderProgram);
    adjustMovement(fairy1);
    mat4 translation = Translate(fairy1.x, fairy1.y, 0);
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
    int pSize = rock.points.size() * sizeof(vec3), nSize = rock.normals.size() * sizeof(vec3);
    VertexAttribPointer(rock.shaderProgram, "uv", 2, 0, (void*)(pSize + nSize));

    glUseProgram(rock.shaderProgram);
    mat4 translation = Translate(0.0f, -0.5f, -0.5f);
    mat4 scale = Scale(0.5, 0.5, 0.5);
    mat4 rotation = RotateY(30.0f);
    SetUniform(rock.shaderProgram, "textureImage", 0);
    SetUniform(rock.shaderProgram, "modelview", translation * camera.modelview * rotation * scale);
    SetUniform(rock.shaderProgram, "persp", camera.persp);
    SetUniform(rock.shaderProgram, "lightPosition", vec3(fairy1.x, fairy1.y, -5.00f));
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
    // clear background
    //glClearColor(0.005f, 0.005f, 0.1f, 1);
    glClearColor(0.0f, 0.0f, 0.02f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
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
    
    updateTargets(fairy1);

    // event loop
    while (!glfwWindowShouldClose(w)) {
        Display();
        glfwSwapBuffers(w);
        glfwPollEvents();
    }
    glfwDestroyWindow(w);
    glfwTerminate();
}
