#pragma once
#include "Path.h"

class SketchHandler
{
public:
	SketchHandler(void);
	~SketchHandler(void);

	bool pointInBoundary(glm::vec3 point, vector<glm::vec3> boundary);
	vector<glm::vec3> processFormation(vector<glm::vec3> stroke);
	bool processSubFormation(vector<glm::vec3> stroke, vector<glm::vec3> parent);
	Path processPath(vector<glm::vec3> stroke, vector<glm::vec3> f1, vector<glm::vec3> f2);
};

