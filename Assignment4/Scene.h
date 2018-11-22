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
    Scene();
    void checkForEvents();
    
    
    inline glm::vec3 getCameraPosition() {
        return cameraPosition;
    }
    
    inline float getCameraPositionX() {
        return cameraPosition.x;
    }
    
    inline float getCameraPositionY() {
        return cameraPosition.y;
    }
    
    inline void setCameraPosition(const glm::vec3 &position) {
        cameraPosition = position;
    }
    
    inline void setCameraPositionX(const float &value) {
        cameraPosition.x = value;
    }
    
    inline void setCameraPositionY(const float &value) {
        cameraPosition.y = value;
    }
    
    
    inline glm::vec3 getCameraDirection() {
        return cameraDirection;
    }
    
    inline void setCameraDirection(const glm::vec3 &direction) {
        cameraDirection = direction;
    }
    
    inline void setCameraDirectionX(const float &value) {
        cameraDirection.x = value;
    }
    
    inline void setCameraDirectionY(const float &value) {
        cameraDirection.y = value;
    }
    
    inline float getCameraAngle() {
        return cameraAngle;
    }
    
    /**
     Increment camera angle by 0.1
     */
    inline void incrementCameraAngle() {
        cameraAngle += angleAdjustmentAmount;
    }
    
    /**
     Decrement camera angle by 0.1
     */
    inline void decrementCameraAngle() {
        cameraAngle -= angleAdjustmentAmount;
    }
    
    
    inline Position2D getMapSize() {
        return mapSize;
    }
    
    inline void setMapSize(const Position2D &value) {
        mapSize = value;
    }
    
    inline vector<Object> & getMapObjects() {
        return mapObjects;
    }
    
    inline void setMapObjects(const vector<Object> &objects) {
        mapObjects = objects;
    }
    
    inline vector<Object> & getActiveKeys() {
        return activeKeys;
    }
    
    private:
    glm::vec3 cameraPosition;
    glm::vec3 cameraDirection;
    float cameraAngle;
    float pickupRadius;
    float playerRadius;
    Position2D mapSize;
    vector<Object> mapObjects;
    vector<Object> activeKeys;
    
    float angleAdjustmentAmount;
};

#endif /* Scene_h */
