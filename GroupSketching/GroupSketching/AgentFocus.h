#pragma once
using namespace glm;
class AgentFocus
{
public:
	AgentFocus(vec3);
	AgentFocus(vec3, vec3, vec3);
	vec3 getEndPoint();
	vec3 getPosition();
	vec3 getColour();
	float getSize();
	float getSpeed();
	void setSpeedLimit(float speed);
	void setPosition(vec3 newPosition);
	void setEndPoint(vec3 end);
	void setPath(vector<vec3> path);
	void setColour(vec3 colour);
	void setType(int t);
	void update(vector<AgentFocus*> neighbours, float sepMod);
	~AgentFocus(void);
	bool isStillMoving();

private:
	vec3 position;
	vec3 endPoint;
	float speedLimit;
	float facing;
	int type;
	float speed;
	bool needsToMove;
	bool pathFound;
	vec3 colour;
	void drawBase();
	void drawTop();
	void drawGun();
	vec3 separation(vector<vec3>, vector<float>);
	vec3 pathfind(vec3);
	vec3 randomVec();
	vec3 rndVec;
	vec3 getPushedBy(vector<vec3>);
	float SEPARATION_STRENGTH;
	float PATHFIND_STRENGTH;
	float SIZE;
	vector<vec3> positionsStack;
	vector<vec3> path;
	vec3 nextPath;
};

