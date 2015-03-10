#pragma once

class Formation
{
	vector<glm::vec3> boundaryCoords;
	vector<glm::vec3> exclusiveBoundaryCoords;
	glm::vec3 centre;

public:
	vector<glm::vec3> agentCoords;
	Formation(void);
	Formation(glm::vec3 point);
	Formation(vector<glm::vec3> boundary);
	Formation(vector<glm::vec3> boundary,vector<glm::vec3> exclusiveBoundary);
	~Formation(void);

	void populate(int n);
	void populate(Formation* formation);
	void populate(vector<glm::vec3> coords);
	vector<glm::vec3> populateBoundary(double stepSize);
	vector<glm::vec3> floodFill(double stepSize, vector<glm::vec3>q);
	vector<glm::vec3> boundingRect(double stepSize);

	vector<glm::vec3> getAgentCoords();
	void setAgentCoords(vector<glm::vec3>);
	vector<glm::vec3> getBoundary();
	void setBoundary(vector<glm::vec3>);
	vector<glm::vec3> getExclusiveBoundary();
	void setExclusiveBoundary(vector<glm::vec3>);
	glm::vec3 getCentre();
	void setCentre(glm::vec3);
private:
	bool pointInBoundary(glm::vec3);
};

