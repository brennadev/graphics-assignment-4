//
//  MazeGame.cpp
//  Assignment4
//
//  Created by Brenna Olson on 11/13/18.
//  Copyright © 2018 Brenna Olson. All rights reserved.
//




//Multi-Object, Multi-Texture Example
//Stephen J. Guy, 2018

//This example demonstartes:
// Loading multiple models (a cube and a knot)
// Using multiple textures (wood and brick)
// Instancing (the teapot is drawn in two locatons)
// Continuous keyboard input - arrows (moves knot up/down/left/right continueous on being held)
// Keyboard modifiers - shift (up/down arrows move knot in/out of screen when shift is pressed)
// Single key events - pressing 'c' changes color of a random teapot
// Mixing textures and colors for models
// Phong lighting
// Binding multiple textures to one shader

const char* INSTRUCTIONS =
"***************\n"
"This demo shows multiple objects being draw at once along with user interaction.\n"
"\n"
"Up/down/left/right - Moves the knot.\n"
"c - Changes to teapot to a random color.\n"
"***************\n"
;

//Mac OS build: g++ multiObjectTest.cpp -x c glad/glad.c -g -F/Library/Frameworks -framework SDL2 -framework OpenGL -o MultiObjTest
//Linux build:  g++ multiObjectTest.cpp -x c glad/glad.c -g -lSDL2 -lSDL2main -lGL -ldl -I/usr/include/SDL2/ -o MultiObjTest

#include "glad/glad.h"  //Include order can matter here
#if defined(__APPLE__) || defined(__linux__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif
#include <cstdio>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "CustomTypes.h"

using namespace std;

int screenWidth = 800;
int screenHeight = 600;
float timePast = 0;

//SJG: Store the object coordinates
//You should have a representation for the state of each object
float objx=0, objy=0, objz=0;
float colR=1, colG=1, colB=1;



bool DEBUG_ON = true;
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
bool fullscreen = false;
void Win2PPM(int width, int height);

//srand(time(NULL));
float rand01(){
    return rand()/(float)RAND_MAX;
}

void drawGeometry(int shaderProgram, int model1_start, int model1_numVerts, int model2_start, int model2_numVerts, const vector<Object> &objects);


vector<Object> readMapFile(int *width, int *height);


