#pragma once
class Agent
{
public:
	Agent(glm::vec3);
	Agent(glm::vec3, glm::vec3);
	glm::vec3 getPosition();
	void setPosition(glm::vec3 newPosition);
	void setEndPoint(glm::vec3 end);
	void update(vector<glm::vec3> neighbours);
	~Agent(void);

private:
	glm::vec3 position;
	glm::vec3 endPoint;
	int minSpd;
	int maxSpd;
};

