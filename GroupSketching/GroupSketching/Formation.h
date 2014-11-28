#pragma once

class Formation
{
public:
	Formation(void);
	Formation(glm::vec3 point);
	Formation(vector<glm::vec3> boundary);
	~Formation(void);

	void populate(int n);
	void populate(Formation* formation);
	void populate(vector<glm::vec3> coords);

	vector<glm::vec3> getCoords();
	vector<glm::vec3> getBoundary();
private:

};

