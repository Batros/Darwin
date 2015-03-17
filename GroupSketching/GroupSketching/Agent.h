#pragma once
using namespace glm;
class Agent
{
public:
	Agent(vec3);
	Agent(vec3, vec3, vec3);
	vec3 getPosition();
	vec3 getColour();
	void setPosition(vec3 newPosition);
	void setEndPoint(vec3 end);
	void update(vector<Agent*> neighbours, float urgency);
	~Agent(void);
	bool isStillMoving();

private:
	vec3 position;
	vec3 endPoint;
	int minSpd;
	int maxSpd;
	bool needsToMove;
	vec3 colour;
	vec3 separation(vector<vec3>, vector<float>);
	vec3 pathfind(vec3);
	vec3 randomVec();
	vec3 rndVec;
	vec3 getPushedBy(vector<vec3>);
	float SEPARATION_STRENGTH;
	float PATHFIND_STRENGTH;
	float SIZE;
	vector<vec3> positionsStack;
};

