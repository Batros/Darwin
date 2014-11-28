#pragma once
#include "Formation.h"
#include "Path.h"

class InputHandler
{
public:
	InputHandler(void);
	~InputHandler(void);

	Formation* processFormation(vector<glm::vec3>);
	Path processPath(vector<glm::vec3>);
};

