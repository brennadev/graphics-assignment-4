//
//  Scene.h
//  Assignment4
//
//  Created by Brenna Olson on 11/19/18.
//  Copyright © 2018 Brenna Olson. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include <vector>
#include "CustomTypes.h"

#define GLM_FORCE_RADIANS
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Scene {
    public:
    void checkForEvents();
    bool isWalkable(float newX, float newY);
    
    private:
    glm::vec3 cameraPosition;
    glm::vec3 cameraDirection;
    float pickupRadius;
    float playerRadius;
    Position2D mapSize;
    vector<Object> mapObjects;
    vector<Object> activeKeys;
    
    const float dxdyValues[];
    
};

#endif /* Scene_h */
