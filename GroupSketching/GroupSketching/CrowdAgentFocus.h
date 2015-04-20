#pragma once
#include "Formation.h"
#include "Path.h"
#include "AgentFocus.h"

class CrowdAgentFocus
{
public:
	CrowdAgentFocus(void);
	CrowdAgentFocus(Formation* f1, Formation* f2, Path path);
	CrowdAgentFocus(Formation* f1, Formation* f2, Path path, vector<AgentFocus*> agents);
	CrowdAgentFocus(Formation* f1, Formation* f1s, Formation* f2, Formation* f2s, Path path, Path subPath, vector<AgentFocus*> agents, vector<AgentFocus*> subAgents);
	~CrowdAgentFocus(void);
	
	vector<glm::vec3> getRelativeAgentCoords();
	vector<glm::vec3> getAbsoluteAgentCoords();
	void update(vector<AgentFocus*> neighbours);
	bool isFinished();

private:
	bool finished;
	glm::vec3 centre;
	GLdouble boundRadius;
	vector<AgentFocus*> agents;
	vector<AgentFocus*> subAgents;
	Formation* startFormation;
	Formation* endFormation;
	Path path;
	glm::vec3 currentPosition;
	glm::vec3 currentDestination;
	glm::vec3 pathVec;
	float urgency;
	float pathLength;
	float pathLeft;
	double rotation;
};
