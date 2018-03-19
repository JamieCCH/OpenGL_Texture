//***********************************************************************************************
// HJ-101088322-Assignment2.cpp by Jamie Ching-chun Huang,101088322 (C) 2018 All Rights Reserved.
//
// Assignment 2 submission.
//
// Description:
// Click run to see the results.
// Enter how many cubes would you like in the console.
// Key 'w'/'s' zoom the camera in/out.
// Key 'a'/'d' move the camera left/right.
// Key 'r'/'f' move the camera up/down.
//***********************************************************************************************

#include "stdlib.h"
#include <iostream>

#include <glew.h>
#include "time.h"
#include <glm.hpp>
#include <matrix_transform.hpp>
#include <GLUT/GLUT.h>
#include <LoadShaders.h>
#include <SOIL.h>

#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
void keyOperations();

GLuint gVAO;
GLuint MatrixID;
GLuint cubeVao;
GLuint cubeIBO;

glm::mat4 MVP;
glm::mat4 View;
glm::mat4 Projection;

glm::vec3 currentCamPos;
glm::vec3 currentCamVel;

int frame=0,currentTime,timebase=0;
float deltaTime = 0;
bool keyStates[256] = {}; // Create an array of boolean values of length 256 (0-255)
float speed = 4.0;
float moveDist = 0;
float maxDis = 20;

void init(void){
    //Specifying the name of vertex and fragment shaders.
    ShaderInfo shaders[] = {
        { GL_VERTEX_SHADER, "triangles.vert" },
        { GL_FRAGMENT_SHADER, "triangles.frag" },
        { GL_NONE, NULL }
    };
    
    //Loading and compiling shaders
    GLuint program = LoadShaders(shaders);
    glUseProgram(program);    //My Pipeline is set up
    
    // Should I be generating some VAOs here? I don't know what I am doing, I am just an Intern
    
    // Get a handle for our "MVP" uniform
    MatrixID = glGetUniformLocation(program, "MVP");
    
    // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    
    currentCamPos = glm::vec3(0.0f,0.0f,4.0f);
    currentCamVel = glm::vec3(0.0f);
    
    // Camera matrix
    View = glm::lookAt(
                       currentCamPos, // Camera is at (4,3,3), in World Space
                       glm::vec3(0,0,0), // and looks at the origin
                       glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                       );
    
 
    glGenVertexArrays(1, &cubeVao);
    glBindVertexArray(cubeVao);
    
    
    GLint width, height;
    unsigned char* image = SOIL_load_image("/Users/jamie/Documents/2018Winter/GAME2012_3DGraphicsProgramming/02_assignments/Assignment3/AS3_JamieCCHuang/OpenGL Template/rubiksTexture.png",&width, &height, 0, SOIL_LOAD_RGB);
    GLuint cube_tex = 0;
    glGenTextures(1, &cube_tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cube_tex);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glUniform1i(glGetUniformLocation(program, "texture0"), 0);
    
    
    float  CubePoints[] = {
        //front
        -0.5f, -0.5f, 0.0f, //buttom left
        0.5f, -0.5f, 0.0f, //buttom right
        0.5f, 0.5f, 0.0f, //top right
        -0.5f, 0.5f, 0.0f, //top left
    };
    
    GLuint CubePointsVbo = 0;
    glGenBuffers(1, &CubePointsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, CubePointsVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubePoints), CubePoints, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    
    float CubeColor[] = {
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
    };
    
    GLuint CubeColorVbo = 0;
    glGenBuffers(1, &CubeColorVbo);
    glBindBuffer(GL_ARRAY_BUFFER, CubeColorVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeColor), CubeColor, GL_STATIC_DRAW);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    
    float extureCoordinates[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };
    
    GLuint texVbo = 0;
    glGenBuffers(1, &texVbo);
    glBindBuffer(GL_ARRAY_BUFFER, texVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(extureCoordinates), extureCoordinates, GL_STATIC_DRAW);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    

    short CubeIndexList[] = {
        0,1,2,
        0,2,3,
    };
    
    glGenBuffers(1, &cubeIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeIndexList), CubeIndexList, GL_STATIC_DRAW);
    
}



