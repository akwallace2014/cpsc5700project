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

// GPU identifiers
GLuint vBuffer = 0;
GLuint program1 = 0;
GLuint program2 = 0;

time_t startTime = clock();
vector<float> movement = { 0.0, 0.0 };
vector<bool> moveForward = { true, false };

// camera object allows us to use all the handy methods in the camera class
int winW = 750, winH = 750;
Camera camera((float)winW / winH, vec3(0, 0, 0), vec3(0, 0, -5));
vec3 lightPos(-0.2f, -1.0f, -0.3f);

const char* objFile = "./Healing_Fairy.obj";

// reading from the object file will fill these vectors
std::vector<vec3> points;           // 3D mesh vertices
std::vector<int3> triangles;        // triplets of vertex indices
std::vector<vec3> normals;          // surface normals
std::vector<vec2> uvs;              // uv coordinates

//const char* vertexShader = R"(
//	#version 330
//    // changing to version 330 and adding layout (location = 0) causes wings to disappear!
//	layout (location = 0) in vec3 point;
//    layout (location = 1) in vec3 normal;
//	out vec3 vPoint, vNormal;
//	uniform mat4 modelview, persp;
//	void main() {
//		vPoint = (modelview*vec4(point, 1)).xyz;
//		vNormal = (modelview*vec4(normal, 0)).xyz;
//		gl_Position = 0.1 * persp*vec4(vPoint, 1);
//	}
//)";
//
//const char* pixelShader = R"(
//	#version 130
//	in vec3 vPoint, vNormal;
//    uniform vec3 lightPosition;
//    uniform vec3 viewPosition;
//
//	uniform vec3 color = vec3(1.000, 0.714, 0.757);
//    vec3 ambientLight = vec3(0.5f, 0.5f, 0.5f);
//    vec3 diffuseLight = vec3(1.0f, 1.0f, 1.0f);
//    vec3 specularLight = vec3(0.5f, 0.5f, 0.5f);
//    float lightConstant = 1.0f;
//    float lightLinear = 0.7f;
//    float lightQuadratic = 1.8f;
//    
//    vec3 emission = vec3(1.0, 0.4, 0.8);
//
//    out vec4 pColor;
//
//	//uniform float amb = 0.05, dif = .7, spc = .5;  // lighting coefficients
//    //uniform float constant = 1.0f, linear = 0.022f, quadratic = 0.0019f;
//	
//	void main() {
//        
//        vec3 ambient = ambientLight * color;
//
//        vec3 norm = normalize(vNormal);
//        vec3 lightDir = normalize(lightPosition - vPoint);
//        float diff = max(dot(norm, lightDir), 0.0);
//        vec3 diffuse = diffuseLight * diff * color;
//
//        vec3 viewDir = normalize(viewPosition - vPoint);
//        vec3 halfwayDir = normalize(lightDir + viewDir);
//        vec3 reflectDir = reflect(-lightDir, norm);
//        float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 32.0f); 
//        vec3 specular = specularLight * spec * color;
//
//        float distance = length(lightPosition - vPoint);
//        float attenuation = 1.0 / (lightConstant + lightLinear * distance + lightQuadratic * (distance * distance));
//
//        ambient *= attenuation;
//        diffuse *= attenuation;
//        specular *= attenuation;
//
//        vec3 result = ambient + diffuse + specular;
//        //result = result + result;
//
//        pColor = vec4((result), 1.0);
//	}
//)";

//const char* lightShader = R"(
//	#version 330
//    out vec4 color;
//	void main() {
//		color = vec4(1.0);
//	}
//)";

