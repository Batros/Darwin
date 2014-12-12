#include "stdafx.h"
#include "CrowdModel.h"


CrowdModel::CrowdModel(void)
{
}


CrowdModel::~CrowdModel(void)
{
}


void CrowdModel::createCrowd(Formation* f1, Formation* f2, Path path) {
	vector<Agent> agents;
	//Basic: Use 50 agents
	//Extended: Decide number of agents intelligently
	f1->populate(10);
	f2->populate(10);
	Crowd* newCrowd = new Crowd(f1, f2, path);
	crowds.push_back(newCrowd);
	//Need to remove any agents added here from freeAgents
}


bool CrowdModel::update() {
	//v1 (done): Take a list of crowds at their current state, update them all
	//v2 (in-dev): Calculate radius around each crowd, if any of these overlap there is the potential for collisions so check with that crowd.
	//So, send other crowd for possible separation calculation.
	//Extended: Continue updating until they don't need to move any more, stop at that point.
	for (int i=0; i<crowds.size(); i++) {
		//v1 (done): No neighbouring crowds.
		//v2 (in-dev): Check all other crowds, see if there are any in the radius. If so, pass these.
		//v3 (future): Convert neighbouring crowds coordinates into this crowd's coordinate system and pass those
		vector <glm::vec3> neighbouringCrowds;
		crowds[i]->update(neighbouringCrowds);
	}
	return false;
}