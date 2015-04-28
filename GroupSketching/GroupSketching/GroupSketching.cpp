#include "stdafx.h"
#include "Input.h"
#include "Camera.h"
#include "SketchHandler.h"
#include "Formation.h"
#include "CrowdModel.h"
#include "CrowdModelAgentFocus.h"

using namespace std;

int screenWidth;
int screenHeight;

bool drawMode;
bool dragging;
bool running;
bool debugSquares;
bool drawFormations;
bool classicControl;
bool agentFocus;
int strokeNumber;
glm::vec3 sq1 = glm::vec3(20.0, 0.0, 30.0);
glm::vec3 sq2 = glm::vec3(-20.0, 0.0, -10.0);
vector<vector<glm::vec3>> formations;
vector<vector<glm::vec3>> paths;
vector<vector<glm::vec3>> strokes;
vector<vector<float>> colours;

Input* input;
Camera* camera;
CrowdModel* crowdModel;
CrowdModelAgentFocus* crowdModelAgentFocus;
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
	input->onMouseMove(x, y, !drawMode);
}

void onMouseDrag(int x, int y) {
	if (classicControl) {
		if (dragging) {
			if (strokeNumber==0) {
				if (strokes[0].size() > 1) {
					strokes[0][1] = input->onMouseClick(x, y);
				}
				else {
					strokes[0].push_back(input->onMouseClick(x, y));
				}
			}
		}
	}
	else {
		if (dragging) {
			strokes[strokeNumber].push_back(input->onMouseClick(x, y));
		}
	}
}

