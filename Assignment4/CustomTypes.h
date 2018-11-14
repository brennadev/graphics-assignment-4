//
//  CustomTypes.h
//  Assignment4
//
//  Created by Brenna Olson on 11/9/18.
//  Copyright © 2018 Brenna Olson. All rights reserved.
//

#ifndef CustomTypes_h
#define CustomTypes_h

/// All possible objects in a scene
enum ObjectType {
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

struct Camera {
    
};


#endif /* CustomTypes_h */
