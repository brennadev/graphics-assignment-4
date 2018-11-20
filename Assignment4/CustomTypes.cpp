//
//  CustomTypes.cpp
//  Assignment4
//
//  Created by Brenna Olson on 11/19/18.
//  Copyright © 2018 Brenna Olson. All rights reserved.
//

#include "CustomTypes.h"


ObjectType findObjectAtPosition(const int x, const int y, const vector<Object> &mapObjects) {
    for (int i = 0; i < mapObjects.size(); i++) {
        if (mapObjects.at(i).position.x == x && mapObjects.at(i).position.y == y) {
            return mapObjects.at(i).type;
        }
    }
    return empty;
}


Position2D findPositionForObject(const ObjectType &object, const vector<Object> &mapObjects) {
    for (int i = 0; i < mapObjects.size(); i++) {
        if (mapObjects.at(i).type == object) {
            return mapObjects.at(i).position;
        }
    }
    
    return {-1, -1};
}


int findVectorLocationForObject(const ObjectType &object, const vector<Object> &mapObjects) {
    for (int i = 0; i < mapObjects.size(); i++) {
        if (mapObjects.at(i).type == object) {
            return i;
        }
    }
    return -1;
}


bool isDoor(const ObjectType &object) {
    return (object == doorA || object == doorB || object == doorC || object == doorD || object == doorE);
}


KeyLocation isKey(const ObjectType &object, const vector<Object> &mapObjects) {
    KeyLocation returnValue = {false, -1};
    
    if (object == keya || object == keyb || object == keyc || object == keyd || object == keye) {
        returnValue.isKey = true;
        //returnValue.position = findPositionForObject(object, mapObjects);
        
        for (int i = 0; i < mapObjects.size(); i++) {
            if (mapObjects.at(i).type == object) {
                returnValue.location = i;
                break;
            }
        }
    }
    
    return returnValue;
}


vector<Object> readMapFile(int *width, int *height, Object *start) {
    ifstream mapFile("mapInputbig.txt");
    
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
                    *start = {startLocation, {i, j}};
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


DoorLocation keyMatches(const ObjectType &door, const vector<Object> &keys, const vector<Object> &mapObjects) {
    DoorLocation returnValue = {false, -1};
    
    for (int i = 0; i < keys.size(); i++) {
        switch (keys.at(i).type) {
            case keya:
                if (door == doorA) {
                    returnValue = {true, findVectorLocationForObject(door, mapObjects)};
                }
                break;
                
            case keyb:
                if (door == doorB) {
                    returnValue = {true, findVectorLocationForObject(door, mapObjects)};
                }
                break;
                
            case keyc:
                if (door == doorC) {
                    returnValue = {true, findVectorLocationForObject(door, mapObjects)};
                }
                break;
                
            case keyd:
                if (door == doorD) {
                    returnValue = {true, findVectorLocationForObject(door, mapObjects)};
                }
                break;
                
            case keye:
                if (door == doorE) {
                    returnValue = {true, findVectorLocationForObject(door, mapObjects)};
                }
                break;
                
            default:
                break;
        }
    }
    
    return returnValue;
}
