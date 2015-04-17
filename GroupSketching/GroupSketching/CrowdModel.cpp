#include "stdafx.h"
#include "CrowdModel.h"
#include "SketchHandler.h"



CrowdModel::CrowdModel(void)
{
	vector<vec3> prevPoints;
	for (int i=0; i<100; i++) {
		//While the current randomly-generated point is found in the list already generated
		//Prevents agents being spawned on the same spot
		vec3 point;
		do {
			double ranX = rand() % 15*1.5;
			double ranZ = rand() % 15*1.5;
			point = vec3(ranX, 0, ranZ);
		} while (find(prevPoints.begin(), prevPoints.end(), point)!=prevPoints.end());

		prevPoints.push_back(point);
		freeAgents.push_back(new Agent(point, point, vec3(0.2, 0.5, 0.9)));
	}
}


CrowdModel::~CrowdModel(void)
{
}


void CrowdModel::createCrowd(Formation* f1, Formation* f2, Path path) {
	vector<Agent*> agents;
	//v1 (done): Use 50 agents
	//v2 (future): Decide number of agents intelligently
	f1->populate(50);
	f2->populate(50);
	Crowd* newCrowd = new Crowd(f1, f2, path);
	crowds.push_back(newCrowd);
	//Need to remove any agents added here from freeAgents
}


void CrowdModel::createCrowd(vector<vec3> bound1, vector<vec3> bound2) {

	pair<pair<Formation*, Formation*>, vector<Agent*>> package = populateFormations(bound1, bound2);

	Formation* f1 = package.first.first;
	Formation* f2 = package.first.second;
	vector<Agent*> agents = package.second;
	
	//Create a default path that is a single line from one centre to the other (the first formation's centre is not part of the path, so only add the second one)
	Path path;
	path.push_back(f2->getCentre());


	//Create a Crowd with the default path
	Crowd* newCrowd = new Crowd(f1, f2, path, agents);
	
	crowds.push_back(newCrowd);

}

void CrowdModel::createCrowd(vector<vec3> bound1, vector<vec3> bound2, Path path) {
	
	pair<pair<Formation*, Formation*>, vector<Agent*>> package = populateFormations(bound1, bound2);

	Formation* f1 = package.first.first;
	Formation* f2 = package.first.second;
	vector<Agent*> agents = package.second;

	//Push the end formation's centre to the end of the path
	path.push_back(f2->getCentre());

	//Create the crowd with the given path, both 
	Crowd* newCrowd = new Crowd(f1, f2, path, agents);
	
	crowds.push_back(newCrowd);
}

void CrowdModel::createCrowd(vector<vec3> bound1, vector<vec3> bound2, vector<vec3> bound1Sub, vector<vec3> bound2Sub) {

	
	//Debug - ignore all that, use your own boundaries
	//SketchHandler* sketchHandler;
	//vector<vec3> v1 {(30.0f, 0.0f, 30.0f), (30.0f, 0.0f, 0.0f), (0.0f, 0.0f, 0.0f), (0.0f, 0.0f, 30.0f)};
	//bound1 = sketchHandler->processFormation();
	pair<pair<pair<Formation*, Formation*>, pair<Formation*, Formation*>>, pair<vector<Agent*>, vector<Agent*>>> package = populateMainAndSubFormations(bound1, bound2, bound1Sub, bound2Sub);
	
	pair<Formation*, Formation*> formations = package.first.first;
	pair<Formation*, Formation*> subFormations = package.first.second;
	Formation* f1 = formations.first;
	Formation* f2 = formations.second;
	Formation* f1s = subFormations.first;
	Formation* f2s = subFormations.second;
	vector<Agent*> agents = package.second.first;
	vector<Agent*> subAgents = package.second.second;

	//Create a default path that is a single line from one centre to the other
	Path path;
	path.push_back(f2->getCentre());

	//Create a default sub-path that is a single line from one sub-centre to the other.
	Path subPath;
	subPath.push_back(f2s->getCentre());

	//Create the crowd with the default paths as the main and subPaths
	Crowd* newCrowd = new Crowd(f1, f1s, f2, f2s, path, subPath, agents, subAgents);
	
	crowds.push_back(newCrowd);
}

