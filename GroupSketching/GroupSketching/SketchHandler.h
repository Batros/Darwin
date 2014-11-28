#pragma once
#include "Formation.h"
#include "Path.h"

class SketchHandler
{
public:
	SketchHandler(void);
	~SketchHandler(void);

	Formation* processFormation(vector<glm::vec3>);
	Path processPath(vector<glm::vec3>);
};

