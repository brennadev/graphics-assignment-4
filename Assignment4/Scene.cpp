//
//  Scene.cpp
//  Assignment4
//
//  Created by Brenna Olson on 11/19/18.
//  Copyright © 2018 Brenna Olson. All rights reserved.
//

#include "Scene.h"

Scene::Scene() {
    cameraDirection = glm::vec3(1, 0, 0);
    cameraPosition = glm::vec3(3.f, 0.f, 0.f);
    cameraAngle = 0;
    angleAdjustmentAmount = 0.1;
}

void Scene::checkForEvents() {
    glm::vec2 cameraDirectionNormalized = glm::normalize(glm::vec2(cameraDirection.x, cameraDirection.y));
    
    for (int dx = 0; dx < 3; dx++) {
        for (int dy = 0; dy < 3; dy++) {
            // i/j is always 0 no matter whether it's an int or float; it's because the camera direction and position (and whatever else) haven't been set in Scene - only in the main file
            
            float i = floor(cameraPosition.x + pickupRadius * dxdyValues[dx]);
            float j = floor(cameraPosition.y + pickupRadius * dxdyValues[dy]);
            
            cout << "checkForEvents i: " << i << endl;
            cout << "checkForEvents j: " << j << endl;
            
            if (i < 1 || j < 1 || i > mapSize.x || j > mapSize.y) {
                continue;
            }
            
            cout << "past the edge check" << endl;
            
            glm::vec2 toGrid = glm::normalize(glm::vec2(static_cast<float>(i) + 0.5 - cameraPosition.x, static_cast<float>(j) + 0.5 - cameraPosition.y));
            
            float angToGrid = acos(glm::dot(toGrid, cameraDirectionNormalized));
            
            
            
            /*if (abs(angToGrid) > M_PI / 8.0) {
                continue;
            }*/
            
            cout << "past the abs check" << endl;
            

            ObjectType type = findObjectAtPosition(i, j, mapObjects);
            KeyLocation potentialKey = isKey(type, mapObjects);
            

            
            // pick up the key if found
            if (potentialKey.isKey) {
                activeKeys.push_back({type, {(int)i, (int)j}});

                // {-1, -1} means it's off the map - as the map will always have walls, the teapots won't be noticeable if in a location that isn't inside the maze
                mapObjects.at(potentialKey.location).position = {-1, -1};
                return;
            }
            
            cout << "type: " << type << endl;
            
            ObjectType potentialDoorType;
            
            if (cameraDirectionNormalized.x > cameraDirectionNormalized.y) {
                potentialDoorType = findObjectAtPosition(floor(toGrid.x + cameraDirectionNormalized.x), toGrid.y, mapObjects);
            } else if (cameraDirectionNormalized.y > cameraDirectionNormalized.x) {
                potentialDoorType = findObjectAtPosition(toGrid.x, floor(toGrid.y + cameraDirectionNormalized.y), mapObjects);
            } else {
                potentialDoorType = findObjectAtPosition(floor(toGrid.x + cameraDirectionNormalized.x), floor(toGrid.y + cameraDirectionNormalized.y), mapObjects);
            }
            
            cout << "potentialDoorType: " << potentialDoorType << endl;
            
            // if a door for a held key is found, remove it as it's unlocked
            if (isDoor(potentialDoorType)) {
                cout << "isDoor true" << endl;
                DoorLocation matches = keyMatches(potentialDoorType, activeKeys, mapObjects);
                
                if (matches.matchingDoorFound) {
                    cout << "matching door found" << endl;
                    mapObjects.erase(mapObjects.begin() + matches.location);
                } else {
                    cout << "matching door not found" << endl;
                }
            } else {
                cout << "isDoor false" << endl;
            }
        }
    }
}


// TODO: fill in (move from MazeGame.cpp)
bool Scene::isWalkable(float newX, float newY) {
    
    return true;
}
