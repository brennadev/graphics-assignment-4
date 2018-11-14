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
