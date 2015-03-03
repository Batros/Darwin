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
	void createCrowd(vector<glm::vec3> stroke1, vector<glm::vec3> stroke2, Path path);
	bool update();

private:
	bool pointInBoundary(glm::vec3 point, vector<glm::vec3> boundary);
	vector<Agent*> freeAgents;
};

