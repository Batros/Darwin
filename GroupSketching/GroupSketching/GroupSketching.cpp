#include "stdafx.h"
#include "Input.h"
#include "Camera.h"
#include "SketchHandler.h"
#include "Formation.h"
#include "CrowdModelAgentFocus.h"

using namespace std;

int screenWidth;
int screenHeight;

bool drawMode;
bool dragging;
bool debugSquares;
bool drawFormations;
bool classicControl;
bool agentFocus;
bool freeCamera;
int strokeNumber;
glm::vec3 sq1 = glm::vec3(20.0, 0.0, 30.0);
glm::vec3 sq2 = glm::vec3(-20.0, 0.0, -10.0);
vector<vector<glm::vec3>> formations;
vector<vector<glm::vec3>> paths;
vector<vector<glm::vec3>> strokes;
vector<vector<float>> colours;

Input* input;
Camera* camera;
CrowdModelAgentFocus* crowdModelAgentFocus;
SketchHandler* sketchHandler;

GLuint texture;

enum {
	MENU_RESET = 1,
	MENU_CONTROL_CLASSIC,
	MENU_CONTROL_NEW,
	MENU_SHOWSKETCH_ON,
	MENU_SHOWSKETCH_OFF,
	MENU_QUIT,
	MENU_PROC_INPUT,
	MENU_SEP_LOW,
	MENU_SEP_MED,
	MENU_SEP_HIGH,
	MENU_TEST_1,
	MENU_TEST_2,
	MENU_TEST_3,
	MENU_TEST_4
};

/*
LoadTexture is a modified version of the LoadTexture function found in
the swiftless openGL tutorials online, found here:
http://www.swiftless.com/tutorials/opengl/texture_under_windows.html
*/
GLuint LoadTexture(const char * filename, int width, int height ) {
    GLuint texture;
    FILE *file;

    //The following code will read in our RAW file
    errno_t errorCode = fopen_s(&file, filename, "rb");
    if ( file == NULL ) {
		return 0;
	}

	GLuint* data = new GLuint[width * height];

    fread( data, width * height * sizeof(GLuint), 1, file);
    fclose(file);

    glGenTextures( 1, &texture ); //generate the texture with the loaded data
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameters

    //here we are setting what textures to use and when. The MIN filter is which quality to show
    //when the texture is near the view, and the MAG filter is which quality to show when the texture
    //is far from the view.

    //The qualities are (in order from worst to best)
    //GL_NEAREST
    //GL_LINEAR
    //GL_LINEAR_MIPMAP_NEAREST
    //GL_LINEAR_MIPMAP_LINEAR

    //And if you go and use extensions, you can use Anisotropic filtering textures which are of an
    //even better quality, but this will do for now.
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    //Here we are setting the parameter to repeat the texture instead of clamping the texture
    //to the edge of our shape. 
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    //Generate the texture
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 4); 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete data;
    return texture; //return whether it was successfull
}

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
	if (freeCamera) {
		input->onMouseMove(x, y, !drawMode);
	}
	else {
		input->onMouseMove(x, y, false);
	}
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
			if (strokeNumber<strokes.size()) {
				strokes[strokeNumber].push_back(input->onMouseClick(x, y));
			}
			else {
				strokeNumber = strokes.size()-1;
			}
		}
	}
}