int main(int argc, char *argv[]){
    # pragma mark - Camera Vars
    glm::vec3 cameraDirection = glm::vec3(1, 0, 0);
    float cameraAngle = 0;
    glm::vec3 cameraPosition = glm::vec3(3.f, 0.f, 0.f);
    
    # pragma mark - Setup
    SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)
    
    //Ask SDL to get a recent version of OpenGL (3.2 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    
    //Create a window (offsetx, offsety, width, height, flags)
    SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
    
    //Create a context to draw in
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    //Load OpenGL extentions with GLAD
    if (gladLoadGLLoader(SDL_GL_GetProcAddress)){
        printf("\nOpenGL loaded\n");
        printf("Vendor:   %s\n", glGetString(GL_VENDOR));
        printf("Renderer: %s\n", glGetString(GL_RENDERER));
        printf("Version:  %s\n\n", glGetString(GL_VERSION));
    }
    else {
        printf("ERROR: Failed to initialize OpenGL context.\n");
        return -1;
    }
    
    
    
    
    # pragma mark - Map File Reading
    int width;
    int height;
    vector<Object> objects = readMapFile(&width, &height);
    
    
    //Here we will load two different model files
    
    //Load Model 1
    ifstream modelFile;
    modelFile.open("models/cube.txt");
    int numLines = 0;
    modelFile >> numLines;
    float* model1 = new float[numLines];
    for (int i = 0; i < numLines; i++){
        modelFile >> model1[i];
    }
    printf("%d\n",numLines);
    int numVertsTeapot = numLines/8;
    modelFile.close();
    
    //Load Model 2
    modelFile.open("models/knot.txt");
    numLines = 0;
    modelFile >> numLines;
    float* model2 = new float[numLines];
    for (int i = 0; i < numLines; i++){
        modelFile >> model2[i];
    }
    printf("%d\n",numLines);
    int numVertsKnot = numLines/8;
    modelFile.close();
    
    //SJG: I load each model in a different array, then concatenate everything in one big array
    // This structure works, but there is room for improvement here. Eg., you should store the start
    // and end of each model a data structure or array somewhere.
    //Concatenate model arrays
    float* modelData = new float[(numVertsTeapot+numVertsKnot)*8];
    copy(model1, model1+numVertsTeapot*8, modelData);
    copy(model2, model2+numVertsKnot*8, modelData+numVertsTeapot*8);
    int totalNumVerts = numVertsTeapot+numVertsKnot;
    int startVertTeapot = 0;  //The teapot is the first model in the VBO
    int startVertKnot = numVertsTeapot; //The knot starts right after the taepot
    
    
    //// Allocate Texture 0 (Wood) ///////
    SDL_Surface* surface = SDL_LoadBMP("wood.bmp");
    if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex0;
    glGenTextures(1, &tex0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture
    
    SDL_FreeSurface(surface);
    //// End Allocate Texture ///////
    
    
    //// Allocate Texture 1 (Brick) ///////
    SDL_Surface* surface1 = SDL_LoadBMP("brick.bmp");
    if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex1;
    glGenTextures(1, &tex1);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE1);
    
    glBindTexture(GL_TEXTURE_2D, tex1);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //How to filter
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface1->w,surface1->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface1->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture
    
    SDL_FreeSurface(surface1);
    //// End Allocate Texture ///////
    
    //Build a Vertex Array Object (VAO) to store mapping of shader attributse to VBO
    GLuint vao;
    glGenVertexArrays(1, &vao); //Create a VAO
    glBindVertexArray(vao); //Bind the above created VAO to the current context
    
    //Allocate memory on the graphics card to store geometry (vertex buffer object)
    GLuint vbo[1];
    glGenBuffers(1, vbo);  //Create 1 buffer called vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
    glBufferData(GL_ARRAY_BUFFER, totalNumVerts*8*sizeof(float), modelData, GL_STATIC_DRAW); //upload vertices to vbo
    //GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
    //GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used
    
    int texturedShader = InitShader("textured-Vertex.glsl", "textured-Fragment.glsl");
    
    //Tell OpenGL how to set fragment shader input
    GLint posAttrib = glGetAttribLocation(texturedShader, "position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
    //Attribute, vals/attrib., type, isNormalized, stride, offset
    glEnableVertexAttribArray(posAttrib);
    
    //GLint colAttrib = glGetAttribLocation(phongShader, "inColor");
    //glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    //glEnableVertexAttribArray(colAttrib);
    
    GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(normAttrib);
    
    GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    
    GLint uniView = glGetUniformLocation(texturedShader, "view");
    GLint uniProj = glGetUniformLocation(texturedShader, "proj");
    
    glBindVertexArray(0); //Unbind the VAO in case we want to create a new one
    
    
    glEnable(GL_DEPTH_TEST);
    
    printf("%s\n",INSTRUCTIONS);
    
    
    # pragma mark - Run Loop
    //Event Loop (Loop forever processing each event as fast as possible)
    SDL_Event windowEvent;
    bool quit = false;
    while (!quit){
        
        float oldCameraAngle = cameraAngle;
        
        while (SDL_PollEvent(&windowEvent)){  //inspect all events in the queue
            if (windowEvent.type == SDL_QUIT) quit = true;
            //List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
            //Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
                quit = true; //Exit event loop
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f){ //If "f" is pressed
                fullscreen = !fullscreen;
                SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Toggle fullscreen
            }
            
            //SJG: Use key input to change the state of the object
            //     We can use the ".mod" flag to see if modifiers such as shift are pressed
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_UP){ //If "up key" is pressed
                cameraPosition += cameraDirection * 0.05f;
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN){ //If "down key" is pressed
                cameraPosition -= cameraDirection * 0.05f;
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT){ //If "up key" is pressed
                cameraAngle -= 0.1;
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT){ //If "down key" is pressed
                //objy += .1;
                cameraAngle += 0.1;
            }
        }
        
        /*if (cameraAngle != oldCameraAngle) {
            cout << "cameraAngle: " << cameraAngle << endl;
        }*/
        
 
        
        cameraDirection.x = cos(cameraAngle);
        cameraDirection.y = sin(cameraAngle);
        //cout << "cameraDirection x: " << cameraDirection.x << endl;
        //cout << "cameraDirection y: " << cameraDirection.y << endl;
        
        // Clear the screen to default color
        glClearColor(.2f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(texturedShader);
        
        
        timePast = SDL_GetTicks()/1000.f;
        
        glm::mat4 view = glm::lookAt(
                                     cameraPosition,  //Cam Position
                                     cameraPosition + cameraDirection,  //Look at point
                                     glm::vec3(0.0f, 0.0f, 1.0f)); //Up
        
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        
        glm::mat4 proj = glm::perspective(3.14f/4, screenWidth / (float) screenHeight, 0.5f, 1000.0f); //FOV, aspect, near, far
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
        
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0);
        glUniform1i(glGetUniformLocation(texturedShader, "tex0"), 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glUniform1i(glGetUniformLocation(texturedShader, "tex1"), 1);
        
        glBindVertexArray(vao);
        drawGeometry(texturedShader, startVertTeapot, numVertsTeapot, startVertKnot, numVertsKnot, objects);
        
        SDL_GL_SwapWindow(window); //Double buffering
    }
    
    //Clean Up
    glDeleteProgram(texturedShader);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);
    
    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}

