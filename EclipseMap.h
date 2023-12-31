#ifndef ECLIPSEMAP_H
#define ECLIPSEMAP_H

#include <vector>
#include <GL/glew.h>
#include <iostream>
#include "glm/glm/ext.hpp"
#include "Shader.h"
#include <vector>
#include "glm/glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <jpeglib.h>
#include <GL/glew.h>

#define PI 3.14159265359
using namespace std;

struct vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture;

    vertex() {}

    vertex(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &texture) : position(position),
                                                                                           normal(normal),
                                                                                           texture(texture) {}
};

struct triangle {
    int vertex1;
    int vertex2;
    int vertex3;

    triangle() {}

    triangle(const int &vertex1, const int &vertex2, const int &vertex3) : vertex1(vertex1), vertex2(vertex2),
                                                                           vertex3(vertex3) {}
};

class EclipseMap {
private:
    float heightFactor = 80;
    float textureOffset = 0;
    float orbitDegree = 0;
    glm::vec3 lightPos = glm::vec3(0, 4000, 0);
    bool pKeyPressed = false;
    // DISPLAY SETTINGS
    enum displayFormatOptions {
        windowed = 1, fullScreen = 0
    };
    const char *windowName = "Ceng477 - HW3";
    int defaultScreenWidth = 1000;
    int defaultScreenHeight = 1000;
    int screenWidth = defaultScreenWidth;
    int screenHeight = defaultScreenHeight;
    int displayFormat = displayFormatOptions::windowed;
    // CAMERA SETTINGS
    float projectionAngle = 45;
    float aspectRatio = 1;
    float near = 0.1;
    float far = 10000;
    float startPitch = 180;
    float startYaw = 90;
    float startSpeed = 0;
    float pitch = startPitch;
    float yaw = startYaw;
    float speed = startSpeed;
    glm::vec3 cameraStartPosition = glm::vec3(0, 4000, 4000);
    glm::vec3 cameraStartDirection = glm::vec3(0, -1, -1);
    glm::vec3 cameraStartUp = glm::vec3(0, 0, 1);
    glm::vec3 cameraUp = cameraStartUp;
    glm::vec3 cameraPosition = cameraStartPosition;
    glm::vec3 cameraDirection = cameraStartDirection;
    // MATRICES
    glm::mat4x4 mMix, mMod, mPro, mCam, mTra, mRot, mIdent = glm::mat4x4();
    float radian, selfRotation, moonSelfRotation;
    //KEYBOARD AVAILABLE
    bool availA=true, availD=true, availS=true, availW=true, availR=true, availF=true,
    availY=true, availH=true, availI=true, availX=true, availP=true, availQ=true;

public:
    unsigned int textureColor;
    unsigned int textureGrey;
    unsigned int VAO;
    unsigned int VBO, EBO;
    float imageHeight;
    float imageWidth;
    float radius = 600;
    int horizontalSplitCount = 250;
    int verticalSplitCount = 125;
    int iter, iterSelf, iterMoonSelf;
    float rotation = 0.5/horizontalSplitCount;

    unsigned int moonTextureColor;
    unsigned int moonVAO;
    unsigned int moonVBO, moonEBO;
    float moonImageHeight;
    float moonImageWidth;
    float moonRadius = 162;
    glm::vec3 moonPosition = glm::vec3(2600, 2600, 0);

    vector<vertex> worldVertices;
    vector<unsigned int> worldIndices;

    vector<vertex> moonVertices;
    vector<unsigned int> moonIndices;

    GLFWwindow *openWindow(const char *windowName, int width, int height);

    void Render(const char *coloredTexturePath, const char *greyTexturePath, const char *moonTexturePath);

    void handleKeyPress(GLFWwindow *window);
    
    void handleKeyPressDebugMode(GLFWwindow *window);

    void initColoredTexture(const char *filename, GLuint shader);

    void initGreyTexture(const char *filename, GLuint shader);

    void initMoonColoredTexture(const char *filename, GLuint shader);

    void initSphere(vector<vertex>* sphereVertices, vector<unsigned int>* sphereIndices, float radius);

    void initBuffers(bool moon);

    void calculateCam(bool moon);
    
    void calculateUniforms(GLuint shaderId, bool moon);

    void bindArrayObject(bool moon);

    void printVectors(vector<vertex>* sphereVertices, vector<unsigned int>* sphereIndices, bool VP);

};

#endif
