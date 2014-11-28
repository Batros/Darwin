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
	~Crowd(void);
	
	vector<glm::vec3> getAgentCoords();
	void update(vector<glm::vec3>);
	bool isFinished();

private:
	bool finished;
	glm::vec3 centre;
	GLdouble boundRadius;
	vector<Agent*> agents;
	Formation* startFormation;
	Formation* endFormation;
	Path path;
};

