//
//  Scene.cpp
//  Assignment4
//
//  Created by Brenna Olson on 11/19/18.
//  Copyright © 2018 Brenna Olson. All rights reserved.
//

#include "Scene.h"

Scene::Scene() {
    
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
            
            /*if (i < 1 || j < 1 || i > mapSize.x || j > mapSize.y) {
                continue;
            }*/
            
            
            glm::vec2 toGrid = glm::normalize(glm::vec2(static_cast<float>(i) + 0.5 - cameraPosition.x, static_cast<float>(j) + 0.5 - cameraPosition.y));
            
            float angToGrid = acos(glm::dot(toGrid, cameraDirectionNormalized));
            
            if (abs(angToGrid) > M_PI / 8.0) {
                continue;
            }
            
            ObjectType type = findObjectAtPosition(i, j, mapObjects);
            
            
            KeyLocation potentialKey = isKey(type, mapObjects);
            
            cout << "type: " << type << endl;
            cout << "potentialKey: " << potentialKey.isKey << endl;
            
            if (potentialKey.isKey) {
                cout << "isKey true" << endl;
                activeKeys.push_back({type, {(int)i, (int)j}});
                
                mapObjects.erase(mapObjects.begin() + potentialKey.location);
                
                // move key under floor
                // TODO: not sure how I'll implement considering the location data stored - the only option would be if I set the location to (-1, -1) since it really doesn't matter where underneath the scene the key gets put (meaning the original location isn't needed)
                
                
                // this means it's off the map
                mapObjects.at(potentialKey.location).position = {-1, -1};
                
            } else {
                cout << "isKey false" << endl;
            }
            
            // if a door for a held key is found, remove it as it's unlocked
            if (isDoor(type)) {
                DoorLocation matches = keyMatches(type, activeKeys, mapObjects);
                
                if (matches.matchingDoorFound) {
                    mapObjects.erase(mapObjects.begin() + matches.location);
                }
            }
        }
    }
}


// TODO: fill in (move from MazeGame.cpp)
bool Scene::isWalkable(float newX, float newY) {
    
    return true;
}
