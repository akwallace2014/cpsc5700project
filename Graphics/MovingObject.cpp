#include "MovingObject.h"

MovingObject::MovingObject(std::vector<vec2> targetPositions, int startIndex, float movementFactor) {
	if (targetPositions.size() < 2) {
		printf("\nMovingObject: error, not enough target positions");
	}

	targets = targetPositions;
	currentIndex = startIndex;
	movementRate = movementFactor;

	if (currentIndex < targets.size() - 1) {
		targetIndex = currentIndex + 1;
	}
	else {
		targetIndex = 0;
	}

	updateTarget();
}

void MovingObject::adjustMovement() {
	currentPos += increments;
	distMoved.x += abs(increments.x);
	distMoved.y += abs(increments.y);

	if (abs(distMoved.x) > abs(totalDistance.x)) {
		incrementIndex();
	}
}

float MovingObject::x() {
	return currentPos.x;
}

float MovingObject::y() {
	return currentPos.y;
}

vec3 MovingObject::position() {
	return vec3(currentPos, -5.0f);
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
	increments = totalDistance * movementRate;
	distMoved = vec2(0.0);
}