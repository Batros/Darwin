/* I declare that this code is my own work and,
except where specified, all code has been written
by me*/
/* Author Thomas Allen
tallen1@sheffield.ac.uk */

/*
The Player class is a camera class adapted from work done during the lab
classes before work started on the assignment
*/

#include "stdafx.h"
#include "Camera.h"
#include "Input.h"
#include "mymaths.h"



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
		// Vector form
		//cout << "posVector: " << posVector[0] << ", " << posVector[1] << ", " << posVector[2] << endl;
		//cout << "lookVector: " << lookVector[0] << ", " << lookVector[1] << ", " << lookVector[2] << endl;
		posVector += stepSize * lookVector;
		/////////////

		//xPos += (double)lookPos[0]*stepSize;
		//yPos += (double)lookPos[1]*stepSize;
		//zPos += (double)lookPos[2]*stepSize;
	}
	if (input->isKeyPressed('s')) {
		// Vector form
		posVector -= stepSize * lookVector;
		/////////////

		//xPos -= (double)lookPos[0]*stepSize;
		//yPos -= (double)lookPos[1]*stepSize;
		//zPos -= (double)lookPos[2]*stepSize;
	}
	if (input->isKeyPressed('d')) {
		// Vector form
		posVector += stepSize * rightVector;
		/////////////

		//xPos += (double)(sin(radsOf(xAng-90)))*stepSize;
		//yPos += (double)(sin(radsOf(yAng-90)))*stepSize;
		//zPos += (double)(cos(radsOf(xAng-90)))*stepSize;
	}
	if (input->isKeyPressed('a')) {
		// Vector form
		posVector -= stepSize * rightVector;
		/////////////

		//xPos -= (double)(sin(radsOf(xAng-90)))*stepSize;
		//yPos -= (double)(sin(radsOf(yAng-90)))*stepSize;
		//zPos -= (double)(cos(radsOf(xAng-90)))*stepSize;
	}
	// Spacebar raises the camera
	if (input->isKeyPressed(' ')) {
		// Vector form
		cout << "posVector: " << posVector[0] << ", " << posVector[1] << ", " << posVector[2] << endl;
		cout << "upVector: " << upVector[0] << ", " << upVector[1] << ", " << upVector[2] << endl;
		
		posVector += stepSize * upVector;
		/////////////
		
		//yPos += stepSize;
	}
	// Control lowers the camera
	if (input->isSpecialKeyPressed(114)) {
		// Vector form
		cout << "posVector: " << posVector[0] << ", " << posVector[1] << ", " << posVector[2] << endl;
		cout << "upVector: " << upVector[0] << ", " << upVector[1] << ", " << upVector[2] << endl;
		
		posVector -= stepSize * upVector;
		/////////////
		
		//yPos -= stepSize;
	}
}

Camera::Camera(void) {
	// Initialise the viewing position
	xPos = 0;
	yPos = 0;
	zPos = 0;
	xAng = 225.0;
	yAng = 0.0;
	sens = 0.1;
	stepSize = 0.5;
	lookPos[0] = 0.0;
	lookPos[1] = 0.0;
	lookPos[2] = 0.0;

	posVector = glm::vec4(0.0, 0.0, 0.0, 1.0);

	lookVector = glm::vec4(0.0, 0.0, 1.0, 0.0);
	rightVector = glm::vec4(-1.0, 0.0, 0.0, 0.0);
	upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);
}

double Camera::getXPos(void) {
	return xPos;
}
double Camera::getYPos(void) {
	return yPos;
}
double Camera::getZPos(void) {
	return zPos;
}
double Camera::getXAng(void) {
	return xAng;
}
double Camera::getYAng(void) {
	return yAng;
}

void Camera::updateAngle(Input* input) {
	// Get the difference in the x and y mouse coordinates
	int diffX = input->getDiffX();
	int	diffY = input->getDiffY();
	input->setChanged();
	if (diffX < -1e4) {
		cout << diffX << endl;
		return;
	}

	//cout << "rotating: " << diffX << ", " << diffY  << endl;
	float xAngle = sens*diffX;
	float yAngle = sens*diffY;

	
	if (diffX > 0) {
		xAngle = 1;
	}
	else if (diffX < 0) {
		xAngle = -1;
	}
	else {
		xAngle = 0;
	}

	if (diffY > 0) {
		yAngle = 1;
	}
	else if (diffY < 0) {
		yAngle = -1;
	}
	else {
		yAngle = 0;
	}
	
	glm::mat4 yRotation = glm::rotate(yAngle, glm::vec3(rightVector));
	glm::mat4 xRotation = glm::rotate(xAngle, glm::vec3(upVector));

	lookVector = xRotation * yRotation * lookVector;
	rightVector = xRotation * rightVector;
	upVector = yRotation * upVector;

	//cout << lookVector[0] << ", " << lookVector[1] << ", " << lookVector[2] << ", " << lookVector[3] << endl;

	///////////////

	/*
	// Update the viewing angle
	xAng += sens*diffX;
	yAng += sens*diffY;

	// Keep the angles within 0 and 360
	if (xAng > 360) {
		xAng -= 360;
	}
	else if (xAng < 0) {
		xAng += 360;
	}

	if (yAng > 360) {
		yAng -= 360;
	}
	else if (yAng < 0) {
		yAng += 360;
	}

	// Calculate the position the camera is looking at
	lookPos[0] = sin(radsOf(xAng));
	lookPos[2] = cos(radsOf(xAng));
	lookPos[1] = sin(radsOf(yAng)); 
	lookPos[1] = 0.0f; // Y-axis looking doesn't work :(
	*/
}

void Camera::positionCamera(Input* input) {
	// Vector form
	glm::vec4 lookPoint = posVector + lookVector;
	glm::vec4 upPoint = upVector;
	gluLookAt(posVector[0], posVector[1], posVector[2], lookPoint[0], lookPoint[1], lookPoint[2], upPoint[0], upPoint[1], upPoint[2]);

	////////////////
	//gluLookAt(xPos, yPos, zPos, 100*lookPos[0], 100*lookPos[1], 100*lookPos[2], 0.0f, 1.0f, 0.0f);
}

void Camera::updateCamera(Input* input) {
	// Update the position variables
	updatePositions(input);
	// Update the viewing angle
	updateAngle(input);
	// Rotate the scene according to the updated values
	positionCamera(input);
}


