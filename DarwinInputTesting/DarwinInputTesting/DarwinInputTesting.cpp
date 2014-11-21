// DarwinInputTesting.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Input.h"
#include "Camera.h"
#include "Agent.h"

using namespace std;

int screenWidth;
int screenHeight;

bool drawMode;
bool dragging;
bool cubeDrawn;
int strokeNumber;
GLdouble cubeX;
GLdouble cubeY;
GLdouble cubeZ;
vector<GLdouble> cube;
vector<vector<GLdouble>> cubes;
vector<vector<vector<GLdouble>>> strokes;
vector<vector<float>> colours;

Input* input;
Camera* camera;
vector<Agent> agents;

/*
The keyboard and mouse movement functions just forward on to the input class,
which does all the processing and storing of data
*/
void keyPressed(unsigned char key, int x, int y) {
	input->keyPressed(key, x, y);
}

void keyUp(unsigned char key, int x, int y) {
	input->keyUp(key, x, y);
}

void keySpecial(int key, int x, int y) {
	input->keySpecial(key, x, y);
}

void keySpecialUp(int key, int x, int y) {
	input->keySpecialUp(key, x, y);
}

void onMouseMove(int x, int y) {
	input->onMouseMove(x, y);
}

void onMouseDrag(int x, int y) {
	if (dragging) {
		cubes.push_back(input->onMouseClick(x, y));
		strokes[strokeNumber].push_back(input->onMouseClick(x, y));
	}
}

void onMouseClick(int button, int state, int x, int y) {
	if(button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			//cube = input->onMouseClick(x, y);
			cubes.push_back(input->onMouseClick(x, y));
			vector<vector<GLdouble>> newStroke;
			strokes.push_back(newStroke);

			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			vector<float> newColour;
			newColour.push_back(r);
			newColour.push_back(g);
			newColour.push_back(b);

			colours.push_back(newColour);

			dragging = true;
			cubeDrawn = true;
		}
		else if (state == GLUT_UP) {
			dragging = false;
			strokeNumber += 1;
		}
	}

	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (drawMode) {
			glutSetCursor(GLUT_CURSOR_NONE);
			drawMode = false;
		}
		else {
			glutSetCursor(GLUT_CURSOR_CROSSHAIR);
			drawMode = true;
		}
	}

	if (button == GLUT_MIDDLE_BUTTON) {
		strokes.clear();
		strokeNumber = 0;
		cubes.clear();
	}
}

/*
drawFloorNew is an update to an older, more basic function that drew a plane
at the origin. This function takes a size, acting as a radius for the square plane
and a number representing how many polygons it should be made up of. This allows
for more customisation in terms of image quality which, obviously, directly affects
the quality of the lighting in the scene.
*/
void drawFloorNew(float size, int polys) {
	for (float x=0.0f; x<polys; x++) {
		glBegin(GL_TRIANGLE_STRIP);
		glColor3f(0.1f, 0.4f, 0.1f);
		for (float z=0.0f; z<=polys; z++) {
			for (float xAdd=0.0f; xAdd<=1.0f; xAdd++) {
				float xCur = 2*((size*(x/polys))+(size*(xAdd/polys))) - size;
				float zCur = 2*(size*(z/polys)) - size;
				glNormal3f(0.0f, 1.0f, 0.0f);
				glVertex3f(xCur, 0.0f, zCur);
			}
		}
		glEnd();
	}
}

/*
renderEnvironment is the top-level function for the scene. It makes calls to drawRoom
and drawObstacle in order to draw the room and then position the obstacle inside it.
*/
void renderEnvironment(void) {
	drawFloorNew(40.0f, 300);
	if (cubeDrawn) {
		/*
		glPushMatrix();
			glTranslated(cube[0], cube[1], cube[2]);
			glColor3f(1.0f, 1.0f, 1.0f);
			glutSolidCube(1.0);
		glPopMatrix();
		*/
		/*
		for(std::vector<GLdouble>::size_type i = 0; i != cubes.size(); i++) {
			glPushMatrix();
				glTranslated(cubes[i][0], cubes[i][1], cubes[i][2]);
				glColor3f(1.0f, 0.5f, 0.5f);
				glutSolidCube(0.2f);
			glPopMatrix();
		}
		*/
		for(std::vector<vector<GLdouble>>::size_type i = 0; i != strokes.size(); i++) {
			glColor3f(colours[i][0], colours[i][1], colours[i][2]);
			for(std::vector<GLdouble>::size_type j = 0; j != strokes[i].size(); j++) {
				glPushMatrix();
					glTranslated(strokes[i][j][0], strokes[i][j][1], strokes[i][j][2]);
					glutSolidCube(0.2f);
				glPopMatrix();
			}
		}
	}
}

/*
display is the main update loop function for the program. It is the function that is
called on every frame and ensures that all of the appropriate openGL calls are made
and then renders the environment and positions the camera using the different functions
and classes that are instantiated at the top of the file.
*/
void display(void) {
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT, GL_FILL);
	glLoadIdentity();

	if (!drawMode) {
		camera->updateCamera(input);
	}
	else {
		camera->positionCamera(input);
	}
	
	float r = 0.25;
	float g = 0.45;
	float b = 1.0;

	for(int i = 0; i < agents.size(); i++) {
		glColor3f(r, g, b);
		glPushMatrix();
			glTranslated(agents[i].getX(), agents[i].getY(), agents[i].getZ());
			glutSolidCube(0.8f);
		glPopMatrix();
	}

	renderEnvironment();
	glutSwapBuffers();
}

/*
reshape deals with the window being reshaped. It's just a standard reshape function
*/
void reshape(int width, int height) {
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80, (GLfloat)width / (GLfloat)height, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
	for (int i=0; i<50; i++) {
		int ranX = rand() % 78 - 39, ranY = rand() % 78 - 39;
		cout << ranX << ", " << ranY << endl;
		Agent agt (ranX, 0, ranY);
		agents.push_back(agt);
	}

	glutInit(&argc, argv);
	screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

	input = new Input(screenHeight, screenWidth);
	camera = new Camera();
	drawMode = false;
	cubeDrawn = false;
	strokeNumber = 0;
	srand (static_cast <unsigned> (time(0)));

	input->init();
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize (screenWidth, screenHeight);
	glutInitWindowPosition (0, 0);
	glutCreateWindow("Input Testing");
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(keySpecial);
	glutSpecialUpFunc(keySpecialUp);
	glutPassiveMotionFunc(onMouseMove);
	glutMotionFunc(onMouseDrag);
	glutMouseFunc(onMouseClick);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutFullScreen();

	glutMainLoop();
	return 0;
}