void processInput() {
	if (classicControl) {
		if ((strokes.size() > 1) && (strokes[0].size() > 1)) {
			formations.clear();
			paths.clear();
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
	if (strokes.size()==2) {
		formations.clear();
		paths.clear();
		vector<glm::vec3> f1 = sketchHandler->processFormation(strokes[0]);
		formations.push_back(f1);
		vector<glm::vec3> f2 = sketchHandler->processFormation(strokes[1]);
		formations.push_back(f2);
		strokes.clear();
		strokeNumber = 0;
		crowdModelAgentFocus->createCrowd(f1, f2);
	}
	else if (strokes.size()==3) {
		formations.clear();
		paths.clear();
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
			crowdModelAgentFocus->createCrowd(f1, f2, path);
		}
	}
	else if (strokes.size()==4) {
		formations.clear();
		paths.clear();
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
		//if (sub1Inside && sub2Inside) {
			strokes.clear();
			strokeNumber = 0;
			crowdModelAgentFocus->createCrowd(f1, f2, f1Sub, f2Sub);
		//}
	}
	else if (strokes.size()==5) {
		formations.clear();
		paths.clear();
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
		//if (sub1Inside && sub2Inside) {
			strokes.clear();
			strokeNumber = 0;
			crowdModelAgentFocus->createCrowd(f1, f2, f1Sub, f2Sub, path);
		//}
	}
	else if (strokes.size()==6) {
		formations.clear();
		paths.clear();
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
		//if (sub1Inside && sub2Inside) {
			strokes.clear();
			strokeNumber = 0;
			crowdModelAgentFocus->createCrowd(f1, f2, f1Sub, f2Sub, path, subPath);
		//}
	}
	else {
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

	if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		if (freeCamera) {
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
	}
}

void reset(void) {
	crowdModelAgentFocus->~CrowdModelAgentFocus();
	crowdModelAgentFocus = new CrowdModelAgentFocus();

	formations.clear();
	paths.clear();
	strokes.clear();
	strokeNumber = 0;
}

void drawFloor(float size, int polys) {
	glEnable(GL_TEXTURE_2D);
	texture = LoadTexture("grass.bmp", 600, 600);
	for (float x=0.0f; x<polys; x++) {
		glBegin(GL_TRIANGLE_STRIP);
		glBindTexture(GL_TEXTURE_2D, texture);
		glColor3f(1.0f, 1.0f, 1.0f);
		int texCount = -1;
		for (float z=0.0f; z<=polys; z++) {
			for (float xAdd=0.0f; xAdd<=1.0f; xAdd++) {
				float xCur = 2*((size*(x/polys))+(size*(xAdd/polys))) - size;
				float zCur = 2*(size*(z/polys)) - size;
				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2d(((xCur+size)/(2*size)),((zCur+size)/(2*size)));
				/*
				texCount++;
				if (texCount==0) {
					glTexCoord2d(0.0, 0.0);
				}
				else if (texCount==1) {
					glTexCoord2d(1.0, 0.0);
				}
				else if (texCount==2) {
					glTexCoord2d(0.0, 1.0);
				}
				else if (texCount==3) {
					glTexCoord2d(1.0, 1.0);
					texCount = -1;
				}
				*/
				glVertex3f(xCur, 0.0f, zCur);
			}
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}

void renderEnvironment(void) {
	drawFloor(50.0f, 300);
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

void drawOverlay(void) {
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex2i(0, screenHeight-50);
		glVertex2i(0, screenHeight);
		glVertex2i(screenWidth, screenHeight);
		glVertex2i(screenWidth, screenHeight - 50);
	glEnd();
}

void chooseFromMenu(int opt) {
	switch (opt) {
	case MENU_RESET: 
		reset();
		break;
		
	case MENU_CONTROL_CLASSIC: 
		classicControl = true;
		formations.clear();
		paths.clear();
		strokes.clear();
		strokeNumber = 0;
		dragging = false;
		break;

	case MENU_CONTROL_NEW:
		classicControl = false;
		formations.clear();
		paths.clear();
		strokes.clear();
		strokeNumber = 0;
		dragging = false;
		break;

	case MENU_SHOWSKETCH_ON: 
		drawFormations = true;
		break;
		
	case MENU_SHOWSKETCH_OFF: 
		drawFormations = false;
		break;

	case MENU_QUIT: 
		exit(EXIT_SUCCESS);
		break;

	case MENU_PROC_INPUT:
		processInput();
		break;

	case MENU_SEP_LOW:
		crowdModelAgentFocus->setSepMod(0.40);
		break;
		
	case MENU_SEP_MED:
		crowdModelAgentFocus->setSepMod(1.0);
		break;

	case MENU_SEP_HIGH:
		crowdModelAgentFocus->setSepMod(1.6);
		break;
		
	case MENU_TEST_1:
		crowdModelAgentFocus->startTest(1);
		break;

	case MENU_TEST_2:
		crowdModelAgentFocus->startTest(2);
		break;

	case MENU_TEST_3:
		crowdModelAgentFocus->startTest(3);
		break;

	case MENU_TEST_4:
		crowdModelAgentFocus->startTest(4);
		break;
	}

	glutPostRedisplay();
}

void setupMenu(void) {
	int testMenu = glutCreateMenu(chooseFromMenu);
	glutAddMenuEntry("1", MENU_TEST_1);
	glutAddMenuEntry("2", MENU_TEST_2);
	glutAddMenuEntry("3", MENU_TEST_3);
	glutAddMenuEntry("4", MENU_TEST_4);

	int controlStyleMenu = glutCreateMenu(chooseFromMenu);
	glutAddMenuEntry("Classic", MENU_CONTROL_CLASSIC);
	glutAddMenuEntry("New", MENU_CONTROL_NEW);

	int showSketchesMenu = glutCreateMenu(chooseFromMenu);
	glutAddMenuEntry("On", MENU_SHOWSKETCH_ON);
	glutAddMenuEntry("Off", MENU_SHOWSKETCH_OFF);

	int separationMenu = glutCreateMenu(chooseFromMenu);
	glutAddMenuEntry("Low", MENU_SEP_LOW);
	glutAddMenuEntry("Medium", MENU_SEP_MED);
	glutAddMenuEntry("High", MENU_SEP_HIGH);

	int menu = glutCreateMenu(chooseFromMenu);
	glutAddMenuEntry("Process input", MENU_PROC_INPUT);
	glutAddMenuEntry("Reset", MENU_RESET);
	glutAddSubMenu("Control style", controlStyleMenu);
	glutAddSubMenu("Show sketches", showSketchesMenu);
	glutAddSubMenu("Agent separation", separationMenu);
	glutAddSubMenu("Test cases", testMenu);
	glutAddMenuEntry("Quit program", MENU_QUIT);


	glutAttachMenu(GLUT_RIGHT_BUTTON);
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
	
	if (freeCamera) {
		if (!drawMode) {
			camera->updateCamera(input);
		}
		else {
			camera->positionCamera(input);
		}
	}
	else {
		camera->updateFixedCamera(input);
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

	crowdModelAgentFocus->update();

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
	screenWidth = 1280;
	screenHeight = 1024;

	sketchHandler = new SketchHandler();
	crowdModelAgentFocus = new CrowdModelAgentFocus();
	input = new Input(screenHeight, screenWidth);
	camera = new Camera();
	drawMode = false;
	debugSquares = false;
	drawFormations = false;
	classicControl = false;
	freeCamera = false;
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
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	//glutFullScreen();

	setupMenu();

	input->init();
	glutMainLoop();
	return 0;
}

