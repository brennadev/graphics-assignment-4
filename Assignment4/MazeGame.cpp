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


#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


#include "Scene.h"

using namespace std;

int screenWidth = 800;
int screenHeight = 600;


//SJG: Store the object coordinates
//You should have a representation for the state of each object
float colR=1, colG=1, colB=1;

const float cubeScaleValue = 1.0f;

bool DEBUG_ON = true;

bool fullscreen = false;


# pragma mark - Function Prototypes
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
void Win2PPM(int width, int height);
void drawGeometry(int shaderProgram, int model1_start, int model1_numVerts, int model2_start, int model2_numVerts, int keyStart, int keyNumVerts, int doorStart, int doorNumVerts, const vector<Object> &objects);

bool isWalkable(const float newX, const float newY, const float playerRadius, const int mapWidth, const int mapHeight, const vector<Object> &mapObjects);



int main(int argc, char *argv[]){
    # pragma mark - Camera Vars
    glm::vec3 cameraDirection = glm::vec3(1, 0, 0);
    float cameraAngle = 0;
    glm::vec3 cameraPosition = glm::vec3(3.f, 0.f, 0.f);
    
    
    # pragma mark - Scene Setup
    Scene scene = Scene();
    
    
    # pragma mark - Initial Setup
    SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)
    
    //Ask SDL to get a recent version of OpenGL (3.2 or greater)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    
    //Create a window (offsetx, offsety, width, height, flags)
    SDL_Window* window = SDL_CreateWindow("Maze Game", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
    
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
    Object start;
    vector<Object> objects = readMapFile(&width, &height, &start);
    cameraPosition.x = cubeScaleValue * start.position.x + 0.5;
    cameraPosition.y = cubeScaleValue * start.position.y + 0.5;
    
    float playingAreaWidth = width * cubeScaleValue;
    float playingAreaHeight = height * cubeScaleValue;
    
    
    # pragma mark - Floor Setup
    // Note: This (the floor) was such a simple thing that I just hardcoded it in - of course, it could've been specified in a model file too
    
    // floor vertex data - 6 vertices (2 triangles), 8 pieces of data
    const int floorVertexCount = 48;
    float *floorVertices = new float[floorVertexCount];
    
    // bottom left
    floorVertices[0] = 0 - 0.5 * cubeScaleValue;                   // position x
    floorVertices[1] = 0 - 0.5 * cubeScaleValue;                   // position y
    floorVertices[3] = 0;                                          // u
    floorVertices[4] = 0;                                          // v
    
    // top left
    floorVertices[8] = 0 - 0.5 * cubeScaleValue;                   // position x
    floorVertices[9] = playingAreaHeight - 0.5 * cubeScaleValue;   // position y
    floorVertices[11] = 0;                                         // u
    floorVertices[12] = 1;                                         // v
    
    // top right
    floorVertices[16] = playingAreaWidth - 0.5 * cubeScaleValue;   // position x
    floorVertices[17] = playingAreaHeight - 0.5 * cubeScaleValue;  // position y
    floorVertices[19] = 1;                                         // u
    floorVertices[20] = 1;                                         // v
    
    // bottom left
    floorVertices[24] = 0 - 0.5 * cubeScaleValue;                  // position x
    floorVertices[25] = 0 - 0.5 * cubeScaleValue;                  // position y
    floorVertices[27] = 0;                                         // u
    floorVertices[28] = 0;                                         // v
    
    // top right
    floorVertices[32] = playingAreaWidth - 0.5 * cubeScaleValue;   // position x
    floorVertices[33] = playingAreaHeight - 0.5 * cubeScaleValue;  // position y
    floorVertices[35] = 1;                                         // u
    floorVertices[36] = 1;                                         // v
    
    // bottom right
    floorVertices[40] = playingAreaWidth - 0.5 * cubeScaleValue;   // position x
    floorVertices[41] = 0 - 0.5 * cubeScaleValue;                  // position y
    floorVertices[43] = 1;                                         // u
    floorVertices[44] = 0;                                         // v
    
    
    // initialize all values that are common to all vertices
    for (int i = 0; i < 6; i++) {
        int triangleStartLocation = i * 8;
        floorVertices[triangleStartLocation + 2] = -0.125;   // position z
        floorVertices[triangleStartLocation + 5] = 0;   // normal x
        floorVertices[triangleStartLocation + 6] = 0;   // normal y
        floorVertices[triangleStartLocation + 7] = 1;   // normal z
    }
    
    
    # pragma mark - Model Loading
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
    int numVertsCube = numLines/8;
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
    
    
    //Load Model 3
    modelFile.open("models/teapot.txt");
    numLines = 0;
    modelFile >> numLines;
    float* model3 = new float[numLines];
    for (int i = 0; i < numLines; i++){
        modelFile >> model3[i];
    }
    printf("%d\n",numLines);
    int numVertsTeapot = numLines/8;
    modelFile.close();
    
    
    # pragma mark - Vertex Data in One Structure
    //SJG: I load each model in a different array, then concatenate everything in one big array
    // This structure works, but there is room for improvement here. Eg., you should store the start
    // and end of each model a data structure or array somewhere.
    //Concatenate model arrays
    float* modelData = new float[(numVertsCube+numVertsKnot + floorVertexCount + numVertsTeapot)*8];
    copy(model1, model1 + numVertsCube * 8, modelData);
    copy(model2, model2+numVertsKnot * 8, modelData+numVertsCube * 8);
    copy(floorVertices, floorVertices + floorVertexCount * 8, modelData + (numVertsCube + numVertsKnot) * 8);
    copy(model3, model3 + numVertsTeapot * 8, modelData + (numVertsCube + numVertsKnot + floorVertexCount) * 8);
    int totalNumVerts = numVertsCube+numVertsKnot + floorVertexCount;
    int startVertCube = 0;
    int startVertKnot = numVertsCube;
    int startVertFloor = numVertsCube + numVertsKnot;
    int startVertTeapot = numVertsCube + numVertsKnot + floorVertexCount;
    
    
    # pragma mark - Texture Allocation
    //// Allocate Texture 0 (Wall) ///////
    SDL_Surface* surface = SDL_LoadBMP("cubeface.bmp");
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

    
    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture
    
    SDL_FreeSurface(surface);
    //// End Allocate Texture ///////
    
    
    //// Allocate Texture 1 (Floor) ///////
    SDL_Surface* surface1 = SDL_LoadBMP("floor.bmp");
    if (surface1==NULL){ //If it failed, print the error
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
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface1->w,surface1->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface1->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture
    
    SDL_FreeSurface(surface1);
    //// End Allocate Texture ///////
    
    
    //// Allocate Texture 1 (Door A) ///////
    SDL_Surface* surface2 = SDL_LoadBMP("doorA.bmp");
    if (surface2==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex2;
    glGenTextures(1, &tex2);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE2);
    
    glBindTexture(GL_TEXTURE_2D, tex2);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface2->w,surface2->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface2->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture
    
    SDL_FreeSurface(surface2);
    //// End Allocate Texture ///////
    
    
    
    
    //// Allocate Texture 1 (Door B) ///////
    SDL_Surface* surface3 = SDL_LoadBMP("doorB.bmp");
    if (surface3==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex3;
    glGenTextures(1, &tex3);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE3);
    
    glBindTexture(GL_TEXTURE_2D, tex3);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface3->w,surface3->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface3->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture
    
    SDL_FreeSurface(surface3);
    //// End Allocate Texture ///////
    
    
    
    //// Allocate Texture 1 (Door C) ///////
    SDL_Surface* surface4 = SDL_LoadBMP("doorC.bmp");
    if (surface4==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex4;
    glGenTextures(1, &tex4);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE4);
    
    glBindTexture(GL_TEXTURE_2D, tex4);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface4->w,surface4->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface4->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture
    
    SDL_FreeSurface(surface4);
    //// End Allocate Texture ///////
    
    
    //// Allocate Texture 1 (Door D) ///////
    SDL_Surface* surface5 = SDL_LoadBMP("doorD.bmp");
    if (surface5==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex5;
    glGenTextures(1, &tex5);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE5);
    
    glBindTexture(GL_TEXTURE_2D, tex5);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface5->w,surface5->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface5->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture
    
    SDL_FreeSurface(surface5);
    //// End Allocate Texture ///////
    
    
    
    //// Allocate Texture 1 (Door E) ///////
    SDL_Surface* surface6 = SDL_LoadBMP("doorE.bmp");
    if (surface3==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex6;
    glGenTextures(1, &tex6);
    
    //Load the texture into memory
    glActiveTexture(GL_TEXTURE6);
    
    glBindTexture(GL_TEXTURE_2D, tex6);
    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface6->w,surface6->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface6->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); //Mip maps the texture
    
    SDL_FreeSurface(surface6);
    //// End Allocate Texture ///////
    
    
    # pragma mark - VAO/VBO Setup
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
    
    
    float playerRadius = 0.01;
    
    # pragma mark - Run Loop
    //Event Loop (Loop forever processing each event as fast as possible)
    SDL_Event windowEvent;
    bool quit = false;
    while (!quit) {
        
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
            

            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_UP){ //If "up key" is pressed
                cout << "current camera position x: " << cameraPosition.x << endl;
                cout << "current camera position y: " << cameraPosition.y << endl;
                
                glm::vec3 newPosition = cameraPosition + cameraDirection * 0.01f;
                
                if (isWalkable(newPosition.x, newPosition.y, playerRadius, width, height, objects)) {
                    cameraPosition = newPosition;
                }
                
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN){ //If "down key" is pressed
                
                
                cout << "current camera position x: " << cameraPosition.x << endl;
                cout << "current camera position y: " << cameraPosition.y << endl;
                
                glm::vec3 newPosition = cameraPosition - cameraDirection * 0.01f;
                
                if (isWalkable(newPosition.x, newPosition.y, playerRadius, width, height, objects)) {
                    cameraPosition = newPosition;
                }
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT){ //If "up key" is pressed
                cameraAngle += 0.1;
            }
            if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT){ //If "down key" is pressed
                cameraAngle -= 0.1;
            }
        }
 
        
        cameraDirection.x = cos(cameraAngle);
        cameraDirection.y = sin(cameraAngle);

        
        // Clear the screen to default color
        glClearColor(.2f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(texturedShader);
        
        
        glm::mat4 view = glm::lookAt(
                                     cameraPosition,  //Cam Position
                                     cameraPosition + cameraDirection,  //Look at point
                                     glm::vec3(0.0f, 0.0f, 1.0f)); //Up
        
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        
        glm::mat4 proj = glm::perspective(3.14f/3, screenWidth / (float) screenHeight, 0.03125f, 2000.0f); //FOV, aspect, near, far
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
        
        
        # pragma mark - Activate Textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0);
        glUniform1i(glGetUniformLocation(texturedShader, "tex0"), 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glUniform1i(glGetUniformLocation(texturedShader, "tex1"), 1);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, tex2);
        glUniform1i(glGetUniformLocation(texturedShader, "tex2"), 2);
        
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, tex3);
        glUniform1i(glGetUniformLocation(texturedShader, "tex3"), 3);
        
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, tex4);
        glUniform1i(glGetUniformLocation(texturedShader, "tex4"), 4);
        
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, tex5);
        glUniform1i(glGetUniformLocation(texturedShader, "tex5"), 5);
        
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, tex6);
        glUniform1i(glGetUniformLocation(texturedShader, "tex6"), 6);
        
        
        glBindVertexArray(vao);
        drawGeometry(texturedShader, startVertCube, numVertsCube, startVertFloor, floorVertexCount, startVertTeapot, numVertsTeapot, startVertKnot, numVertsKnot, objects);
        
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


