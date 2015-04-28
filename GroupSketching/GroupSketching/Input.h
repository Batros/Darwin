#ifndef __INPUTDATA_H_INCLUDED__
#define __INPUTDATA_H_INCLUDED__

#include "stdafx.h"

class Input {
	// Arrays to store which keys are pressed/not pressed
	bool keyStates[256];
	bool keySpecialStates[256];

	/*
	Array to store mouse coordinates for current frame, previous frame and a value
	which indicates whether they have changed since the last calculation
	*/
	int mouseCoords[5];

	// Variables to contain the dimensions of the screen
	int screenHeight;
	int screenWidth;

public:
	// Default constructor
	Input(void);
	// Main constructor takes the height and width of the screen for manipulating the mouse cursor
	Input(int height, int width);

	void init(void);

	/*
	These functions are called on keypresses and manage the arrays of key states
	*/
	void keyPressed(unsigned char key, int x, int y);
	void keyUp(unsigned char key, int x, int y);
	void keySpecial(int key, int x, int y);
	void keySpecialUp(int key, int x, int y);

	// This function is called each time the mouse is moved on the screen
	void onMouseMove(int x, int y, bool warp);

	glm::vec3 onMouseClick(int x, int y);

	/*
	getDiffX and getDiffY return the difference in mouse coordinates for the x and y axes.
	These are called by the camera class (Player) to determine how to change the viewing angle
	*/
	int getDiffX(void);
	int getDiffY(void);

	void setChanged();

	/*
	These functions allow other classes to access which keys are pressed at any time without
	having direct access to the arrays that this class uses
	*/
	bool isKeyPressed(unsigned char key) const;
	bool isSpecialKeyPressed(int key) const;
};

#endif // __INPUTDATA_H_INCLUDED