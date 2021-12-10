// MovingObject.cpp - Implementation for moving objects around the screen
// Author: Alisa Wallace
#include "MovingObject.h"

MovingObject::MovingObject(std::vector<vec2> targetPositions, std::vector<int> turns, int startIndex, float movementFactor) {
	if (targetPositions.size() < 2) {
		printf("\nMovingObject: error, not enough target positions");
	}

	targets = targetPositions;
	turningPoints = turns;
	currentIndex = startIndex;
	movementRate = movementFactor;

	if (currentIndex < targets.size() - 1) {
		targetIndex = currentIndex + 1;
	}
	else {
		targetIndex = 0;
	}

	updateTarget();
	
	// if fairies don't need to rotate on their first path, start out facing the correct direction
	if (turningPoints.at(currentIndex) == 0) {
		if (totalDistance.x >= 0.0f) {
			rotationY = 40.0f;
		}
		else {
			rotationY = -90.0f;
		}
	}
	// if fairies DO need to rotate in their first path, start them facing the opposite direction
	// so the rotation will set them correctly
	else {
		if (totalDistance.x > 0.0f) {
			rotationY = -90.0f;
		}
		else {
			rotationY = 40.0f;
		}
	}
	 
	
}

void MovingObject::adjustMovement() {
	currentPos += increments;
	distMoved.x += abs(increments.x);
	distMoved.y += abs(increments.y);
	if (turningPoints.at(currentIndex) != 0) {
		rotationY += rotYIncr;
	}

	if (abs(distMoved.x) > abs(totalDistance.x)) {
		incrementIndex();
	}
}

vec3 MovingObject::position() {
	return vec3(currentPos, -5.0f);
}

mat4 MovingObject::translationMatrix() {
	return Translate(currentPos.x, currentPos.y, 0.0f);
}

mat4 MovingObject::scaleMatrix() {
	return Scale(0.1f, 0.1f, 0.1f);
}

mat4 MovingObject::rotationMatrix() {
	return RotateY(rotationY);
}

void MovingObject::incrementIndex() {
	++currentIndex;
	if (currentIndex > targets.size() - 1) {
		currentIndex = 0;
	}

	targetIndex = currentIndex + 1;
	if (targetIndex > targets.size() - 1) {
		targetIndex = 0;
	}

	updateTarget();
}

void MovingObject::updateTarget() {
	currentPos = targets.at(currentIndex);
	movingFrom = currentPos;
	movingTowards = targets.at(targetIndex);
	totalDistance = movingTowards - currentPos;
	
	// get all fairies moving at the same rate
	float pathLength = sqrt((totalDistance.x * totalDistance.x) + (totalDistance.y * totalDistance.y));
	float numCalls = pathLength / movementRate;
	increments.x = totalDistance.x / numCalls;
	increments.y = totalDistance.y / numCalls;

	// determine rotation rate so fairy will rotate completely along the path
	int turningPoint = turningPoints.at(currentIndex);
	float totalRotationNeeded = 130.0f;
	if (turningPoint != 0) {
		rotYIncr = totalRotationNeeded / numCalls;
		if (turningPoint == -1) {
			rotYIncr *= -1.0f;
		}
	}
	
	distMoved = vec2(0.0);
}