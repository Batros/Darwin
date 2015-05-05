#include "stdafx.h"
#include "CrowdModelAgentFocus.h"


CrowdModelAgentFocus::CrowdModelAgentFocus(void) {
	this->sepMod = 1.0;
	vector<glm::vec3> prevPoints;
	for (int i=0; i<100; i++) {
		float ranX = rand() % 20;
		float ranZ = rand() % 20;
		glm::vec3 point = glm::vec3(ranX, 0, ranZ);

		//While the current randomly-generated point is found in the list already generated
		//Prevents agents being spawned on the same spot
		while (find(prevPoints.begin(), prevPoints.end(), point)!=prevPoints.end()) {
			float ranX = rand() % 20;
			float ranZ = rand() % 20;
			point = glm::vec3(ranX, 0, ranZ);
		}
		prevPoints.push_back(point);
		AgentFocus* newAgent = new AgentFocus(point);
		float ranAgent = rand() % 100;
		if (ranAgent>=20) {
			newAgent->setType(2);
		}
		freeAgents.push_back(newAgent);
	}
}


CrowdModelAgentFocus::~CrowdModelAgentFocus(void)
{
}


/*
LoadTexture is a modified version of the LoadTexture function found in
the swiftless openGL tutorials online, found here:
http://www.swiftless.com/tutorials/opengl/texture_under_windows.html
*/
GLuint CrowdModelAgentFocus::LoadTexture(const char * filename, int width, int height ) {
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

void CrowdModelAgentFocus::createCrowd(Formation* f1, Formation* f2, Path path) {
	vector<AgentFocus*> agents;
	//v1 (done): Use 50 agents
	//v2 (future): Decide number of agents intelligently
	f1->populate(50);
	f2->populate(50);
	CrowdAgentFocus* newCrowd = new CrowdAgentFocus(f1, f2, path);
	crowds.push_back(newCrowd);
	//Need to remove any agents added here from freeAgents
}


void CrowdModelAgentFocus::createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2) {
	vector<vec3> path;
	createCrowd(bound1, bound2, path);
}

void CrowdModelAgentFocus::createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, Path path) {
	//First vector - check all free agents and add it to a list if it's within this series of points
	vector<glm::vec3> agentsInBoundary;
	vector<AgentFocus*> agents;
	vector<int> agentsToDelete;

	//Create formation with the first boundaries. This will contain agents inside the main group, but not the sub-group.
	Formation* f1 = new Formation(bound1);

	//Then populate it with these agents
	for (int i=0; i<freeAgents.size(); i++) {
		if (pointInBoundary(freeAgents[i]->getPosition(), bound1)) {
			agentsInBoundary.push_back(freeAgents[i]->getPosition());
			agents.push_back(freeAgents[i]);
			agentsToDelete.push_back(i);
		}
	}

	for (int i=0; i<crowds.size(); i++) {
		vector<AgentFocus*> crowdAgents = crowds[i]->getAgents();
		vector<int> crowdAgentsToDelete;
		for (int j=0; j<crowdAgents.size(); j++) {
			if (pointInBoundary(crowdAgents[j]->getPosition(), bound1)) {
				agentsInBoundary.push_back(crowdAgents[j]->getPosition());
				agents.push_back(crowdAgents[j]);
				crowdAgentsToDelete.push_back(j);
			}
		}
		crowds[i]->removeAgents(crowdAgentsToDelete);
	}
	f1->populate(agentsInBoundary);

	//Second formation - populate it with the number of agents found in the first one
	Formation* f2 = new Formation(bound2);	

	f2->populate(agentsInBoundary.size());

	//Remove all necessary agents from freeAgents
	for (int i=agentsToDelete.size()-1; i>=0; i--) {
		freeAgents.erase(freeAgents.begin()+agentsToDelete[i]);
	}

	/*
	if (path.size() < 1) {
		path.push_back(f2->getCentre());
	}
	*/

	//Create the crowd with a path and a default sub-path, both formations and the list of agents
	CrowdAgentFocus* newCrowd = new CrowdAgentFocus(f1, f2, path, agents);
	
	crowds.push_back(newCrowd);
}

