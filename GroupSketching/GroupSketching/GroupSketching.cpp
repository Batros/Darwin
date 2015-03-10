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
bool running;
bool debugSquares;
int strokeNumber;
glm::vec3 sq1 = glm::vec3(20.0, 0.0, 30.0);
glm::vec3 sq2 = glm::vec3(-20.0, 0.0, -10.0);
vector<vector<glm::vec3>> formations;
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
	if (strokes.size()==0) {
		return;
		/*
		Formation* f1 = sketchHandler->processFormation(strokes[0]);
		Formation* f2 = sketchHandler->processFormation(strokes[1]);
		Path path = sketchHandler->processPath(strokes[2]);
		*/
		debugSquares = true;

		Formation* f1 = new Formation(sq1);
		Formation* f2 = new Formation(sq2);
		Path path;
		crowdModel->createCrowd(f1, f2, path);
		running = true;
	}
	else if (strokes.size()==2) {
		vector<glm::vec3> f1 = sketchHandler->processFormation(strokes[0]);
		formations.push_back(f1);
		vector<glm::vec3> f2 = sketchHandler->processFormation(strokes[1]);
		formations.push_back(f2);
		Path path;
		strokes.clear();
		strokeNumber = 0;
		crowdModel->createCrowd(f1, f2);
		running = true;
	}
	else if (strokes.size()==3) {
		// Formation, Formation, Path
		vector<glm::vec3> f1 = sketchHandler->processFormation(strokes[0]);
		formations.push_back(f1);
		vector<glm::vec3> f2 = sketchHandler->processFormation(strokes[1]);
		formations.push_back(f2);
		Path path = sketchHandler->processPath(strokes[2], f1, f2);
		strokes.clear();
		strokeNumber = 0;
		if (path.size() > 1) {
			crowdModel->createCrowd(f1, f2, path);
			running = true;
		}
		else {
			// Path incorrect
		}
	}
	else if (strokes.size()==4) {
		// Formation, Sub-formation, Formation, Sub-formation
		vector<glm::vec3> f1 = sketchHandler->processFormation(strokes[0]);
		formations.push_back(f1);
		vector<glm::vec3> f1Sub = sketchHandler->processFormation(strokes[1]);
		formations.push_back(f1Sub);
		bool sub1Inside = sketchHandler->processSubFormation(f1Sub, f1);

		vector<glm::vec3> f2 = sketchHandler->processFormation(strokes[2]);
		formations.push_back(f2);
		vector<glm::vec3> f2Sub = sketchHandler->processFormation(strokes[3]);
		formations.push_back(f2Sub);
		bool sub2Inside = sketchHandler->processSubFormation(f2Sub, f2);

		Path path;
		if (sub1Inside && sub2Inside) {
			strokes.clear();
			strokeNumber = 0;
			crowdModel->createCrowd(f1, f2, f1Sub, f2Sub);
			running = true;
		}
	}
	else if (strokes.size()==5) {
		// Formation, Sub-formation, Formation, Sub-formation, Path
		vector<glm::vec3> f1 = sketchHandler->processFormation(strokes[0]);
		formations.push_back(f1);
		vector<glm::vec3> f1Sub = sketchHandler->processFormation(strokes[1]);
		bool sub1Inside = sketchHandler->processSubFormation(f1Sub, f1);

		vector<glm::vec3> f2 = sketchHandler->processFormation(strokes[2]);
		formations.push_back(f2);
		vector<glm::vec3> f2Sub = sketchHandler->processFormation(strokes[3]);
		bool sub2Inside = sketchHandler->processSubFormation(f2Sub, f2);


		Path path = sketchHandler->processPath(strokes[4], f1, f2);
		if (sub1Inside && sub2Inside) {
			strokes.clear();
			strokeNumber = 0;
			crowdModel->createCrowd(f1, f2, path);
			running = true;
		}
	}
	else if (strokes.size()==6) {
		// Formation, Sub-formation, Formation, Sub-formation, Path, Sub-path
		vector<glm::vec3> f1 = sketchHandler->processFormation(strokes[0]);
		formations.push_back(f1);
		vector<glm::vec3> f1Sub = sketchHandler->processFormation(strokes[1]);
		bool sub1Inside = sketchHandler->processSubFormation(f1Sub, f1);

		vector<glm::vec3> f2 = sketchHandler->processFormation(strokes[2]);
		formations.push_back(f2);
		vector<glm::vec3> f2Sub = sketchHandler->processFormation(strokes[3]);
		bool sub2Inside = sketchHandler->processSubFormation(f2Sub, f2);


		Path path = sketchHandler->processPath(strokes[4], f1, f2);
		Path subPath = sketchHandler->processPath(strokes[5], f1Sub, f2Sub);
		if (sub1Inside && sub2Inside) {
			strokes.clear();
			strokeNumber = 0;
			crowdModel->createCrowd(f1, f2, path);
			running = true;
		}
	}
	else {
		for(vector<glm::vec3>::size_type i = 0; i != strokes.size(); i++) {
			if (strokes[i].size() > 0) {
				vector<glm::vec3> f = sketchHandler->processFormation(strokes[i]);
				formations.push_back(f);
			}
		}
		strokes.clear();
		strokeNumber = 0;
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
		if (state == GLUT_DOWN) {
			crowdModel->~CrowdModel();
			crowdModel = new CrowdModel();

			formations.clear();
			strokes.clear();
			strokeNumber = 0;
			running = false;
		}
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
	for(vector<glm::vec3>::size_type i = 0; i != strokes.size(); i++) {
		glColor3f(1.0, 1.0, 1.0);
		glLineWidth(5.0f);
		glBegin(GL_LINE_STRIP);
		for(glm::vec3::size_type j = 0; j != strokes[i].size(); j++) {
			glVertex3d(strokes[i][j].x, strokes[i][j].y+0.01, strokes[i][j].z);
		}
		glEnd();
	}

	for(vector<glm::vec3>::size_type i = 0; i != formations.size(); i++) {
		glColor3f(0.0, 0.0, 0.0);
		glLineWidth(5.0f);
		glBegin(GL_LINE_STRIP);
		for(glm::vec3::size_type j = 0; j != formations[i].size(); j++) {
			glVertex3d(formations[i][j].x, formations[i][j].y+0.01, formations[i][j].z);
		}
		glVertex3d(formations[i][0].x, formations[i][0].y+0.01, formations[i][0].z);
		glEnd();
	}

	if (debugSquares) {
		glColor3f(1.0f, 1.0f, 1.0f);
		glLineWidth(20.0f);
		glBegin(GL_LINE_STRIP);
		glVertex3d(sq1.x-5.0, sq1.y, sq1.z-5.0);
		glVertex3d(sq1.x-5.0, sq1.y, sq1.z+5.0);
		glVertex3d(sq1.x+5.0, sq1.y, sq1.z+5.0);
		glVertex3d(sq1.x+5.0, sq1.y, sq1.z-5.0);
		glVertex3d(sq1.x-5.0, sq1.y, sq1.z-5.0);
		glEnd();

		glBegin(GL_LINE_STRIP);
		glVertex3d(sq2.x-5.0, sq2.y, sq2.z-5.0);
		glVertex3d(sq2.x-5.0, sq2.y, sq2.z+5.0);
		glVertex3d(sq2.x+5.0, sq2.y, sq2.z+5.0);
		glVertex3d(sq2.x+5.0, sq2.y, sq2.z-5.0);
		glVertex3d(sq2.x-5.0, sq2.y, sq2.z-5.0);
		glEnd();
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

	if (input->isKeyPressed(VK_ESCAPE)) {
		exit(EXIT_SUCCESS);
	}

	crowdModel->update();
	//running = !(crowdModel->update());

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
	//screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	//screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	screenWidth = 1024;
	screenHeight = 768;

	sketchHandler = new SketchHandler();
	crowdModel = new CrowdModel();
	input = new Input(screenHeight, screenWidth);
	camera = new Camera();
	drawMode = false;
	running = false;
	debugSquares = false;
	strokeNumber = 0;
	srand (static_cast <unsigned> (time(0)));

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
	//glutFullScreen();

	input->init();
	glutMainLoop();
	return 0;
}

