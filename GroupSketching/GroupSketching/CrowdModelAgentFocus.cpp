#include "stdafx.h"
#include "CrowdModelAgentFocus.h"


CrowdModelAgentFocus::CrowdModelAgentFocus(void)
{
	vector<glm::vec3> prevPoints;
	for (int i=0; i<150; i++) {
		float ranX = rand() % 30;
		float ranZ = rand() % 30;
		glm::vec3 point = glm::vec3(ranX+10, 0, ranZ+10);

		//While the current randomly-generated point is found in the list already generated
		//Prevents agents being spawned on the same spot
		while (find(prevPoints.begin(), prevPoints.end(), point)!=prevPoints.end()) {
			float ranX = rand() % 30;
			float ranZ = rand() % 30;
			point = glm::vec3(ranX+10, 0, ranZ+10);
		}
		prevPoints.push_back(point);
		AgentFocus* newAgent = new AgentFocus(point, point, glm::vec3(0.2, 0.5, 0.9));
		if (i>=0) {
			newAgent->setType(2);
		}
		freeAgents.push_back(newAgent);
	}
}


CrowdModelAgentFocus::~CrowdModelAgentFocus(void)
{
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
	f1->populate(agentsInBoundary);

	//Second formation - populate it with the number of agents found in the first one
	Formation* f2 = new Formation(bound2);	

	f2->populate(agentsInBoundary.size());

	//Remove all necessary agents from freeAgents
	for (int i=agentsToDelete.size()-1; i>=0; i--) {
		freeAgents.erase(freeAgents.begin()+agentsToDelete[i]);
	}

	path.push_back(f2->getCentre());

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
		if (pointInBoundary(freeAgents[i]->getPosition(), bound1)) {
			if (pointInBoundary(freeAgents[i]->getPosition(), bound1Sub)) {
				//Add it to a sub-group
				agentsInSubBoundary.push_back(freeAgents[i]->getPosition());
				subAgents.push_back(freeAgents[i]);
				agentsToDelete.push_back(i);
			} else {
				agentsInBoundary.push_back(freeAgents[i]->getPosition());
				agents.push_back(freeAgents[i]);
				agentsToDelete.push_back(i);
			}
		}
	}

	//Remove all necessary agents from freeAgents
	for (int i=agentsToDelete.size()-1; i>=0; i--) {
		freeAgents.erase(freeAgents.begin()+agentsToDelete[i]);
	}

	f1->populate(agentsInBoundary);

	//Second formation - populate it with the number of agents found in the first one
	Formation* f2 = new Formation(bound2, bound2Sub);
	Formation* f2Sub = new Formation(bound2Sub);

	f2->populate(agentsInBoundary.size());
	path.push_back(f2->getCentre());
	
	f2Sub->populate(agentsInSubBoundary.size());
	subPath.push_back(f2Sub->getCentre());

	//Create the crowd with a path and a default sub-path, both formations and the list of agents
	CrowdAgentFocus* newCrowd = new CrowdAgentFocus(f1, f1Sub, f2, f2Sub, path, subPath, agents, subAgents);
	
	crowds.push_back(newCrowd);
}

bool CrowdModelAgentFocus::update() {
	//v1 (done): Take a list of crowds at their current state, update them all
	//v2 (in-dev): Calculate radius around each crowd, if any of these overlap there is the potential for collisions so check with that crowd.
	//So, send other crowd for possible separation calculation.
	//Extended: Continue updating until they don't need to move any more, stop at that point.
	
	for (int i=0; i<freeAgents.size(); i++) {
		glm::vec3 colour = freeAgents[i]->getColour();
		glm::vec3 position = freeAgents[i]->getPosition();
		glPushMatrix();
			glColor3f(colour.x, colour.y, colour.z);
			glTranslated(position.x, position.y, position.z);
			glutSolidSphere(freeAgents[i]->getSize(), 20, 20);
		glPopMatrix();
	}

	for (int i=0; i<crowds.size(); i++) {
		//v1 (done): No neighbouring crowds.
		//v2 (in-dev): Check all other crowds, see if there are any in the radius. If so, pass these.
		//v3 (future): Convert neighbouring crowds coordinates into this crowd's coordinate system and pass those
		vector<AgentFocus*> neighbouringCrowds;
		crowds[i]->update(neighbouringCrowds);
	}
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