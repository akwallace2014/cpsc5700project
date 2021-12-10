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

float MovingObject::getRotationAmount() {
	const float posTheta = 40.0f;	// amount to rotate around positive y-axis (facing right)
	const float negTheta = -90.0f;	// amount to rotate around negative y-axis (facing left)
	float totalRotation = 120.0f;
	
	float a = totalDistance.x;
	float b = totalDistance.y;
	float c = sqrt((a * a) + (b * b));
	float result = totalRotation * increments.x;
	printf("\nMovingObject: i = %d, totalRotation is %f\n", currentIndex, result);
	return result;
}