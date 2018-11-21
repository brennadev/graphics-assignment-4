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
    pickupRadius = 2.0;
}

void Scene::checkForEvents() {
    
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
            

            // if a door for a held key is found, remove it as it's unlocked
            if (isDoor(type)) {
                cout << "isDoor true" << endl;
                DoorLocation matches = keyMatches(type, activeKeys, mapObjects);
                
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
