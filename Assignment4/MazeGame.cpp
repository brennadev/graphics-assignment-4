//
//  MazeGame.cpp
//  Assignment4
//
//  Created by Brenna Olson on 11/13/18.
//  Copyright © 2018 Brenna Olson. All rights reserved.
//

# pragma mark - Library Includes
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


# pragma mark - Function Prototypes
/// Read in data from map file
vector<Object> readMapFile(int *width, int *height);


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
    
    
    # pragma mark - Map File Reading
    int width;
    int height;
    vector<Object> objects = readMapFile(&width, &height);
    
    SDL_Event windowEvent;
    bool quit = false;
    while (!quit){
        while (SDL_PollEvent(&windowEvent)){  //inspect all events in the queue
            if (windowEvent.type == SDL_QUIT) quit = true;
            //List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
            //Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
                quit = true; //Exit event loop
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f){ //If "f" is pressed
                isFullScreen = !isFullScreen;
                SDL_SetWindowFullscreen(window, isFullScreen ? SDL_WINDOW_FULLSCREEN : 0); //Toggle fullscreen
            }
            
            //SJG: Use key input to change the state of the object
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
            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_c){ //If "c" is pressed
                //colR = rand01();
                //colG = rand01();
                //colB = rand01();
            }
            
        }
        
        // Clear the screen to default color
        glClearColor(.2f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /*glUseProgram(texturedShader);
        
        
        timePast = SDL_GetTicks()/1000.f;
        
        glm::mat4 view = glm::lookAt(
                                     glm::vec3(3.f, 0.f, 0.f),  //Cam Position
                                     glm::vec3(0.0f, 0.0f, 0.0f),  //Look at point
                                     glm::vec3(0.0f, 0.0f, 1.0f)); //Up
        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        
        glm::mat4 proj = glm::perspective(3.14f/4, screenWidth / (float) screenHeight, 1.0f, 10.0f); //FOV, aspect, near, far
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
        
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0);
        glUniform1i(glGetUniformLocation(texturedShader, "tex0"), 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glUniform1i(glGetUniformLocation(texturedShader, "tex1"), 1);
        
        glBindVertexArray(vao);
        drawGeometry(texturedShader, startVertTeapot, numVertsTeapot, startVertKnot, numVertsKnot);
        */
        SDL_GL_SwapWindow(window); //Double buffering
    }
    
    
    # pragma mark - Cleanup
    SDL_GL_DeleteContext(context);
    SDL_Quit();
    
    return 0;
}


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
