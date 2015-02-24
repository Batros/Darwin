#include "stdafx.h"
#include <iostream>
#include "Agent.h"
using namespace std;


Agent::Agent(glm::vec3 position) {
	this->position = position;
	needsToMove = false;
}

Agent::Agent(glm::vec3 position, glm::vec3 end, glm::vec3 colour) {
	this->position = position;
	this->endPoint = end;
	this->colour = colour;
	needsToMove = true;
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
	if (needsToMove) {
		//v1 (done): Do nothing with neighbours, move 1/200th of the way to the end, with minimum and maximum speeds.
		//v2 (in-dev): Calculate inverse-square separation of neighbours, and cohesion vector towards them.
		//v3 (future): Take into account different attributes - have an AgentProperties class or something that has colour, speed etc, so heterogeneity can be implemented
		//v4 (future): Allow for an adjustable weighting to these vectors, and have a pointer to the crowd velocity so agents' velocities can be clamped.
		//v5 (future): Add some kind of rule that takes into account any other restrictions, e.g. the requirement that the agents walk in line with others.
		glm::vec3 sepVec = separation(neighbours);
		glm::vec3 cohVec = cohesion(neighbours);
		glm::vec3 endVec = pathfind(endPoint);
		glm::vec3 modVec = (sepVec+cohVec+endVec)*10.0f;
		glm::vec3 newPos = position+endVec+sepVec;
		setPosition(newPos);
		glPushMatrix();
			glColor3f(colour.x, colour.y, colour.z);
			glTranslated(position.x, position.y, position.z);
			glutSolidSphere(1.0f, 20, 20);
			glLineWidth(2.5);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(modVec.x,0,modVec.z);
			glEnd();
		glPopMatrix();
	}
}

glm::vec3 Agent::separation(vector<glm::vec3> neighbours)
{
	//Return the inverse-square sum of all vectors away from nearby neighbours.
	glm::vec3 out = glm::vec3 (0.0, 0.0, 0.0);
	for (glm::vec3 neighbour : neighbours) {
		glm::vec3 separation = glm::vec3(position.x-neighbour.x, 0, position.z-neighbour.z);
		if (glm::length(separation)<5) {
			//TODO: Deal with divide-by-0 errors
			out += glm::vec3(separation.x, 0, separation.z);
		}
	}
	//TODO: Clamp value to some large-ish maximum (so they won't shoot away at lightspeed)
	if (glm::length(out)>0.1f) {
		out = out*(0.1f/glm::length(out));
	}

	/*if (abs(out.x)>0.001 && abs(out.z)>0.001) {
		return (1.0f/out)*0.0025f;
	} else {
		return glm::vec3(0,0,0);
	}

	if (out.x>0) {
		out.x = 0.1;
	} else {
		out.x = -0.1;
	}
	if (out.z>0) {
		out.z = 0.1;
	} else {
		out.z = -0.1;
	}*/
	return out;
}

glm::vec3 Agent::cohesion(vector<glm::vec3> neighbours)
{
	//Return the sum of all vectors towards neighbours.
	glm::vec3 out = glm::vec3 (0.0, 0.0, 0.0);
	for (glm::vec3 neighbour : neighbours) {
		out += (neighbour-position);
	}
	return out*0.0f;
}

glm::vec3 Agent::pathfind(glm::vec3 endPoint)
{
	//Return a value that points towards the end, but with a length of 0.2.
	return glm::length(endPoint-position)==0 ? glm::vec3(0, 0, 0) : glm::normalize(endPoint-position)*0.2f;
}