void drawGeometry(int shaderProgram, int model1_start, int model1_numVerts, int model2_start, int model2_numVerts, const vector<Object> &objects){
    
    GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(colR,colG,colB);
    glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    

    //Rotate model (matrix) based on how much time has past
    glm::mat4 model;
   
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    
    for (int i = 0; i < objects.size(); i++) {
        if (objects.at(i).type == wall) {
            model = glm::mat4(); // Load identity
            model = glm::translate(model,glm::vec3(objects.at(i).position.x * 0.25f, objects.at(i).position.y * 0.25f, 0));
            //model = glm::scale(model,2.f*glm::vec3(1.f,1.f,0.5f)); //scale example
            model = glm::scale(model, 0.25f * glm::vec3(1, 1, 1));
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            
            //Set which texture to use (0 = wood texture ... bound to GL_TEXTURE0)
            glUniform1i(uniTexID, 0);
            
            //Draw an instance of the model (at the position & orientation specified by the model matrix above)
            glDrawArrays(GL_TRIANGLES, model1_start, model1_numVerts); //(Primitive Type, Start Vertex, Num Verticies)
        }
    }
}

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile){
    FILE *fp;
    long length;
    char *buffer;
    
    // open the file containing the text of the shader code
    fp = fopen(shaderFile, "r");
    
    // check for errors in opening the file
    if (fp == NULL) {
        printf("can't open shader source file %s\n", shaderFile);
        return NULL;
    }
    
    // determine the file size
    fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
    length = ftell(fp);  // return the value of the current position
    
    // allocate a buffer with the indicated number of bytes, plus one
    buffer = new char[length + 1];
    
    // read the appropriate number of bytes from the file
    fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
    fread(buffer, 1, length, fp); // read all of the bytes
    
    // append a NULL character to indicate the end of the string
    buffer[length] = '\0';
    
    // close the file
    fclose(fp);
    
    // return the string
    return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName){
    GLuint vertex_shader, fragment_shader;
    GLchar *vs_text, *fs_text;
    GLuint program;
    
    // check GLSL version
    printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    // Create shader handlers
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Read source code from shader files
    vs_text = readShaderSource(vShaderFileName);
    fs_text = readShaderSource(fShaderFileName);
    
    // error check
    if (vs_text == NULL) {
        printf("Failed to read from vertex shader file %s\n", vShaderFileName);
        exit(1);
    } else if (DEBUG_ON) {
        printf("Vertex Shader:\n=====================\n");
        printf("%s\n", vs_text);
        printf("=====================\n\n");
    }
    if (fs_text == NULL) {
        printf("Failed to read from fragent shader file %s\n", fShaderFileName);
        exit(1);
    } else if (DEBUG_ON) {
        printf("\nFragment Shader:\n=====================\n");
        printf("%s\n", fs_text);
        printf("=====================\n\n");
    }
    
    // Load Vertex Shader
    const char *vv = vs_text;
    glShaderSource(vertex_shader, 1, &vv, NULL);  //Read source
    glCompileShader(vertex_shader); // Compile shaders
    
    // Check for errors
    GLint  compiled;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printf("Vertex shader failed to compile:\n");
        if (DEBUG_ON) {
            GLint logMaxSize, logLength;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
            printf("printing error message of %d bytes\n", logMaxSize);
            char* logMsg = new char[logMaxSize];
            glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
            printf("%d bytes retrieved\n", logLength);
            printf("error message: %s\n", logMsg);
            delete[] logMsg;
        }
        exit(1);
    }
    
    // Load Fragment Shader
    const char *ff = fs_text;
    glShaderSource(fragment_shader, 1, &ff, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
    
    //Check for Errors
    if (!compiled) {
        printf("Fragment shader failed to compile\n");
        if (DEBUG_ON) {
            GLint logMaxSize, logLength;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
            printf("printing error message of %d bytes\n", logMaxSize);
            char* logMsg = new char[logMaxSize];
            glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
            printf("%d bytes retrieved\n", logLength);
            printf("error message: %s\n", logMsg);
            delete[] logMsg;
        }
        exit(1);
    }
    
    // Create the program
    program = glCreateProgram();
    
    // Attach shaders to program
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    
    // Link and set program to use
    glLinkProgram(program);
    
    return program;
}



