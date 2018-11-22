//
//  Scene.cpp
//  Assignment4
//
//  Created by Brenna Olson on 11/19/18.
//  Copyright © 2018 Brenna Olson. All rights reserved.
//

#include "Scene.h"

const float dxdyValues[] = {-1.0, 1.0, 2.0};

Scene::Scene() {
    cameraDirection = glm::vec3(1, 0, 0);
    cameraPosition = glm::vec3(3.f, 0.f, 0.f);
    cameraAngle = 0;
    angleAdjustmentAmount = 0.1;
    pickupRadius = 0.2;
}

void Scene::checkForEvents() {
    
    for (int dx = 0; dx < 3; dx++) {
        for (int dy = 0; dy < 3; dy++) {
            
            float i = floor(cameraPosition.x + pickupRadius * dxdyValues[dx]);
            float j = floor(cameraPosition.y + pickupRadius * dxdyValues[dy]);
            
            
            if (i < 1 || j < 1 || i > mapSize.x || j > mapSize.y) {
                continue;
            }
            

            ObjectType type = findObjectAtPosition(i, j, mapObjects);
            KeyLocation potentialKey = isKey(type, mapObjects);
            

            
            // pick up the key if found
            if (potentialKey.isKey) {
                activeKeys.push_back({type, {(int)i, (int)j}});

                // {-1, -1} means it's off the map - as the map will always have walls, the teapots won't be noticeable if in a location that isn't inside the maze
                mapObjects.at(potentialKey.location).position = {-1, -1};
                return;
            }
            
            // if a door for a held key is found, remove it as it's unlocked
            if (isDoor(type)) {
                DoorLocation matches = keyMatches(type, activeKeys, mapObjects);
                
                if (matches.matchingDoorFound) {
                    mapObjects.erase(mapObjects.begin() + matches.location);
                    activeKeys.erase(activeKeys.begin() + matches.activeKeysLocation);
                }
            }
        }
    }
}

