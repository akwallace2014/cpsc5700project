#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H

#include <vector>
#include "glad.h"
#include "VecMat.h"

class MovingObject {
public:
    // Constructor
	MovingObject(std::vector<vec2> targetPositions, int startIndex = 0, float movementFactor = 0.0015);

    // adjusts the object's location coordinates 
	void adjustMovement();

    // returns the object's current individual coordinate
    float x();
    float y();

    // returns the object's current coordinates as a vec3
    vec3 position();

private:
    // factor by which to adjust coordinates in each call to adjustMovement
    float movementRate;

    // coordinates of target positions for the object to move towards
    std::vector<vec2> targets;

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
