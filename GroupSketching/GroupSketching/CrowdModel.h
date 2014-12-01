#pragma once
#include "Agent.h"
#include "Crowd.h"
#include "Formation.h"
#include "Path.h"

class CrowdModel
{
public:
	CrowdModel(void);
	~CrowdModel(void);
	void createCrowd(Formation* f1, Formation* f2, Path path);
	bool update();

private:
	vector<Crowd*> crowds;
	vector<Agent*> freeAgents;
};