void initialSetup() {

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
    // access GPU vertex buffer


    // associate position input to shader with position array in vertex buffer
    VertexAttribPointer(program1, "point", 3, 0, (void*)0);
    VertexAttribPointer(program1, "normal", 3, 0, (void*)(points.size() * sizeof(vec3)));
    // get matrices based on changes resulting from callback functions/mouse input, send to shader
    for (int i = 0; i < 2; i++) {
            if (i == 0) {
                glUseProgram(program2);
                adjustMovement(i);
                mat4 translation = Translate(movement[i], movement[i], 0);
                mat4 scale = Scale(0.25, 0.25, 0.25);
                mat4 rotation = RotateY(30.0f);
                SetUniform(program2, "modelview", translation * camera.modelview * rotation * scale);
                SetUniform(program2, "persp", camera.persp);
                

                // render triangles
                glDrawElements(GL_TRIANGLES, 3 * triangles.size(), GL_UNSIGNED_INT, &triangles[0]);
                glFlush();
            }
            else {
                glUseProgram(program1);
                mat4 scale = Scale(0.25, 0.25, 0.25);
                mat4 rotation = RotateY(30.0f);
                SetUniform(program1, "modelview", camera.modelview * rotation * scale);
                SetUniform(program1, "persp", camera.persp);
                SetUniform(program1, "lightPosition", vec3(movement[0], movement[0], -5.00f));
                SetUniform(program1, "viewPosition", camera.GetTran());

                // render triangles
                glDrawElements(GL_TRIANGLES, 3 * triangles.size(), GL_UNSIGNED_INT, &triangles[0]);
                glFlush();
            }
        //adjustMovement(i);
        //mat4 translation = Translate(movement[i], movement[i], 0);
        //mat4 scale = Scale(0.25, 0.25, 0.25);
        //mat4 rotation = RotateY(30.0f);
        //SetUniform(program, "modelview", translation * camera.modelview * rotation * scale);
        //SetUniform(program, "persp", camera.persp);
        //SetUniform(program, "lightPosition", vec3(movement[i], movement[i], -5.00f));
        //SetUniform(program, "viewPosition", camera.GetTran());

        //// render triangles
        //glDrawElements(GL_TRIANGLES, 3 * triangles.size(), GL_UNSIGNED_INT, &triangles[0]);
        //glFlush();
    }

}

void InitVertexBuffer() {
    // make GPU buffer for points & colors, set it active buffer
    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
    // allocate buffer memory to hold vertex locations based off of the vector that was filled earlier
    int sizePoints = points.size() * sizeof(vec3), sizeNormals = normals.size() * sizeof(vec3), sizeUvs = uvs.size() * sizeof(vec2);
    glBufferData(GL_ARRAY_BUFFER, sizePoints + sizeNormals + sizeUvs, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizePoints, points.data());
    glBufferSubData(GL_ARRAY_BUFFER, sizePoints, sizeNormals, normals.data());
    glBufferSubData(GL_ARRAY_BUFFER, sizePoints + sizeNormals, sizeUvs, uvs.data());
}

bool InitShader(GLuint &program, int ID) {
    if (ID == 0) {
        program = LinkProgramViaFile("./FairyVertexShader.glsl", "FairyFragmentShader.glsl");
    }
    else {
        program = LinkProgramViaFile("./FairyVertexShader.glsl", "LightSourceFragmentShader.glsl");
    }
    if (!program)
        printf("can't init shader program\n");
    return program != 0;
}

// application

void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void ErrorGFLW(int id, const char* reason) {
    printf("GFLW error %i: %s\n", id, reason);
}

void Close() {
    // unbind vertex buffer and free GPU memory
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vBuffer);
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

    if (!ReadAsciiObj((char*)objFile, points, triangles, &normals, &uvs)) {
        printf("Failed to read .obj file (enter any key to continue)\n");
        getchar();
    }
    printf("GL version: %s\n", glGetString(GL_VERSION));
    printf("%i vertices, %i triangles, %i normals, %i uvs\n", points.size(), triangles.size(), normals.size(), uvs.size());
    Normalize(points, 0.8f);
    PrintGLErrors();

    glViewport(0, 0, winW, winH);

    if (!InitShader(program1, 0))
        return 0;
    if (!InitShader(program2, 1))
        return 0;

    InitVertexBuffer();
    glfwSwapInterval(1); // ensure no generated frame backlog
    
    // event loop
    while (!glfwWindowShouldClose(w)) {
        Display();
        glfwSwapBuffers(w);
        glfwPollEvents();
    }

    Close();
    glfwDestroyWindow(w);
    glfwTerminate();
}
