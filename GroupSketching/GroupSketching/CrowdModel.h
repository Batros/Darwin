#pragma once
#include "Agent.h"
#include "Crowd.h"
#include "Formation.h"
#include "Path.h"

class CrowdModel
{
	vector<Crowd*> crowds;
public:
	CrowdModel(void);
	~CrowdModel(void);
	void createCrowd(Formation* f1, Formation* f2, Path path);
	void createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2);
	void createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, Path path);
	void createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub);
	void createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub, Path path);
	void createCrowd(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub, Path path, Path subPath);
	bool update();

private:
	bool pointInBoundary(glm::vec3 point, vector<glm::vec3> boundary);
	vector<Agent*> freeAgents;
	pair<pair<Formation*, Formation*>, vector<Agent*>> CrowdModel::populateFormations(vector<glm::vec3> bound1, vector<glm::vec3> bound2);
	pair<pair<pair<Formation*, Formation*>, pair<Formation*, Formation*>>, pair<vector<Agent*>, vector<Agent*>>> CrowdModel::populateMainAndSubFormations(vector<glm::vec3> bound1, vector<glm::vec3> bound2, vector<glm::vec3> bound1Sub, vector<glm::vec3> bound2Sub);
};