void CrowdModelAgentFocus::createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub) {
	vector<vec3> path;
	vector<vec3> subPath;
	createCrowd(bound1, bound2, bound1Sub, bound2Sub, path, subPath);
}

void CrowdModelAgentFocus::createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub, Path path) {
	vector<vec3> subPath;
	createCrowd(bound1, bound2, bound1Sub, bound2Sub, path, subPath);
}

void CrowdModelAgentFocus::createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub, Path path, Path subPath) {
	//First vector - check all free agents and add it to a list if it's within this series of points
	vector<glm::vec3> agentsInBoundary;
	vector<glm::vec3> agentsInSubBoundary;
	vector<AgentFocus*> agents;
	vector<AgentFocus*> subAgents;
	vector<int> agentsToDelete;

	//Create formation with the first boundaries. This will contain agents inside the main group, but not the sub-group.
	Formation* f1 = new Formation(bound1, bound1Sub);
	Formation* f1Sub = new Formation(bound1Sub);

	//Then populate it with these agents
	for (int i=0; i<freeAgents.size(); i++) {
		if (pointInBoundary(freeAgents[i]->getPosition(), bound1Sub)) {
			//Add it to a sub-group
			agentsInSubBoundary.push_back(freeAgents[i]->getPosition());
			subAgents.push_back(freeAgents[i]);
			agentsToDelete.push_back(i);
		}
	}

	//Remove all necessary agents from freeAgents
	for (int i=agentsToDelete.size()-1; i>=0; i--) {
		freeAgents.erase(freeAgents.begin()+agentsToDelete[i]);
	}

	agentsToDelete.clear();
	for (int i=0; i<freeAgents.size(); i++) {
		if (pointInBoundary(freeAgents[i]->getPosition(), bound1)) {
			agentsInBoundary.push_back(freeAgents[i]->getPosition());
			agents.push_back(freeAgents[i]);
			agentsToDelete.push_back(i);
		}
	}

	for (int i=agentsToDelete.size()-1; i>=0; i--) {
		freeAgents.erase(freeAgents.begin()+agentsToDelete[i]);
	}

	f1->populate(agentsInBoundary);

	//Second formation - populate it with the number of agents found in the first one
	Formation* f2 = new Formation(bound2, bound2Sub);
	Formation* f2Sub = new Formation(bound2Sub);

	f2->populate(agentsInBoundary.size());

	f2Sub->populate(agentsInSubBoundary.size());

	if (subPath.size()==0) {
		subPath = path;
	}

	//Create the crowd with a path and a default sub-path, both formations and the list of agents
	CrowdAgentFocus* newCrowd = new CrowdAgentFocus(f1, f1Sub, f2, f2Sub, path, subPath, agents, subAgents);
	
	crowds.push_back(newCrowd);
}

void CrowdModelAgentFocus::setSepMod(float newSepMod) {
	this->sepMod = newSepMod;
}

bool CrowdModelAgentFocus::update() {
	GLuint texture = LoadTexture("plate.bmp", 512, 512);

	vector<AgentFocus*> crowdAgents;

	for (int i=0; i<freeAgents.size(); i++) {
		freeAgents[i]->update(crowdAgents, sepMod);
	}

	//crowdAgents = freeAgents;
	for (int i=0; i<crowds.size(); i++) {
		vector<AgentFocus*> agents = crowds[i]->getAgents();
		crowdAgents.insert(crowdAgents.begin(), agents.begin(), agents.end());
	}

	for (int i=0; i<crowds.size(); i++) {
		crowds[i]->update(crowdAgents, sepMod);

		vector<AgentFocus*> stoppedAgents = crowds[i]->getStoppedAgents();
		for (int j=0; j<stoppedAgents.size(); j++) {
			freeAgents.push_back(stoppedAgents[j]);
		}
		crowds[i]->emptyStoppedAgents();
	}


	//crowdAgents.insert(crowdAgents.begin(), freeAgents.begin(), freeAgents.end());
	
	return false;
}