void transformObject(glm::vec3 scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation){
    
    glm::mat4 Model;
    Model = glm::mat4(1.0f);
    Model = glm::translate(Model, translation);
    Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
    Model = glm::scale(Model, scale);
    
    MVP = Projection * View * Model;
    
}

void drawCube(){
    glBindVertexArray(cubeVao);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
    
}



//---------------------------------------------------------------------
//
// display
//
void display(void){
    keyOperations();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    transformObject(glm::vec3(1.0f),Y_AXIS,0,glm::vec3(0,0,0));
    drawCube();
    
    float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 45;
    moveDist += deltaTime*speed;
    if(moveDist>=maxDis){
        speed = -4.0f;
    }else if(moveDist<=5){
        speed = 4.0f;
    }
    transformObject(glm::vec3(0.5f),Y_AXIS,angle,glm::vec3(0,moveDist,0));

    glutSwapBuffers();
    deltaTime = (glutGet(GLUT_ELAPSED_TIME) - currentTime)/1000.0f;
    currentTime = glutGet(GLUT_ELAPSED_TIME);
}

void keyDown(unsigned char key, int x, int y){
    keyStates[key] = true; // Set the state of the current key to pressed
}

void keyUp(unsigned char key, int x, int y){
    keyStates[key] = false; // Release the state of the current key to pressed
}

void keyOperations (void) {
    float cameraSpeed = 10.0f;
    if (keyStates['w']) { // If the 'w' key has been pressed
        currentCamPos.z -= cameraSpeed * (deltaTime);
    }
    if (keyStates['s']) { // If the 's' key has been pressed
        currentCamPos.z += cameraSpeed * (deltaTime);
    }
    if (keyStates['a']) { // If the 'a' key has been pressed
        currentCamPos.x -= cameraSpeed * (deltaTime);
    }
    if (keyStates['d']) { // If the 'd' key has been pressed
        currentCamPos.x += cameraSpeed * (deltaTime);
    }
    if (keyStates['r']) { // If the 'r' key has been pressed
        currentCamPos.y += cameraSpeed * (deltaTime);
    }
    if (keyStates['f']) { // If the 'f' key has been pressed
        currentCamPos.y -= cameraSpeed * (deltaTime);
    }
    View = glm::lookAt(
                       currentCamPos, // Camera is at (4,3,3), in World Space
                       glm::vec3(0,0,0), // and looks at the origin
                       glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                       );
}

void keyboardFun(unsigned char key, int x, int y){
    float cameraSpeed = 10.0f;
    switch(key) {
        case 'w':
            // call a function
            currentCamPos.z -= cameraSpeed * (deltaTime);
            std::cout<<deltaTime<<std::endl;
            break;
        case 's':
            currentCamPos.z += cameraSpeed * (deltaTime);
            // call a function
            break;
        case 'a':
            currentCamPos.x -= cameraSpeed * (deltaTime);
            // call a function
            break;
        case 'd':
            currentCamPos.x += cameraSpeed * (deltaTime);
            // call a function
            break;
        case 'r':
            currentCamPos.y += cameraSpeed * (deltaTime);
            // call a function
            break;
        case 'f':
            currentCamPos.y -= cameraSpeed * (deltaTime);
            // call a function
            break;
            
        default:
            break;
    }
    
}

void idle(){
    
}

void Timer(int id){
    glutPostRedisplay();
    glutTimerFunc(15, Timer, 0);
}

//---------------------------------------------------------------------
//
// main
//
int main(int argc, char** argv){
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_DEPTH);
    
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Hello World! This is funny haha!");
    
    glewExperimental = true;
    glewInit();    //Initializes the glew and prepares the drawing pipeline.
    
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
    glEnable(GL_DEPTH_TEST);
    init();
    
    glutTimerFunc(15, Timer, 0);
    
    glutDisplayFunc(display);
    
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    
    glutIdleFunc(idle);
    
    glutMainLoop();
}

