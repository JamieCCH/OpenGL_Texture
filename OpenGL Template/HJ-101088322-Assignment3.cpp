//***********************************************************************************************
// HJ-101088322-Assignment3.cpp by Jamie Ching-chun Huang,101088322 (C) 2018 All Rights Reserved.
//
// Assignment 3 submission.
//
// Description:
// Click run to see the results.
//
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

GLuint MatrixID;
GLuint cubeVao;


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
    
    currentCamPos = glm::vec3(-2.0f,2.0f,3.0f);
    currentCamVel = glm::vec3(0.0f);

    // Camera matrix
    View = glm::lookAt(
                       currentCamPos, // Camera is at (-2,2,3), in World Space
                       glm::vec3(0,0,0), // and looks at the origin
                       glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                       );
    
//Create a VAO to describe the cube
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
    
//VBO for cube vertex positions
    float  CubePoints[] = {
        //top
        -0.65f, 0.65f, 0.65f,   //0
        0.65f, 0.65f, 0.65f,    //1
        0.65f, 0.65f, -0.65f,   //2
        -0.65f, 0.65f, -0.65f,  //3
        //front
        -0.65f, -0.65f, 0.65f,  //buttom left    //4
        0.65f, -0.65f, 0.65f,   //buttom right   //5
        0.65f, 0.65f, 0.65f,    //top right      //6
        -0.65f, 0.65f, 0.65f,   //top left       //7
        //left
        -0.65f, -0.65f, -0.65f, //8
        -0.65f, -0.65f, 0.65f,  //9
        -0.65f, 0.65f, 0.65f,   //10
        -0.65f, 0.65f, -0.65f,  //11
        //bottom
        -0.65f, -0.65f, 0.65f,  //0 12
        0.65f, -0.65f, 0.65f,   //1 13
        0.65f, -0.65f, -0.65f,  //5 14
        -0.65f, -0.65f, -0.65f, //4 15
        //right
        0.65f, -0.65f, 0.65f,   //16
        0.65f, -0.65f, -0.65f,  //17
        0.65f, 0.65f, -0.65f,   //18
        0.65f, 0.65f, 0.65f,    //19
        //back
        -0.65f, -0.65f, -0.65f, //20
        0.65f, -0.65f, -0.65f,  //5 21
        0.65f, 0.65f, -0.65f,   //6 22
        -0.65f, 0.65f, -0.65f,  //7 23
    };
    
    GLuint CubePointsVbo = 0;
    glGenBuffers(1, &CubePointsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, CubePointsVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubePoints), CubePoints, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    
//VBO for cube color positions
//    float CubeColor[] = {
//        1.0, 0.0, 0.0,
//        1.0, 0.0, 0.0,
//        1.0, 0.0, 0.0,
//        1.0, 0.0, 0.0,
//    };
//
//    GLuint CubeColorVbo = 0;
//    glGenBuffers(1, &CubeColorVbo);
//    glBindBuffer(GL_ARRAY_BUFFER, CubeColorVbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeColor), CubeColor, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//    glEnableVertexAttribArray(1);
    
//VBO for vertex texture uv map.
    float textureCoordinates[] = {
        //top
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        //front
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        //left
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        //bottom
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        //right
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        //back
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };

    GLuint texVbo = 0;
    glGenBuffers(1, &texVbo);
    glBindBuffer(GL_ARRAY_BUFFER, texVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    

    short CubeIndexList[] = {
        //top
        0,1,2,
        0,2,3,
        //front
        4,5,6,
        4,6,7,
        //left
        8,9,10,
        8,10,11,
        //bottom
        15,14,13,
        15,13,12,
        //right
        16,17,18,
        16,18,19,
        //back
        21,20,23,
        21,23,22,
    };
    
    GLuint cubeIBO;
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
    glClearColor(0.3, 0.5, 0.7, 1.0);
    
    transformObject(glm::vec3(1.0f),Y_AXIS,0,glm::vec3(0,0,0));
//    float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 45;
//    transformObject(glm::vec3(1.0f),Y_AXIS,angle,glm::vec3(0,0,0));
    drawCube();
    

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
    
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(300, 100);
    glutCreateWindow("Huang, Jamie Ching-chun, 101088322");
    
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


