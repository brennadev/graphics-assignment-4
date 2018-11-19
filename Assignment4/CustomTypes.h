//
//  CustomTypes.h
//  Assignment4
//
//  Created by Brenna Olson on 11/9/18.
//  Copyright © 2018 Brenna Olson. All rights reserved.
//

#ifndef CustomTypes_h
#define CustomTypes_h

#include <vector>
using namespace std;

# pragma mark - Type Declarations
/// All possible objects in a scene
enum ObjectType {
    empty,          // nothing displayed at the location
    wall,
    startLocation,
    endLocation,
    doorA,
    doorB,
    doorC,
    doorD,
    doorE,
    keya,
    keyb,
    keyc,
    keyd,
    keye
};


struct Position2D {
    int x;
    int y;
};


struct Object {
    ObjectType type;
    Position2D position;
};


/**
 For use in finding keys using isKey
 */
struct KeyLocation {
    bool isKey;
    Position2D position;
};


# pragma mark - Functions on Objects
/**
 Find the object (if there is one) at a given location in the game's grid

 @param x X position in grid
 @param y Y position in grid
 @param mapObjects All objects in scene
 @return The type of the object at location (x, y). Returns ObjectType case empty if no object is at (x, y).
 */
ObjectType findObjectAtPosition(const int x, const int y, const vector<Object> &mapObjects) {
    for (int i = 0; i < mapObjects.size(); i++) {
        if (mapObjects.at(i).position.x == x && mapObjects.at(i).position.y == y) {
            return mapObjects.at(i).type;
        }
    }
    return empty;
}


/**
 Find the location of a given object in the game's grid

 @param object Object in grid
 @param mapObjects All objects in scene
 @return The location of the object (or first location of the object for object types that have multiple locations) in the scene. If the object type isn't contained in the scene, (-1, -1) is returned.
 */
Position2D findPositionForObject(const ObjectType &object, const vector<Object> &mapObjects) {
    for (int i = 0; i < mapObjects.size(); i++) {
        if (mapObjects.at(i).type == object) {
            return mapObjects.at(i).position;
        }
    }
    
    return {-1, -1};
}


/**
 Determine if a given object in the scene is one of the doors

 @param object Object in grid to check
 @return true if it's a door, false otherwise
 */
bool isDoor(const ObjectType &object) {
    return (object == doorA || object == doorB || object == doorC || object == doorD || object == doorE);
}


/**
 Determine if a given object in the scene is one of the keys and return the location of the key if found in scene

 @param object Object in grid to check
 @param mapObjects All objects in scene
 @return true if a key, false if not a key; key position in position if it's a key and is found in map, {-1, -1} position if not found in map or not a key
 */
KeyLocation isKey(const ObjectType &object, const vector<Object> &mapObjects) {
    KeyLocation returnValue = {false, {-1, -1}};
    
    if (object == keya || object == keyb || object == keyc || object == keyd || object == keye) {
        returnValue.isKey = true;
        returnValue.position = findPositionForObject(object, mapObjects);
    }
    
    return returnValue;
}

#endif /* CustomTypes_h */