void CrowdModel::createCrowd(vector<vec3> bound1, vector<vec3> bound2, vector<vec3> bound1Sub, vector<vec3> bound2Sub, Path path) {
	
	pair<pair<pair<Formation*, Formation*>, pair<Formation*, Formation*>>, pair<vector<Agent*>, vector<Agent*>>> package = populateMainAndSubFormations(bound1, bound2, bound1Sub, bound2Sub);
	
	pair<Formation*, Formation*> formations = package.first.first;
	pair<Formation*, Formation*> subFormations = package.first.second;
	Formation* f1 = formations.first;
	Formation* f2 = formations.second;
	Formation* f1s = subFormations.first;
	Formation* f2s = subFormations.second;
	vector<Agent*> agents = package.second.first;
	vector<Agent*> subAgents = package.second.second;

	path.push_back(f2->getCentre());

	//Create a default sub-path that is the same as the main path, but with a different end point
	Path subPath;

	for (vec3 point : path) {
		subPath.push_back(point);
	}
	
	subPath[subPath.size()-1] = f2s->getCentre();

	//Create the crowd with the given path and a default sub-path
	Crowd* newCrowd = new Crowd(f1, f1s, f2, f2s, path, subPath, agents, subAgents);
	
	crowds.push_back(newCrowd);
}

void CrowdModel::createCrowd(vector<vec3> bound1, vector<vec3> bound2, vector<vec3> bound1Sub, vector<vec3> bound2Sub, Path path, Path subPath) {
	
	pair<pair<pair<Formation*, Formation*>, pair<Formation*, Formation*>>, pair<vector<Agent*>, vector<Agent*>>> package = populateMainAndSubFormations(bound1, bound2, bound1Sub, bound2Sub);
	
	pair<Formation*, Formation*> formations = package.first.first;
	pair<Formation*, Formation*> subFormations = package.first.second;
	Formation* f1 = formations.first;
	Formation* f2 = formations.second;
	Formation* f1s = subFormations.first;
	Formation* f2s = subFormations.second;
	vector<Agent*> agents = package.second.first;
	vector<Agent*> subAgents = package.second.second;
	
	path.push_back(f2->getCentre());
	subPath.push_back(f2s->getCentre());

	//Create the crowd with the given path and sub-path
	Crowd* newCrowd = new Crowd(f1, f1s, f2, f2s, path, subPath, agents, subAgents);
	
	crowds.push_back(newCrowd);
}

bool CrowdModel::update() {
	//v1 (done): Take a list of crowds at their current state, update them all
	//v2 (in-dev): Calculate radius around each crowd, if any of these overlap there is the potential for collisions so check with that crowd.
	//So, send other crowd for possible separation calculation.
	//Extended: Continue updating until they don't need to move any more, stop at that point.
	
	for (unsigned int i=0; i<freeAgents.size(); i++) {
		vec3 colour = freeAgents[i]->getColour();
		vec3 position = freeAgents[i]->getPosition();
		glPushMatrix();
			glColor3f(colour.x, colour.y, colour.z);
			glTranslated(position.x, position.y, position.z);
			glutSolidSphere(0.5f, 20, 20);
		glPopMatrix();
	}

	for (unsigned int i=0; i<crowds.size(); i++) {
		//v1 (done): No neighbouring crowds.
		//v2 (in-dev): Check all other crowds, see if there are any in the radius. If so, pass these.
		//v3 (future): Convert neighbouring crowds coordinates into this crowd's coordinate system and pass those
		vector <Agent*> neighbouringCrowds;
		crowds[i]->update(neighbouringCrowds);
	}
	return false;
}