void drawGeometry(int shaderProgram, int model1_start, int model1_numVerts, int model2_start, int model2_numVerts, int keyStart, int keyNumVerts, int doorStart, int doorNumVerts, const vector<Object> &objects) {
    
    GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
    glm::vec3 colVec(colR,colG,colB);
    glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
    
    GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
    

    //Rotate model (matrix) based on how much time has past
    glm::mat4 model;
   
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    // floor
    model = glm::mat4();    // triangle vertices are already set to their correct locations
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(uniTexID, 1);
    glDrawArrays(GL_TRIANGLES, model2_start, model2_numVerts);
    
    
    // scene items
    for (int i = 0; i < objects.size(); i++) {
        
        switch (objects.at(i).type) {
            case wall:
                model = glm::mat4(); // Load identity
                model = glm::translate(model,glm::vec3(objects.at(i).position.x * cubeScaleValue + 0.5, objects.at(i).position.y * cubeScaleValue + 0.5, 0));
                model = glm::scale(model, cubeScaleValue * glm::vec3(1, 1, 1));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                
                //Set which texture to use (0 = wood texture ... bound to GL_TEXTURE0)
                glUniform1i(uniTexID, 0);
                
                //Draw an instance of the model (at the position & orientation specified by the model matrix above)
                glDrawArrays(GL_TRIANGLES, model1_start, model1_numVerts); //(Primitive Type, Start Vertex, Num Verticies)
                break;
                
            case doorA:
                model = glm::mat4(); // Load identity
                model = glm::translate(model,glm::vec3(objects.at(i).position.x * cubeScaleValue, objects.at(i).position.y * cubeScaleValue, 0));
                model = glm::scale(model, cubeScaleValue * glm::vec3(1, 1, 1));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            
                glUniform1i(uniTexID, 2);
            
                //Draw an instance of the model (at the position & orientation specified by the model matrix above)
                glDrawArrays(GL_TRIANGLES, doorStart, doorNumVerts); //(Primitive Type, Start Vertex, Num Verticies)
                break;
                
            case doorB:
                model = glm::mat4(); // Load identity
                model = glm::translate(model,glm::vec3(objects.at(i).position.x * cubeScaleValue, objects.at(i).position.y * cubeScaleValue, 0));
                model = glm::scale(model, cubeScaleValue * glm::vec3(1, 1, 1));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                
                glUniform1i(uniTexID, 3);
                
                //Draw an instance of the model (at the position & orientation specified by the model matrix above)
                glDrawArrays(GL_TRIANGLES, doorStart, doorNumVerts); //(Primitive Type, Start Vertex, Num Verticies)
                break;
                
            case doorC:
                model = glm::mat4(); // Load identity
                model = glm::translate(model,glm::vec3(objects.at(i).position.x * cubeScaleValue, objects.at(i).position.y * cubeScaleValue, 0));
                model = glm::scale(model, cubeScaleValue * glm::vec3(1, 1, 1));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                
                glUniform1i(uniTexID, 4);
                
                //Draw an instance of the model (at the position & orientation specified by the model matrix above)
                glDrawArrays(GL_TRIANGLES, doorStart, doorNumVerts); //(Primitive Type, Start Vertex, Num Verticies)
                break;
                
            case doorD:
                model = glm::mat4(); // Load identity
                model = glm::translate(model,glm::vec3(objects.at(i).position.x * cubeScaleValue, objects.at(i).position.y * cubeScaleValue, 0));
                model = glm::scale(model, cubeScaleValue * glm::vec3(1, 1, 1));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                
                glUniform1i(uniTexID, 5);
                
                //Draw an instance of the model (at the position & orientation specified by the model matrix above)
                glDrawArrays(GL_TRIANGLES, doorStart, doorNumVerts); //(Primitive Type, Start Vertex, Num Verticies)
                break;
                
            case doorE:
                model = glm::mat4(); // Load identity
                model = glm::translate(model,glm::vec3(objects.at(i).position.x * cubeScaleValue, objects.at(i).position.y * cubeScaleValue, 0));
                model = glm::scale(model, cubeScaleValue * glm::vec3(1, 1, 1));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                
                glUniform1i(uniTexID, 6);
                
                //Draw an instance of the model (at the position & orientation specified by the model matrix above)
                glDrawArrays(GL_TRIANGLES, doorStart, doorNumVerts); //(Primitive Type, Start Vertex, Num Verticies)
                break;
                
            case keya:
                model = glm::mat4(); // Load identity
                model = glm::translate(model,glm::vec3(objects.at(i).position.x * cubeScaleValue, objects.at(i).position.y * cubeScaleValue, 0));
                model = glm::scale(model, cubeScaleValue * glm::vec3(1, 1, 1));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                
                glUniform1i(uniTexID, 2);
                
                //Draw an instance of the model (at the position & orientation specified by the model matrix above)
                glDrawArrays(GL_TRIANGLES, keyStart, keyNumVerts); //(Primitive Type, Start Vertex, Num Verticies)
                break;
                
            case keyb:
                model = glm::mat4(); // Load identity
                model = glm::translate(model,glm::vec3(objects.at(i).position.x * cubeScaleValue, objects.at(i).position.y * cubeScaleValue, 0));
                model = glm::scale(model, cubeScaleValue * glm::vec3(1, 1, 1));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                
                glUniform1i(uniTexID, 3);
                
                //Draw an instance of the model (at the position & orientation specified by the model matrix above)
                glDrawArrays(GL_TRIANGLES, keyStart, keyNumVerts); //(Primitive Type, Start Vertex, Num Verticies)
                
                break;
                
            case keyc:
                model = glm::mat4(); // Load identity
                model = glm::translate(model,glm::vec3(objects.at(i).position.x * cubeScaleValue, objects.at(i).position.y * cubeScaleValue, 0));
                model = glm::scale(model, cubeScaleValue * glm::vec3(1, 1, 1));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                
                glUniform1i(uniTexID, 4);
                
                //Draw an instance of the model (at the position & orientation specified by the model matrix above)
                glDrawArrays(GL_TRIANGLES, keyStart, keyNumVerts); //(Primitive Type, Start Vertex, Num Verticies)
                break;
                
            case keyd:
                model = glm::mat4(); // Load identity
                model = glm::translate(model,glm::vec3(objects.at(i).position.x * cubeScaleValue, objects.at(i).position.y * cubeScaleValue, 0));
                model = glm::scale(model, cubeScaleValue * glm::vec3(1, 1, 1));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                
                glUniform1i(uniTexID, 5);
                
                //Draw an instance of the model (at the position & orientation specified by the model matrix above)
                glDrawArrays(GL_TRIANGLES, keyStart, keyNumVerts); //(Primitive Type, Start Vertex, Num Verticies)
                
                break;
                
            case keye:
                model = glm::mat4(); // Load identity
                model = glm::translate(model,glm::vec3(objects.at(i).position.x * cubeScaleValue, objects.at(i).position.y * cubeScaleValue, 0));
                model = glm::scale(model, cubeScaleValue * glm::vec3(1, 1, 1));
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                
                glUniform1i(uniTexID, 6);
                
                //Draw an instance of the model (at the position & orientation specified by the model matrix above)
                glDrawArrays(GL_TRIANGLES, keyStart, keyNumVerts); //(Primitive Type, Start Vertex, Num Verticies)
                break;
                
            default:
                break;
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


# pragma mark - Game Actions
bool isWalkable(const float newX, const float newY, const float playerRadius, const int mapWidth, const int mapHeight, const vector<Object> &mapObjects) {
    float dxdyValues[] = {-1, 1, 2};
    
    for (int dx = 0; dx < 3; dx++) {
        for (int dy = 0; dy < 3; dy++) {
            float i = floor(newX + playerRadius * dxdyValues[dx]);
            float j = floor(newY + playerRadius * dxdyValues[dy]);
            cout << "i: " << i << endl;
            cout << "j: " << j << endl;
            cout << "newX: " << newX << endl;
            cout << "newY: " << newY << endl;
            cout << "dx: " << dxdyValues[dx] << endl;
            cout << "dy: " << dxdyValues[dy] << endl;
            
            // check the edges
            /*if (i < cubeScaleValue || j < cubeScaleValue || i > mapWidth || j > mapHeight) {
                return false;
            }*/
            
            //cout << "past edges" << endl;
            ObjectType objectAtCurrentPosition = findObjectAtPosition(static_cast<int>(i), static_cast<int>(j), mapObjects);
            cout << "objectAtCurrentPosition: " << objectAtCurrentPosition << endl;
            
            /*if (objectAtCurrentPosition == wall || isDoor(objectAtCurrentPosition)) {
                return false;
            }*/
            cout << "past wall/door" << endl;
        }
    }
    
    return true;
}