/*# pragma mark - Library Includes
#include "glad/glad.h"  //Include order can matter here
#if defined(__APPLE__) || defined(__linux__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


# pragma mark - C++ Standard Library Includes
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


# pragma mark - Project Includes
#include "CustomTypes.h"

using namespace std;


# pragma mark - Constants
bool isFullScreen = false;  // TODO: not sure if I'll need this
const int screenWidth = 800;
const int screenHeight = 600;
bool DEBUG_ON = true;


# pragma mark - Object Coordinates
float objx=0, objy=0, objz=0;
float colR=1, colG=1, colB=1;


# pragma mark - Function Prototypes
/// Read in data from map file
vector<Object> readMapFile(int *width, int *height);
static char* readShaderSource(const char* shaderFile);
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);


int main(int argc, char *argv[]) {
    # pragma mark - Setup
    SDL_Init(SDL_INIT_VIDEO);  // Initialize Graphics (for OpenGL)
    
    // Ask SDL to get a recent version of OpenGL (3.2 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    
    // Create a window (offsetx, offsety, width, height, flags)
    SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
    
    // Create a context to draw in
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    // Load OpenGL extentions with GLAD
    if (gladLoadGLLoader(SDL_GL_GetProcAddress)){
        printf("\nOpenGL loaded\n");
        printf("Vendor:   %s\n", glGetString(GL_VENDOR));
        printf("Renderer: %s\n", glGetString(GL_RENDERER));
        printf("Version:  %s\n\n", glGetString(GL_VERSION));
    }
    else {
        printf("ERROR: Failed to initialize OpenGL context.\n");
        return -1;
    }
    
    
    # pragma mark - Shader Setup
    int shaders = InitShader("vertex.glsl", "fragment.glsl");
    
    
    # pragma mark - Map File Reading
    int width;
    int height;
    vector<Object> objects = readMapFile(&width, &height);
    
    
    # pragma mark - Texture File Reading/Allocation
    SDL_Surface* surface = SDL_LoadBMP("Textures/cubeface.bmp");
    
    if (!surface) { // If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError());
        return 1;
    }
    
    GLuint texture0;
    glGenTextures(1, &texture0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); // Mip maps the texture
    
    SDL_FreeSurface(surface);
    
    
    # pragma mark - Model File Reading
    ifstream modelFile;
    modelFile.open("cube.txt");
    int numLines = 0;
    modelFile >> numLines;
    float* model1 = new float[numLines];
    
    for (int i = 0; i < numLines; i++){
        modelFile >> model1[i];
    }

    int cubeVertexCount = numLines / 8;
    modelFile.close();
    
    
    
    
    # pragma mark - Model Setup
    
    
    GLuint vao;
    glGenVertexArrays(1, &vao); //Create a VAO
    glBindVertexArray(vao); //Bind the above created VAO to the current context
    
    GLuint cubeVBO;
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, numLines * sizeof(float), model1, GL_STATIC_DRAW);
    
    
    # pragma mark - Shader Buffer Setup
    GLuint positionAttribute = glGetAttribLocation(shaders, "positionIn");
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(positionAttribute);
    
    GLuint normalAttribute = glGetAttribLocation(shaders, "inNormal");
    glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(normalAttribute);
    
    GLuint textureAttribute = glGetAttribLocation(shaders, "textureCoordinateIn");
    glEnableVertexAttribArray(textureAttribute);
    glVertexAttribPointer(textureAttribute, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    
    glClearColor(.2f, 0.4f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //glUseProgram(shaders);
    //glBindVertexArray(positionAttribute);  //Bind the VAO for the shaders we are using
    //glBindVertexArray(normalAttribute);
    //glBindVertexArray(textureAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 3); //Number of vertices
    
    
    
    // bind vertex array before creating attributes
    
    GLint uniView = glGetUniformLocation(shaders, "view");
    GLint uniProj = glGetUniformLocation(shaders, "projection");
    
    glBindVertexArray(0); // Unbind the VAO in case we want to create a new one
    
    
    glEnable(GL_DEPTH_TEST);
    
    
    
    
    # pragma mark - Run Loop
    SDL_Event windowEvent;
    bool quit = false;
    
    while (!quit) {
        while (SDL_PollEvent(&windowEvent)){  //inspect all events in the queue
            if (windowEvent.type == SDL_QUIT) quit = true;

            //Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
                quit = true; //Exit event loop
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f){ //If "f" is pressed
                isFullScreen = !isFullScreen;
                SDL_SetWindowFullscreen(window, isFullScreen ? SDL_WINDOW_FULLSCREEN : 0); //Toggle fullscreen
            }
            
            
            //     We can use the ".mod" flag to see if modifiers such as shift are pressed
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_UP){ //If "up key" is pressed
                //if (windowEvent.key.keysym.mod & KMOD_SHIFT) objx -= .1; //Is shift pressed?
                //else objz += .1;
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN){ //If "down key" is pressed
                //if (windowEvent.key.keysym.mod & KMOD_SHIFT) objx += .1; //Is shift pressed?
                //else objz -= .1;
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT){ //If "up key" is pressed
                //objy -= .1;
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT){ //If "down key" is pressed
                //objy += .1;
            }
            
        }
        
        // Clear the screen to default color
        glClearColor(.2f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shaders);
        
        
        //timePast = SDL_GetTicks()/1000.f;
        
        glm::mat4 view = glm::lookAt(
                                     glm::vec3(3.f, 0.f, 0.f),  //Cam Position
                                     glm::vec3(0.0f, 0.0f, 0.0f),  //Look at point
                                     glm::vec3(0.0f, 0.0f, 1.0f)); //Up
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        
        glm::mat4 proj = glm::perspective(3.14f/4, screenWidth / (float) screenHeight, 1.0f, 10.0f); //FOV, aspect, near, far
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
        
        
        
        
        
        ///////////////////
        
        GLint uniColor = glGetUniformLocation(shaders, "inColor");
        glm::vec3 colVec(colR,colG,colB);
        glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
        
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, texture0);
        //glUniform1i(glGetUniformLocation(shaders, "tex0"), 0);
        
        
        
        
        //glUniform1i(texture0, 0);
        glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
        
        
        ///////////////////
        
        
        
        //glBindVertexArray(vao);
        //drawGeometry(texturedShader, startVertTeapot, numVertsTeapot, startVertKnot, numVertsKnot);
        
        SDL_GL_SwapWindow(window); //Double buffering
    }
    
    
    # pragma mark - Cleanup
    glDeleteProgram(shaders);
    //glDeleteShader()
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &positionAttribute);
    glDeleteVertexArrays(1, &normalAttribute);
    glDeleteVertexArrays(1, &textureAttribute);
    
    SDL_GL_DeleteContext(context);
    SDL_Quit();
    
    return 0;
}
*/

