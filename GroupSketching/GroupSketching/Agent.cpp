#include "stdafx.h"
#include <iostream>
#include "Agent.h"
using namespace std;


Agent::Agent(glm::vec3 position) {
	this->position = position;
}

Agent::Agent(glm::vec3 position, glm::vec3 end) {
	this->position = position;
	this->endPoint = end;
}

glm::vec3 Agent::getPosition()
{
	return position;
}

void Agent::setEndPoint(glm::vec3 end) {
	endPoint = end;
}

void Agent::setPosition(glm::vec3 newPosition)
{
	position = newPosition;
}

Agent::~Agent(void)
{
}


void Agent::update(vector<glm::vec3> neighbours)
{
	//Basic: Do nothing with neighbours, move 1/200th of the way to the end, with minimum and maximum speeds.
	//Extended: Calculate inverse-square separation of neighbours, and cohesion vector towards them.
	//glm::vec3 sepVec = separation(neighbours);
	//glm::vec3 cohVec = cohesion(neighbours);
	//glm::vec3 endVec = pathfind(endPoint);
	//glm::vec3 newPos = position+sepVec+cohVec+endVec;
	//glm::vec3 newPos = position+glm::vec3((endPoint.x-position.x)/200, (endPoint.y-position.y)/200, (endPoint.z-position.z)/200);
	//setPosition(newPos);
	glPushMatrix();
		glTranslated(position.x, position.y, position.z);
		glutSolidSphere(1.0f, 20, 20);
	glPopMatrix();
	//cout << min(4, 5);
}

glm::vec3 Agent::separation(vector<glm::vec3> neighbours)
{
	//Return the sum of all vectors with squared-length, away from all neighbours.
	glm::vec3 out = glm::vec3 (0.0, 0.0, 0.0);
	for (glm::vec3 neighbour : neighbours) {
		out += glm::vec3(pow(position.x-neighbour.x, 2.0f), pow(position.y-neighbour.y, 2.0f), pow(position.z-neighbour.z, 2.0f));
	}
	return out*10.0f;
}

glm::vec3 Agent::cohesion(vector<glm::vec3> neighbours)
{
	//Return the sum of all vectors towards neighbours.
	glm::vec3 out = glm::vec3 (0.0, 0.0, 0.0);
	for (glm::vec3 neighbour : neighbours) {
		out += (neighbour-position);
	}
	return out;
}

glm::vec3 Agent::pathfind(glm::vec3 endPoint)
{
	//Return a value that points towards the end, but with a length of 0.2.
	return glm::normalize(endPoint-position)*0.2f;
}