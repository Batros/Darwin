#include "stdafx.h"
#include "CrowdModelAgentFocus.h"


CrowdModelAgentFocus::CrowdModelAgentFocus(void)
{
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
		freeAgents.push_back(new AgentFocus(point, point, glm::vec3(0.2, 0.5, 0.9)));
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

	pair<pair<Formation*, Formation*>, vector<AgentFocus*>> package = populateFormations(bound1, bound2);

	Formation* f1 = package.first.first;
	Formation* f2 = package.first.second;
	vector<AgentFocus*> agents = package.second;
	
	//Create a default path that is a single line from one centre to the other (the first formation's centre is not part of the path, so only add the second one)
	Path path;
	path.push_back(f2->getCentre());


	//Create a Crowd with the default path
	CrowdAgentFocus* newCrowd = new CrowdAgentFocus(f1, f2, path, agents);
	
	crowds.push_back(newCrowd);

}

void CrowdModelAgentFocus::createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, Path path) {
	
	pair<pair<Formation*, Formation*>, vector<AgentFocus*>> package = populateFormations(bound1, bound2);

	Formation* f1 = package.first.first;
	Formation* f2 = package.first.second;
	vector<AgentFocus*> agents = package.second;

	//Push the end formation's centre to the end of the path
	path.push_back(f2->getCentre());
	CrowdAgentFocus* newCrowd = new CrowdAgentFocus(f1, f2, path, agents);
	
	crowds.push_back(newCrowd);
}

void CrowdModelAgentFocus::createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub) {
	//First vector - check all free agents and add it to a list if it's within this series of points
	vector<glm::vec3> agentsInBoundary;
	vector<glm::vec3> agentsInSubBoundary;
	vector<AgentFocus*> agents;
	vector<int> agentsToDelete;

	
	glutSolidCube(2);
	//Create formation with the first boundaries. This will contain agents inside the main group, but not the sub-group.
	Formation* f1 = new Formation(bound1, bound1Sub);

	//Then populate it with these agents
	for (int i=0; i<freeAgents.size(); i++) {
		if (pointInBoundary(freeAgents[i]->getPosition(), bound1)) {
			if (pointInBoundary(freeAgents[i]->getPosition(), bound1Sub)) {
				//Add it to a sub-group
				agentsInSubBoundary.push_back(freeAgents[i]->getPosition());
			}
			else {
				agentsInBoundary.push_back(freeAgents[i]->getPosition());
				agents.push_back(freeAgents[i]);
			}
			agentsToDelete.push_back(i);
		}
	}
	f1->populate(agentsInBoundary);

	//Second formation - populate it with the number of agents found in the first one
	Formation* f2 = new Formation(bound2, bound2Sub);
	Formation* f2Sub = new Formation(bound2Sub);
		
	f2->populate(agentsInBoundary.size());
	f2Sub->populate(agentsInSubBoundary.size());

	//Remove all necessary agents from freeAgents
	for (int i=agentsToDelete.size()-1; i>=0; i--) {
		freeAgents.erase(freeAgents.begin()+agentsToDelete[i]);
	}
	
	//Create a default path that is a single line from one centre to the other
	Path path;
	path.push_back(f2->getCentre());

	//Create the crowd with this default path, both formations and the list of agents
	CrowdAgentFocus* newCrowd = new CrowdAgentFocus(f1, f2, path, agents);
	
	crowds.push_back(newCrowd);
}