# pragma mark - Other Setup
vector<Object> readMapFile(int *width, int *height) {
    ifstream mapFile("mapInput.txt");
    
    if (!mapFile) {
        cout << "Unable to open map input file\n";
        exit(1);
    }
    
    mapFile >> *width >> *height;
    
    vector<Object> objects;
    
    char currentItem;
    
    for (int j = 0; j < *height; j++) {
        for (int i = 0; i < *width; i++) {
            mapFile >> currentItem;
            
            switch (currentItem) {
                case 'S':
                    objects.push_back({startLocation, {i, j}});
                    break;
                    
                case 'G':
                    objects.push_back({endLocation, {i, j}});
                    break;
                    
                case 'W':
                    objects.push_back({wall, {i, j}});
                    break;
                    
                case 'A':
                    objects.push_back({doorA, {i, j}});
                    break;
                    
                case 'B':
                    objects.push_back({doorB, {i, j}});
                    break;
                    
                case 'C':
                    objects.push_back({doorC, {i, j}});
                    break;
                    
                case 'D':
                    objects.push_back({doorD, {i, j}});
                    break;
                    
                case 'E':
                    objects.push_back({doorE, {i, j}});
                    break;
                    
                case 'a':
                    objects.push_back({keya, {i, j}});
                    break;
                    
                case 'b':
                    objects.push_back({keyb, {i, j}});
                    break;
                    
                case 'c':
                    objects.push_back({keyc, {i, j}});
                    break;
                    
                case 'd':
                    objects.push_back({keyd, {i, j}});
                    break;
                    
                case 'e':
                    objects.push_back({keye, {i, j}});
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    return objects;
}

/*
# pragma mark - Shader Setup
// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile){
    FILE *fp;
    long length;
    char *buffer;
    
    // open the file containing the text of the shader code
    fp = fopen(shaderFile, "r");
    
    // check for errors in opening the file
    if (fp == NULL) {
        printf("can't open shader source file %s\n", shaderFile);
        return NULL;
    }
    
    // determine the file size
    fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
    length = ftell(fp);  // return the value of the current position
    
    // allocate a buffer with the indicated number of bytes, plus one
    buffer = new char[length + 1];
    
    // read the appropriate number of bytes from the file
    fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
    fread(buffer, 1, length, fp); // read all of the bytes
    
    // append a NULL character to indicate the end of the string
    buffer[length] = '\0';
    
    // close the file
    fclose(fp);
    
    // return the string
    return buffer;
}


// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName){
    GLuint vertex_shader, fragment_shader;
    GLchar *vs_text, *fs_text;
    GLuint program;
    
    // check GLSL version
    printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    // Create shader handlers
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Read source code from shader files
    vs_text = readShaderSource(vShaderFileName);
    fs_text = readShaderSource(fShaderFileName);
    
    // error check
    if (vs_text == NULL) {
        printf("Failed to read from vertex shader file %s\n", vShaderFileName);
        exit(1);
    } else if (DEBUG_ON) {
        printf("Vertex Shader:\n=====================\n");
        printf("%s\n", vs_text);
        printf("=====================\n\n");
    }
    if (fs_text == NULL) {
        printf("Failed to read from fragent shader file %s\n", fShaderFileName);
        exit(1);
    } else if (DEBUG_ON) {
        printf("\nFragment Shader:\n=====================\n");
        printf("%s\n", fs_text);
        printf("=====================\n\n");
    }
    
    // Load Vertex Shader
    const char *vv = vs_text;
    glShaderSource(vertex_shader, 1, &vv, NULL);  //Read source
    glCompileShader(vertex_shader); // Compile shaders
    
    // Check for errors
    GLint  compiled;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printf("Vertex shader failed to compile:\n");
        if (DEBUG_ON) {
            GLint logMaxSize, logLength;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
            printf("printing error message of %d bytes\n", logMaxSize);
            char* logMsg = new char[logMaxSize];
            glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
            printf("%d bytes retrieved\n", logLength);
            printf("error message: %s\n", logMsg);
            delete[] logMsg;
        }
        exit(1);
    }
    
    // Load Fragment Shader
    const char *ff = fs_text;
    glShaderSource(fragment_shader, 1, &ff, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
    
    //Check for Errors
    if (!compiled) {
        printf("Fragment shader failed to compile\n");
        if (DEBUG_ON) {
            GLint logMaxSize, logLength;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
            printf("printing error message of %d bytes\n", logMaxSize);
            char* logMsg = new char[logMaxSize];
            glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
            printf("%d bytes retrieved\n", logLength);
            printf("error message: %s\n", logMsg);
            delete[] logMsg;
        }
        exit(1);
    }
    
    // Create the program
    program = glCreateProgram();
    
    // Attach shaders to program
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    
    // Link and set program to use
    glLinkProgram(program);
    
    return program;
}
*/
