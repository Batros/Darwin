#pragma once
class Agent
{
public:
	Agent(glm::vec3);
	glm::vec3 getPosition();
	void setPosition(glm::vec3 newPosition);
	void update(vector<Agent*> neighbours, glm::vec3 endPoint);
	~Agent(void);

private:
	glm::vec3 position;
	int minSpd;
	int maxSpd;
};

