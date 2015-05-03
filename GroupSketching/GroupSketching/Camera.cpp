#include "stdafx.h"
#include "Camera.h"
#include "Input.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

using namespace std;

void Camera::updatePositions(Input* input) {
	if (input->isKeyPressed('q')) {
		glm::mat4 rotate = glm::rotate(-1.0f, glm::vec3(lookVector));

		rightVector = rotate * rightVector;
		upVector = rotate * upVector;
	}
	if (input->isKeyPressed('e')) {
		glm::mat4 rotate = glm::rotate(1.0f, glm::vec3(lookVector));

		rightVector = rotate * rightVector;
		upVector = rotate * upVector;
	}

	// wasd moves the camera forwards, left, backwards and right
	if (input->isKeyPressed('w')) {
		posVector += stepSize * lookVector;
	}
	if (input->isKeyPressed('s')) {
		posVector -= stepSize * lookVector;
	}
	if (input->isKeyPressed('d')) {
		posVector += stepSize * rightVector;
	}
	if (input->isKeyPressed('a')) {
		posVector -= stepSize * rightVector;
	}
	// Spacebar raises the camera
	if (input->isKeyPressed(' ')) {
		posVector += stepSize * upVector;
	}
	// Control lowers the camera
	if (input->isSpecialKeyPressed(114)) {
		posVector -= stepSize * upVector;
	}
}

void Camera::updatePositionsFixed(Input* input) {
	// wasd moves the camera forwards, left, backwards and right
	if (input->isKeyPressed('w')) {
		posVector += stepSize * glm::vec4(0.0, 0.0, 1.0, 0.0);
	}
	if (input->isKeyPressed('s')) {
		posVector -= stepSize * glm::vec4(0.0, 0.0, 1.0, 0.0);
	}
	if (input->isKeyPressed('d')) {
		posVector += stepSize * glm::vec4(-1.0, 0.0, 0.0, 0.0);
	}
	if (input->isKeyPressed('a')) {
		posVector -= stepSize * glm::vec4(-1.0, 0.0, 0.0, 0.0);
	}
}

Camera::Camera(void) {
	sens = 0.1;
	stepSize = 0.75;

	posVector = glm::vec4(0.0, 15.0, 0.0, 1.0);
	//lookVector = glm::vec4(0.0, -1.0, 0.0, 0.0);
	lookVector = glm::vec4(0.0, -0.75, 0.6, 0.0);
	rightVector = glm::vec4(-1.0, 0.0, 0.0, 0.0);
	upVector = glm::vec4(0.0, 0.0, 1.0, 0.0);
}

void Camera::updateAngle(Input* input) {
	// Get the difference in the x and y mouse coordinates
	int diffX = input->getDiffX();
	int	diffY = input->getDiffY();
	input->setChanged();
	
	float xAngle = sens*diffX;
	float yAngle = sens*diffY;
	
	glm::mat4 xRotation = glm::rotate(xAngle, glm::vec3(upVector));

	lookVector = xRotation * lookVector;
	rightVector = xRotation * rightVector;

	glm::mat4 yRotation = glm::rotate(yAngle, glm::vec3(rightVector));
	lookVector = yRotation * lookVector;
	upVector = yRotation * upVector;
}

void Camera::positionCamera(Input* input) {
	glm::vec4 lookPoint = posVector + lookVector;
	glm::vec4 upPoint = upVector;
	gluLookAt(posVector[0], posVector[1], posVector[2], lookPoint[0], lookPoint[1], lookPoint[2], upPoint[0], upPoint[1], upPoint[2]);
}

void Camera::updateCamera(Input* input) {
	// Update the position variables
	updatePositions(input);
	// Update the viewing angle
	updateAngle(input);
	// Rotate the scene according to the updated values
	positionCamera(input);
}

void Camera::updateFixedCamera(Input* input) {
	updatePositionsFixed(input);
	positionCamera(input);
}


