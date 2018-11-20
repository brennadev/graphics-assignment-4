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
#include <iostream>
#include <fstream>
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


/**
 Grid Position (why the values are ints)
 */
struct Position2D {
    int x;
    int y;
};



/**
 Object in the Scene
 */
struct Object {
    ObjectType type;
    Position2D position;
};


/**
 For use in finding keys using isKey - returns an index in a vector
 */
struct KeyLocation {
    bool isKey;
    int location;
};



/**
 For use in finding a potentially matching door using keyMatches
 */
struct DoorLocation {
    bool matchingDoorFound;
    int location;
};


# pragma mark - Functions on Objects
/**
 Find the object (if there is one) at a given location in the game's grid

 @param x X position in grid
 @param y Y position in grid
 @param mapObjects All objects in scene
 @return The type of the object at location (x, y). Returns ObjectType case empty if no object is at (x, y).
 */
ObjectType findObjectAtPosition(const int x, const int y, const vector<Object> &mapObjects);


/**
 Find the location of a given object in the game's grid

 @param object Object in grid
 @param mapObjects All objects in scene
 @return The location of the object (or first location of the object for object types that have multiple locations) in the scene. If the object type isn't contained in the scene, (-1, -1) is returned.
 */
Position2D findPositionForObject(const ObjectType &object, const vector<Object> &mapObjects);


/**
 Find the vector index of the first instance of an object in the map objects

 @param object Object to find
 @param mapObjects All objects in scene
 @return Vector index if item is found in vector; -1 otherwise
 */
int findVectorLocationForObject(const ObjectType &object, const vector<Object> &mapObjects);


/**
 Determine if a given object in the scene is one of the doors

 @param object Object in grid to check
 @return true if it's a door, false otherwise
 */
bool isDoor(const ObjectType &object);


/**
 Determine if a given object in the scene is one of the keys and return the location of the key if found in scene

 @param object Object in grid to check
 @param mapObjects All objects in scene
 @return true if a key, false if not a key; key position in position if it's a key and is found in map, -1 location if not found in map or not a key
 */
KeyLocation isKey(const ObjectType &object, const vector<Object> &mapObjects);


/**
 Read in entire contents of map file

 @param width Variable to assign map width to
 @param height Variable to assign map height to
 @param start Variable to assign map starting point to
 @return All objects in map
 */
vector<Object> readMapFile(int *width, int *height, Object *start);


/**
 See if a given door has its matching key

 @param door Door to check if corresponding key is held
 @param keys All keys currently held
 @param mapObjects All objects in scene
 @return true and door's vector index if corresponding key is held; false and -1 otherwise
 */
DoorLocation keyMatches(const ObjectType &door, const vector<Object> &keys, const vector<Object> &mapObjects);

#endif /* CustomTypes_h */
