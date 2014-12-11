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
	f1->populate(2);
	f2->populate(2);
	Crowd* newCrowd = new Crowd(f1, f2, path);
	crowds.push_back(newCrowd);
	//Need to remove any agents added here from freeAgents
}


bool CrowdModel::update() {
	//Basic: Take a list of crowds at their current state, update them all
	//Extended: Calculate radius around each crowd, if any of these overlap there is the potential for collisions.
	//So, send other crowd for possible separation calculation.
	//Extended: Continue updating until they don't need to move any more, stop at that point.
	//cout << crowds.size();
	//cout << "here";
	for (int i=0; i<crowds.size(); i++) {
		//Basic: No neighbouring crowds.
		//Extended: Check all other crowds, see if there are any in the radius. If so, pass these.
		vector <glm::vec3> neighbours;
		crowds[i]->update(neighbours);
	}
	return false;
}