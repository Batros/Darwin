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
	vector<AgentFocus*> getAgents();
	vector<AgentFocus*> getStoppedAgents();
	void emptyStoppedAgents();
	void update(vector<AgentFocus*> neighbours);
	void removeAgents(vector<int> agentsToDelete);
	bool isFinished();

private:
	bool finished;
	glm::vec3 centre;
	GLdouble boundRadius;
	vector<AgentFocus*> agents;
	vector<AgentFocus*> subAgents;
	vector<AgentFocus*> stoppedAgents;
	Formation* startFormation;
	Formation* endFormation;
};