void CrowdModelAgentFocus::createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub, Path path) {
	//First vector - check all free agents and add it to a list if it's within this series of points
	vector<glm::vec3> agentsInBoundary;
	vector<AgentFocus*> agents;
	vector<int> agentsToDelete;

	
	glutSolidCube(2);
	//Create formation with the first boundaries. This will contain agents inside the main group, but not the sub-group.
	Formation* f1 = new Formation(bound1, bound1Sub);

	//Then populate it with these agents
	for (int i=0; i<freeAgents.size(); i++) {
		if (pointInBoundary(freeAgents[i]->getPosition(), bound1)) {
			if (pointInBoundary(freeAgents[i]->getPosition(), bound1Sub)) {
				//Add it to a sub-group
			} else {
				agentsInBoundary.push_back(freeAgents[i]->getPosition());
				//Modify freeAgents[i] so that its position is relative to the formation's centre (the destination will be made relative later)
				freeAgents[i]->setPosition(freeAgents[i]->getPosition()-f1->getCentre());
				agents.push_back(freeAgents[i]);
				agentsToDelete.push_back(i);
			}
		}
	}
	f1->populate(agentsInBoundary);

	//Second formation - populate it with the number of agents found in the first one
	Formation* f2 = new Formation(bound2, bound2Sub);
		
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

void CrowdModelAgentFocus::createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub, Path path, Path subPath) {
	//First vector - check all free agents and add it to a list if it's within this series of points
	vector<glm::vec3> agentsInBoundary;
	vector<AgentFocus*> agents;
	vector<int> agentsToDelete;

	
	glutSolidCube(2);
	//Create formation with the first boundaries. This will contain agents inside the main group, but not the sub-group.
	Formation* f1 = new Formation(bound1, bound1Sub);

	//Then populate it with these agents
	for (int i=0; i<freeAgents.size(); i++) {
		if (pointInBoundary(freeAgents[i]->getPosition(), bound1)) {
			if (pointInBoundary(freeAgents[i]->getPosition(), bound1Sub)) {
				//Add it to a sub-group
			} else {
				agentsInBoundary.push_back(freeAgents[i]->getPosition());
				//Modify freeAgents[i] so that its position is relative to the formation's centre (the destination will be made relative later)
				freeAgents[i]->setPosition(freeAgents[i]->getPosition()-f1->getCentre());
				agents.push_back(freeAgents[i]);
				agentsToDelete.push_back(i);
			}
		}
	}
	f1->populate(agentsInBoundary);

	//Second formation - populate it with the number of agents found in the first one
	Formation* f2 = new Formation(bound2, bound2Sub);
		
	f2->populate(agentsInBoundary.size());

	//Remove all necessary agents from freeAgents
	for (int i=agentsToDelete.size()-1; i>=0; i--) {
		freeAgents.erase(freeAgents.begin()+agentsToDelete[i]);
	}

	//Create the crowd with a path and sub-path, both formations and the list of agents
	CrowdAgentFocus* newCrowd = new CrowdAgentFocus(f1, f2, path, agents);
	
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
			glutSolidSphere(0.5f, 20, 20);
		glPopMatrix();
	}

	for (int i=0; i<crowds.size(); i++) {
		//v1 (done): No neighbouring crowds.
		//v2 (in-dev): Check all other crowds, see if there are any in the radius. If so, pass these.
		//v3 (future): Convert neighbouring crowds coordinates into this crowd's coordinate system and pass those
		vector <AgentFocus*> neighbouringCrowds;
		crowds[i]->update(neighbouringCrowds);
	}
	return false;
}

bool CrowdModelAgentFocus::pointInBoundary(glm::vec3 point, vector<glm::vec3> boundary) {
	bool inBoundary = false;
	float m_point = 1;
	float c_point = point.z - (m_point * point.x);
	for(vector<glm::vec3>::size_type i = 1; i < boundary.size(); i++) {
		float m_line, c_line;
		m_line = (boundary[i].z - boundary[i-1].z) / (boundary[i].x - boundary[i-1].x);
		c_line = (boundary[i].z - (m_line * boundary[i].x));

		glm::vec3 intersect = glm::vec3(0, 0, 0);

		intersect.x = (c_line - c_point) / (m_point - m_line);
		intersect.z = ((m_point * c_line) - (m_line * c_point)) / (m_point - m_line);
		intersect.y = boundary[i].y;

		// Only look one side of the point
		if (intersect.x > point.x) {
			// Check if intesect point is on the line segment
			if (intersect.x <= boundary[i].x) {
				if (intersect.x >= boundary[i-1].x) {
					inBoundary = !inBoundary;
				}
			}
			else {
				if (intersect.x <= boundary[i-1].x) {
					inBoundary = !inBoundary;
				}
			}
		}
	}
	return inBoundary;
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