#pragma once
#include "Formation.h"
#include "Path.h"
#include "Agent.h"

class Crowd
{
public:
	Crowd(void);
	Crowd(Formation* f1, Formation* f2, Path path);
	Crowd(Formation* f1, Formation* f2, Path path, vector<Agent*> agents);
	Crowd(Formation* f1, Formation* f1s, Formation* f2, Formation* f2s, Path path, Path subPath, vector<Agent*> agents, vector<Agent*> subAgents);
	~Crowd(void);
	
	vector<glm::vec3> getRelativeAgentCoords();
	vector<glm::vec3> getAbsoluteAgentCoords();
	void update(vector<Agent*> neighbours);
	bool isFinished();

private:
	bool finished;
	glm::vec3 centre;
	GLdouble boundRadius;
	vector<Agent*> agents;
	vector<Agent*> subAgents;
	Formation* startFormation;
	Formation* endFormation;
	Formation* startSubFormation;
	Formation* endSubFormation;
	Path path;
	Path subPath;
	glm::vec3 currentPosition;
	glm::vec3 currentDestination;
	glm::vec3 pathVec;
	float pathLength;
	float pathLeft;
	glm::vec3 currentSubPosition;
	glm::vec3 currentSubDestination;
	glm::vec3 subPathVec;
	float subPathLength;
	float subPathLeft;
	float urgency;
	double rotation;
};

