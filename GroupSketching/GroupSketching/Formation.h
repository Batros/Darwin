#pragma once

class Formation
{
	vector<glm::vec3> agentCoords;
	vector<glm::vec3> boundaryCoords;
	glm::vec3 centre;

public:
	Formation(void);
	Formation(glm::vec3 point);
	Formation(vector<glm::vec3> boundary);
	~Formation(void);

	void populate(int n);
	void populate(Formation* formation);
	void populate(vector<glm::vec3> coords);
	vector<glm::vec3> populateBoundary(double stepSize);
	vector<glm::vec3> floodFill(double stepSize, vector<glm::vec3>q);

	vector<glm::vec3> getAgentCoords();
	void setAgentCoords(vector<glm::vec3>);
	vector<glm::vec3> getBoundary();
	void setBoundary(vector<glm::vec3>);
	glm::vec3 getCentre();
	void setCentre(glm::vec3);
private:
	bool pointInBoundary(glm::vec3);
};

