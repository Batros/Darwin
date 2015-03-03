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
	void update(vector<Agent*> neighbours);
	~Agent(void);

private:
	vec3 position;
	vec3 endPoint;
	int minSpd;
	int maxSpd;
	bool needsToMove;
	vec3 colour;
	vec3 separation(vector<vec3>);
	vec3 cohesion(vector<vec3>);
	vec3 pathfind(vec3);
	vec3 getPushedBy(vector<Agent*>);
	float COHESION_STRENGTH;
	float SEPARATION_STRENGTH;
	float PATHFIND_STRENGTH;
};

