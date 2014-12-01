#include "stdafx.h"
#include "Input.h"
#include "Camera.h"
#include "SketchHandler.h"
#include "Formation.h"
#include "CrowdModel.h"

using namespace std;

int screenWidth;
int screenHeight;

bool drawMode;
bool dragging;
bool cubeDrawn;
bool running;
int strokeNumber;
vector<vector<glm::vec3>> strokes;
vector<vector<float>> colours;

Input* input;
Camera* camera;
CrowdModel* crowdModel;
SketchHandler* sketchHandler;

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
		strokes[strokeNumber].push_back(input->onMouseClick(x, y));
	}
}

void processInput() {
	if (sizeof(strokes)==3) {
		Formation* f1 = sketchHandler->processFormation(strokes[0]);
		Formation* f2 = sketchHandler->processFormation(strokes[1]);
		Path path = sketchHandler->processPath(strokes[2]);

		crowdModel->createCrowd(f1, f2, path);
		running = true;
		strokes.empty();
	}
}

void onMouseClick(int button, int state, int x, int y) {
	if(button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			vector<glm::vec3> newStroke;
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
			dragging = false;
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
	}
}

void drawFloor(float size, int polys) {
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

void renderEnvironment(void) {
	drawFloor(40.0f, 300);
	if (cubeDrawn) {
		for(vector<glm::vec3>::size_type i = 0; i != strokes.size(); i++) {
			glColor3f(colours[i][0], colours[i][1], colours[i][2]);
			glLineWidth(20.0f);
			glBegin(GL_LINE_STRIP);
			for(glm::vec3::size_type j = 0; j != strokes[i].size(); j++) {
				glVertex3d(strokes[i][j].x, strokes[i][j].y+0.01, strokes[i][j].z);
			}
			glEnd();
		}
	}
}

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

	if (input->isKeyPressed(VK_RETURN)) {
		processInput();
	}

	if (running) {
		running = !(crowdModel->update());
	}

	renderEnvironment();
	glutSwapBuffers();
}

void reshape(int width, int height) {
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80, (GLfloat)width / (GLfloat)height, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
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
	glutCreateWindow("Group Sketching");
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