void processInput() {
	if (classicControl) {
		if (strokes[0].size() > 1) {
			vector<glm::vec3> classicStroke;
			classicStroke.push_back(strokes[0][0]);
			classicStroke.push_back(glm::vec3(strokes[0][0].x, strokes[0][0].y, strokes[0][1].z));
			classicStroke.push_back(glm::vec3(strokes[0][1].x, strokes[0][0].y, strokes[0][1].z));
			classicStroke.push_back(glm::vec3(strokes[0][1].x, strokes[0][0].y, strokes[0][0].z));
			classicStroke.push_back(strokes[0][0]);
		
			vector<glm::vec3> endStroke;
			endStroke.push_back(strokes[1][0]);
			endStroke.push_back(glm::vec3(strokes[1][0].x, strokes[1][0].y, strokes[1][0].z+1));
			endStroke.push_back(glm::vec3(strokes[1][0].x+1, strokes[1][0].y, strokes[1][0].z+1));
			endStroke.push_back(glm::vec3(strokes[1][0].x+1, strokes[1][0].y, strokes[1][0].z));
			endStroke.push_back(strokes[1][0]);

			formations.push_back(classicStroke);
			formations.push_back(endStroke);
			crowdModelAgentFocus->createCrowd(classicStroke, endStroke);
			running = true;
		}
		strokes.clear();
		strokeNumber = 0;
	}
	else {
		vector<int> blankStrokes;
		for (int i=0; i<strokes.size(); i++) {
			if (strokes[i].size()==0) {
				blankStrokes.push_back(i);
			}
		}

		for (int i=blankStrokes.size()-1; i>=0; i--) {
			strokes.erase(strokes.begin()+blankStrokes[i]);
		}
	}
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
		formations.clear();
		vector<glm::vec3> f1 = sketchHandler->processFormation(strokes[0]);
		formations.push_back(f1);
		vector<glm::vec3> f2 = sketchHandler->processFormation(strokes[1]);
		formations.push_back(f2);
		strokes.clear();
		strokeNumber = 0;
		if (agentFocus) {
			crowdModelAgentFocus->createCrowd(f1, f2);
		}
		else {
			crowdModel->createCrowd(f1, f2);
		}
		running = true;
	}
	else if (strokes.size()==3) {
		formations.clear();
		// Formation, Formation, Path
		vector<glm::vec3> f1 = sketchHandler->processFormation(strokes[0]);
		formations.push_back(f1);
		vector<glm::vec3> f2 = sketchHandler->processFormation(strokes[1]);
		formations.push_back(f2);
		Path path = sketchHandler->processPath(strokes[2], f1, f2);
		paths.push_back(path);
		strokes.clear();
		strokeNumber = 0;
		if (path.size() > 1) {
			if (agentFocus) {
				crowdModelAgentFocus->createCrowd(f1, f2, path);
			}
			else {
				crowdModel->createCrowd(f1, f2, path);
			}
			running = true;
		}
		else {
			// Path incorrect
		}
	}
	else if (strokes.size()==4) {
		formations.clear();
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
			if (agentFocus) {
				crowdModelAgentFocus->createCrowd(f1, f2, f1Sub, f2Sub);
			}
			else {
				crowdModel->createCrowd(f1, f2, f1Sub, f2Sub);
			}
			running = true;
		}
	}
	else if (strokes.size()==5) {
		formations.clear();
		// Formation, Sub-formation, Formation, Sub-formation, Path
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


		Path path = sketchHandler->processPath(strokes[4], f1, f2);
		paths.push_back(path);
		if (sub1Inside && sub2Inside) {
			strokes.clear();
			strokeNumber = 0;
			if (agentFocus) {
				crowdModelAgentFocus->createCrowd(f1, f2, f1Sub, f2Sub, path);
			}
			else {
				crowdModel->createCrowd(f1, f2, f1Sub, f2Sub, path);
			}
			running = true;
		}
	}
	else if (strokes.size()==6) {
		formations.clear();
		// Formation, Sub-formation, Formation, Sub-formation, Path, Sub-path
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


		Path path = sketchHandler->processPath(strokes[4], f1, f2);
		Path subPath = sketchHandler->processPath(strokes[5], f1Sub, f2Sub);
		paths.push_back(path);
		paths.push_back(subPath);
		if (sub1Inside && sub2Inside) {
			strokes.clear();
			strokeNumber = 0;
			if (agentFocus) {
				crowdModelAgentFocus->createCrowd(f1, f2, f1Sub, f2Sub, path, subPath);
			}
			else {
				crowdModel->createCrowd(f1, f2, f1Sub, f2Sub, path, subPath);
			}
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
			if (classicControl) {
				newStroke.push_back(input->onMouseClick(x, y));
			}
			strokes.push_back(newStroke);
			dragging = true;
		}
		else if (state == GLUT_UP) {
			if (classicControl) {
				if (strokeNumber > 0) {
					processInput();
				}
				else {
					strokeNumber += 1;
				}
			}
			else {
				strokeNumber += 1;
			}
			dragging = false;
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

			crowdModelAgentFocus->~CrowdModelAgentFocus();
			crowdModelAgentFocus = new CrowdModelAgentFocus();

			formations.clear();
			paths.clear();
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
	if (classicControl) {
		glColor3f(1.0, 1.0, 1.0);
		glLineWidth(5.0f);
		glBegin(GL_LINE_STRIP);
		if (strokes.size() > 0 && strokes[0].size() > 1) {
			glVertex3d(strokes[0][0].x, strokes[0][0].y+0.01, strokes[0][0].z);
			glVertex3d(strokes[0][0].x, strokes[0][0].y+0.01, strokes[0][1].z);
			glVertex3d(strokes[0][1].x, strokes[0][0].y+0.01, strokes[0][1].z);
			glVertex3d(strokes[0][1].x, strokes[0][0].y+0.01, strokes[0][0].z);
			glVertex3d(strokes[0][0].x, strokes[0][0].y+0.01, strokes[0][0].z);
		}
		glEnd();
	}
	else {
		for(vector<glm::vec3>::size_type i = 0; i != strokes.size(); i++) {
			glColor3f(1.0, 1.0, 1.0);
			glLineWidth(5.0f);
			glBegin(GL_LINE_STRIP);
			for(glm::vec3::size_type j = 0; j != strokes[i].size(); j++) {
				glVertex3d(strokes[i][j].x, strokes[i][j].y+0.01, strokes[i][j].z);
			}
			glEnd();
		}
	}

	if (drawFormations) {
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

		for (vector<glm::vec3>::size_type i = 0; i != paths.size(); i++) {
			glBegin(GL_LINE_STRIP);
			for(glm::vec3::size_type j = 0; j != paths[i].size(); j++) {
				glVertex3d(paths[i][j].x, paths[i][j].y+0.01, paths[i][j].z);
			}
			glEnd();
		}
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

	if (input->isKeyPressed('f')) {
		drawFormations = true;
	}
	if (input->isKeyPressed('g')) {
		drawFormations = false;
	}

	if (input->isKeyPressed('c')) {
		classicControl = true;
		formations.clear();
		paths.clear();
		strokes.clear();
		strokeNumber = 0;
	}
	if (input->isKeyPressed('n')) {
		classicControl = false;
		formations.clear();
		paths.clear();
		strokes.clear();
		strokeNumber = 0;
	}
	if (input->isKeyPressed('o')) {
		agentFocus = false;
		formations.clear();
		paths.clear();
		strokes.clear();
		strokeNumber = 0;
	}
	if (input->isKeyPressed('p')) {
		agentFocus = true;
		formations.clear();
		paths.clear();
		strokes.clear();
		strokeNumber = 0;
	}

	if (agentFocus) {
		crowdModelAgentFocus->update();
	}
	else {
		crowdModel->update();
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
	//screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	//screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	screenWidth = 1024;
	screenHeight = 768;

	sketchHandler = new SketchHandler();
	crowdModel = new CrowdModel();
	crowdModelAgentFocus = new CrowdModelAgentFocus();
	input = new Input(screenHeight, screenWidth);
	camera = new Camera();
	agentFocus = false;
	drawMode = false;
	running = false;
	debugSquares = false;
	drawFormations = true;
	classicControl = false;
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

