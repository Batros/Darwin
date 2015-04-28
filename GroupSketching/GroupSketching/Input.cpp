#include "stdafx.h"
#include "Input.h"


using namespace std;

Input::Input(void) {
	// If no height and width are given assume 800x600
	Input(800, 600);
	mouseCoords[0] = 0;
	mouseCoords[1] = 0;
	mouseCoords[2] = 0;
	mouseCoords[3] = 0;
	mouseCoords[4] = 0;
}

Input::Input(int height, int width) {
	// Initialise the arrays/variables to false/0
	bool keyStates[256] = {false};
	bool keySpecialStates[256] = {false};

	// Store the height and width of the screen
	screenHeight = height;
	screenWidth = width;
}

void Input::init() {
	// Make super sure that the arrays are initialised to false!
	for (int i=0; i<256; i++) {
		keyStates[i] = false;
		keySpecialStates[i] = false;
	}
	int screenWMiddle = screenWidth>>1;
	int screenHMiddle = screenHeight>>1;
		
	// Set the new mouse coordinates to the middle of the screen and set the update value to 0
	mouseCoords[0] = screenWMiddle;
	mouseCoords[1] = screenWMiddle;
	mouseCoords[2] = screenHMiddle;
	mouseCoords[3] = screenHMiddle;
	mouseCoords[4] = 0;

	// Move the cursor to the middle of the screen
	glutWarpPointer(screenWMiddle, screenHMiddle);
}

glm::vec3 Input::onMouseClick(int x, int y) {
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;
 
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );
 
    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
 
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
 
	glm::vec3 posVec = glm::vec3(posX, 0.01, posZ);
	
	return posVec;
}

void Input::keyPressed(unsigned char key, int x, int y) {
	keyStates[key] = true; // Set the state of the current key to pressed
}

void Input::keyUp(unsigned char key, int x, int y) {
	keyStates[key] = false; // Set the state of the current key to not pressed
}

void Input::keySpecial(int key, int x, int y) {
	keySpecialStates[key] = true; // Set the state of the current key to pressed
}

void Input::keySpecialUp(int key, int x, int y) {
	keySpecialStates[key] = false; // Set the state of the current key to not pressed
}

void Input::onMouseMove(int x, int y, bool warp) {
	
	// If the mouse is near the edge of the screen
	if (warp && ((x > (screenWidth - 30)) || (x < 30) || (y > (screenHeight - 30)) || (y < 30))) {
		// Calculate the middle of the screen
		int screenWMiddle = screenWidth>>1;
		int screenHMiddle = screenHeight>>1;
		
		// Set the new mouse coordinates to the middle of the screen and set the update value to 0
		mouseCoords[0] = screenWMiddle;
		mouseCoords[1] = screenWMiddle;
		mouseCoords[2] = screenHMiddle;
		mouseCoords[3] = screenHMiddle;
		mouseCoords[4] = 0;

		// Move the cursor to the middle of the screen
		glutWarpPointer(screenWMiddle, screenHMiddle);
	}
	else {
		// If the mouse is not near the edge of the screen update normally
		mouseCoords[0] = mouseCoords[1];
		mouseCoords[1] = x;
		mouseCoords[2] = mouseCoords[3];
		mouseCoords[3] = y;
		mouseCoords[4] = 1;
	}
}

void Input::setChanged(void) {
	mouseCoords[4] = 0;
}

int Input::getDiffX(void) {
	// If the mouse coordinates are new calculate the difference in x and return it
	if (mouseCoords[4]==1) {
		int diffX = mouseCoords[0] - mouseCoords[1];
		return diffX;
	}
	// Otherwise return no change
	else {
		return 0;
	}
}

int Input::getDiffY(void) {
	// If the mouse coordinates are new calculate the difference in y and return it
	if (mouseCoords[4]==1) {
		int diffY = mouseCoords[2] - mouseCoords[3];
		return diffY;
	}
	// Otherwise return no change
	else {
		return 0;
	}
}

bool Input::isKeyPressed(unsigned char key) const {
	return keyStates[key];
}

bool Input::isSpecialKeyPressed(int key) const {
	return keySpecialStates[key];
}