#ifndef __CAMERA_H_INCLUDED__
#define __CAMERA_H_INCLUDED__

#include "stdafx.h"
#include "Input.h"

class Camera {
	// Position variables denote where the camera is
	double xPos;
	double yPos;
	double zPos;
	// Angle variables denote the rotation of the camera
	double xAng;
	double yAng;
	// Sensitivity represents how much the view changes as the mouse moves
	double sens;
	// stepSize is like sensitivity for movement
	float stepSize;
	// Position that the camera is looking at
	float lookPos[3];

	glm::vec4 posVector;

	glm::vec4 lookVector;
	glm::vec4 rightVector;
	glm::vec4 upVector;

	// updateAngle updates the angle of the camera and where it is facing
	void updateAngle(Input* input);

	// updatePositions updates the position of the camera in the world
	void updatePositions(Input* input);

	public:
		// Default constructor
		Camera(void);

		double getXPos(void);
		double getYPos(void);
		double getZPos(void);
		double getXAng(void);
		double getYAng(void);

		// updateCamera calls the other update methods and draws the changes to the screen
		void updateCamera(Input* input);
		void positionCamera(Input* input);
};

#endif // __CAMERA_H_INCLUDED__