bool CrowdModelAgentFocus::pointInBoundary(glm::vec3 point, vector<glm::vec3> boundary) {
	int boundarySides = boundary.size();
	float boundaryX[1024];
	float boundaryY[1024];
	// Convert x-z space to x-y space
	for (int k = 0; k < boundary.size(); k++) {
		boundaryX[k] = boundary[k].x;
		boundaryY[k] = boundary[k].z;
	}
	int   i, j = boundarySides - 1;
	bool  oddNodes = false;

	float x = point.x;
	float y = point.z;

	for (i = 0; i<boundarySides; i++) {
		if (boundaryY[i]<y && boundaryY[j] >= y
			|| boundaryY[j]<y && boundaryY[i] >= y) {
			if (boundaryX[i] + (y - boundaryY[i]) / (boundaryY[j] - boundaryY[i])*(boundaryX[j] - boundaryX[i])<x) {
				oddNodes = !oddNodes;
			}
		}
		j = i;
	}
	return oddNodes;
}

pair<pair<Formation*, Formation*>, vector<AgentFocus*>>  CrowdModelAgentFocus::populateFormations(vector<glm::vec3> bound1, vector<glm::vec3> bound2) {

	//First vector - check all free agents and add it to a list if it's within this series of points
	vector<glm::vec3> agentsInBoundary;
	vector<AgentFocus*> agents;
	vector<int> agentsToDelete;

	glutSolidCube(2);
	//Create formation with the first boundary
	Formation* f1 = new Formation(bound1);

	//Then populate it with these agents
	for (int i=0; i<freeAgents.size(); i++) {
		if (pointInBoundary(freeAgents[i]->getPosition(), bound1)) {
			agentsInBoundary.push_back(freeAgents[i]->getPosition());
			//Modify freeAgents[i] so that its position is relative to the formation's centre (the destination will be made relative later)
			freeAgents[i]->setPosition(freeAgents[i]->getPosition()-f1->getCentre());
			agents.push_back(freeAgents[i]);
			agentsToDelete.push_back(i);
		}
	}
	f1->populate(agentsInBoundary);

	//Second formation - populate it with the number of agents found in the first one
	Formation* f2 = new Formation(bound2);
		
	f2->populate(agentsInBoundary.size());
	
	//Remove all necessary agents from freeAgents
	for (int i=agentsToDelete.size()-1; i>=0; i--) {
		freeAgents.erase(freeAgents.begin()+agentsToDelete[i]);
	}

	//Construct return values
	pair<Formation*, Formation*> formations(f1, f2);
	pair<pair<Formation*, Formation*>, vector<AgentFocus*>> returnVal(formations, agents);

	return returnVal;
}

