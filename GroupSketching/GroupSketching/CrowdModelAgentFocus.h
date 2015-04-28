#pragma once
#include "AgentFocus.h"
#include "CrowdAgentFocus.h"
#include "Formation.h"
#include "Path.h"

class CrowdModelAgentFocus
{
	vector<CrowdAgentFocus*> crowds;
public:
	CrowdModelAgentFocus(void);
	~CrowdModelAgentFocus(void);
	void createCrowd(Formation* f1, Formation* f2, Path path);
	void createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2);
	void createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, Path path);
	void createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub);
	void createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub, Path path);
	void createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub, Path path, Path subPath);
	void setSepMod(float newSepMod);
	bool update();

private:
	float sepMod;
	bool pointInBoundary(glm::vec3 point, vector<glm::vec3> boundary);
	vector<AgentFocus*> freeAgents;
	pair<pair<Formation*, Formation*>, vector<AgentFocus*>> CrowdModelAgentFocus::populateFormations(vector<glm::vec3> bound1, vector<glm::vec3> bound2);
	pair<pair<Formation*, Formation*>, vector<AgentFocus*>> CrowdModelAgentFocus::populateFormationsWithSubformations(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub);
};

