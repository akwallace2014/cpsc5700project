// MovingObject.h - Header file for moving objects around the screen
#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H

#include <vector>
#include "glad.h"
#include "VecMat.h"

class MovingObject {
public:
    // Constructor - only accommodates movement in x/y planes, z value is fixed
	MovingObject(std::vector<vec2> targetPositions, std::vector<int> turns, int startIndex = 0, float movementFactor = 0.002);

    // adjusts the object's location coordinates & rotation amount
	void adjustMovement();

    // returns the object's current coordinates as a vec3
    vec3 position();

    mat4 translationMatrix();

    // hardcoded scale values for now
    mat4 scaleMatrix();

    // only rotates along the y-axis for now
    mat4 rotationMatrix();


private:
    // degrees currently rotated along y-axis
    float rotationY;

    // amount to rotate along y-axis with each call to adjustMovement()
    float rotYIncr;

    // factor by which to adjust location coordinates in each call to adjustMovement
    float movementRate;

    // coordinates of target positions for the object to move towards
    std::vector<vec2> targets;

    // whether the location in the matching index of targets represents a turning point for the object
    std::vector<int> turningPoints;

    // indices in targets vector
    int currentIndex;
    int targetIndex; 

    // coordinates of origin - will not change until target updated
    // equal to targets[currentIndex]
    vec2 movingFrom;

    // target coordinates - will not change until target updated
    // equal to targets[targetIndex]
    vec2 movingTowards;

    // total distance needed to move from current location to target location
    vec2 totalDistance;
   
    // current coordinates of the object - will change as movement adjusted
    vec2 currentPos;

    // adjustments made to current coordinates to move them towards the target position
    vec2 increments;

    // total distance current moved from origin location to current location
    vec2 distMoved;

    void incrementIndex();

    void updateTarget();
};
#endif // !MOVINGOBJECT_H