bool CrowdModel::pointInBoundary(vec3 point, vector<vec3> boundary) {
	bool inBoundary = false;
	float m_point = 1;
	float c_point = point.z - (m_point * point.x);
	for(vector<vec3>::size_type i = 1; i < boundary.size(); i++) {
		float m_line, c_line;
		m_line = (boundary[i].z - boundary[i-1].z) / (boundary[i].x - boundary[i-1].x);
		c_line = (boundary[i].z - (m_line * boundary[i].x));

		vec3 intersect = vec3(0, 0, 0);

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

pair<pair<Formation*, Formation*>, vector<Agent*>>  CrowdModel::populateFormations(vector<vec3> bound1, vector<vec3> bound2) {

	//First vector - check all free agents and add it to a list if it's within this series of points
	vector<vec3> agentsInBoundary;
	vector<Agent*> agents;
	vector<int> agentsToDelete;

	glutSolidCube(2);
	//Create formation with the first boundary
	Formation* f1 = new Formation(bound1);

	//Then populate it with these agents
	for (unsigned int i=0; i<freeAgents.size(); i++) {
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
	pair<pair<Formation*, Formation*>, vector<Agent*>> returnVal(formations, agents);

	return returnVal;
}

pair<pair<pair<Formation*, Formation*>, pair<Formation*, Formation*>>, pair<vector<Agent*>, vector<Agent*>>>  CrowdModel::populateMainAndSubFormations(vector<vec3> bound1, vector<vec3> bound2, vector<vec3> bound1Sub, vector<vec3> bound2Sub) {
	//First vector - check all free agents and add it to a list if it's within this series of points
	vector<vec3> agentsInBoundary;
	vector<Agent*> agents;
	vector<vec3> agentsInSubBoundary;
	vector<Agent*> subAgents;
	vector<int> agentsToDelete;

	
	glutSolidCube(2);
	//Create formation with the first boundaries. This will contain agents inside the main group, but not the sub-group.
	Formation* f1 = new Formation(bound1, bound1Sub);

	//Then populate it with these agents
	for (unsigned int i=0; i<freeAgents.size(); i++) {
		if (pointInBoundary(freeAgents[i]->getPosition(), bound1)) {
			if (pointInBoundary(freeAgents[i]->getPosition(), bound1Sub)) {
				//Add it to a sub-group
				agentsInSubBoundary.push_back(freeAgents[i]->getPosition());
				subAgents.push_back(freeAgents[i]);
				freeAgents[i]->setColour(vec3(0.2f, 0.7f, 0.7f));
			} else {
				agentsInBoundary.push_back(freeAgents[i]->getPosition());
				agents.push_back(freeAgents[i]);
				freeAgents[i]->setColour(vec3(0.7f, 0.2f, 0.7f));
			}
			//Modify freeAgents[i] so that its position is relative to the formation's centre (the destination will be made relative later)
			freeAgents[i]->setPosition(freeAgents[i]->getPosition()-f1->getCentre());
			agentsToDelete.push_back(i);
		}
	}
	f1->populate(agentsInBoundary);

	Formation* f1s = new Formation(bound1Sub);

	f1s->populate(agentsInSubBoundary);

	//Second formation - populate it with the number of agents found in the first one
	Formation* f2 = new Formation(bound2, bound2Sub);
		
	f2->populate(agentsInBoundary.size());
	
	Formation* f2s = new Formation(bound2Sub);

	f2s->populate(agentsInSubBoundary.size());
	
	//Remove all necessary agents from freeAgents
	for (int i=agentsToDelete.size()-1; i>=0; i--) {
		freeAgents.erase(freeAgents.begin()+agentsToDelete[i]);
	}
	
	//Construct return values
	pair<Formation*, Formation*> formations(f1, f2);
	pair<Formation*, Formation*> subFormations(f1s, f2s);
	pair<pair<Formation*, Formation*>, pair<Formation*, Formation*>> allFormations(formations, subFormations);
	pair<vector<Agent*>, vector<Agent*>> allAgents(agents, subAgents);
	pair<pair<pair<Formation*, Formation*>, pair<Formation*, Formation*>>, pair<vector<Agent*>, vector<Agent*>>> returnVal(allFormations, allAgents);

	return returnVal;
}