void CrowdModelAgentFocus::startTest(int t) {
	freeAgents.clear();
	crowds.clear();
	switch (t) {
	case 1:
	{
		//VIPs protected by bodyguards
		for (int i=0; i<5; i++) {
			glm::vec3 point = glm::vec3(i, 0, 10);

			AgentFocus* newAgent = new AgentFocus(point);
			freeAgents.push_back(newAgent);
		}

		for (int i=0; i<15; i++) {
			for (int j=0; j<4; j++) {
				glm::vec3 point = glm::vec3(i-5, 0, 15+j);
				AgentFocus* newAgent = new AgentFocus(point);
				newAgent->setType(2);
				freeAgents.push_back(newAgent);
			}
		}

		break;
	}
	case 2:
	{
		//Elite troops leading a formation

		float xElite[15] = {2.82, 4.34, 1.68, 2.19, 5.82, 6.15, 3.99, 5.67, 4.77, 0.92, 2.75, 3.50, 0.75, 0.16, 3.25};
		float zElite[15] = {2.05, 6.44, 1.65, 1.07, 5.60, 3.70, 2.95, 3.66, 5.42, 0.02, 3.49, 2.78, 1.78, 0.56, 4.80};

		for (int i=0; i<15; i++) {
			glm::vec3 point = glm::vec3(xElite[i], 0, zElite[i]+10);
			AgentFocus* newAgent = new AgentFocus(point);
			freeAgents.push_back(newAgent);
		}

		float xReg[25] = {6.25, 10.8, 6.31, 6.25, 2.48, 7.31, 4.25, 9.68, 3.92, 9.12, 8.99, 4.25, 1.69, 3.46, 1.37, 11.5, 10.0, 11.9, 2.06, 9.32, 6.32, 11.2, 1.99, 4.11, 5.63};
		float zReg[25] = {8.05, 9.50, 11.3, 0.87, 1.53, 2.79, 1.45, 8.34, 3.68, 3.69, 5.33, 0.10, 5.15, 1.27, 3.33, 2.61, 4.06, 2.01, 1.32, 2.52, 5.42, 11.5, 6.68, 8.24, 1.19};

		for (int i=0; i<25; i++) {
			glm::vec3 point = glm::vec3(xReg[i], 0, zReg[i]);
			AgentFocus* newAgent = new AgentFocus(point);
			newAgent->setType(2);
			freeAgents.push_back(newAgent);
		}

		break;
	}
	case 3:
	{
		//Heavy tanks and light infantry
		float xHeavy[25] = {2.38, 6.17, 0.00, 10.0, 3.89, 6.95, 9.57, 11.5, 6.81, 9.92, 9.94, 8.71, 11.3, 1.12, 6.27, 8.72, 7.30, 8.81, 7.75, 10.0, 0.49, 1.41, 10.5, 6.26, 0.27};
		float zHeavy[25] = {3.30, 3.65, 7.13, 7.09, 8.79, 9.54, 4.75, 1.82, 8.26, 1.74, 8.55, 8.32, 11.6, 3.51, 5.97, 9.24, 0.47, 2.94, 0.98, 3.52, 5.72, 8.70, 10.5, 9.99, 10.7};
		
		for (int i=0; i<25; i++) {
			glm::vec3 point = glm::vec3(xHeavy[i]-5, 0, zHeavy[i]);
			AgentFocus* newAgent = new AgentFocus(point);
			freeAgents.push_back(newAgent);
		}

		float xLight[25] = {5.33, 5.79, 8.57, 3.85, 4.06, 3.87, 3.51, 9.6, 8.53, 3.71, 8.87, 9.0, 5.69, 2.99, 2.62, 7.05, 2.22, 3.05, 8.67, 6.26, 6.86, 9.00, 6.72, 2.23, 6.83};
		float zLight[25] = {3.07, 3.39, 1.05, 8.53, 6.51, 8.59, 10.3, 1.80, 4.84, 9.68, 4.00, 5.38, 7.05, 4.20, 10.3, 7.67, 5.17, 8.75, 0.36, 8.47, 3.98, 6.30, 11.3, 9.54, 0.98};

		for (int i=0; i<25; i++) {
			glm::vec3 point = glm::vec3(xLight[i]+9, 0, zLight[i]);
			AgentFocus* newAgent = new AgentFocus(point);
			newAgent->setType(2);
			freeAgents.push_back(newAgent);
		}

		break;
	}
	case 4:
	{
		//Pincer
		float xs[50] = {7.38, 7.61, 0.83, 0.72, 6.04, 14.5, 1.78, 5.07, 1.72, 8.35, 12.7, 3.21, 17.0, 12.1, 3.54, 12.0, 3.05, 7.13, 8.05, 1.27, 7.41, 10.2, 7.67, 12.8, 5.59,
						12.2, 10.4, 0.57, 2.31, 3.17, 13.2, 1.89, 10.7, 4.29, 14.9, 3.85, 4.48, 5.44, 9.25, 1.04, 8.98, 3.15, 3.66, 8.20, 11.3, 2.00, 1.46, 8.97, 4.58, 14.5};
		float zs[50] = {4.13, 10.2, 0.41, 9.79, 2.75, 12.4, 2.20, 5.42, 11.8, 10.4, 13.8, 5.74, 9.43, 11.2, 8.67, 0.08, 3.85, 10.8, 4.05, 12.8, 1.57, 13.5, 6.21, 4.59, 9.60,
						9.40, 11.6, 4.70, 7.46, 13.03, 8.21, 14.8, 6.03, 1.65, 0.49, 3.50, 8.33, 2.60, 9.84, 2.05, 5.59, 9.63, 6.80, 13.6, 11.0, 10.7, 7.57, 8.40, 6.45, 7.48};

		for (int i=0; i<50; i++) {
			glm::vec3 point = glm::vec3(xs[i], 0, zs[i]);
			AgentFocus* newAgent = new AgentFocus(point);
			freeAgents.push_back(newAgent);
		}

		break;
	}
	}
}