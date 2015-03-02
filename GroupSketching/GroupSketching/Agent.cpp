#include "stdafx.h"
#include <iostream>
#include "Agent.h"
using namespace std;
using namespace glm;


Agent::Agent(vec3 position) {
	this->position = position;
	needsToMove = false;
}

Agent::Agent(vec3 position, vec3 end, vec3 colour) {
	this->position = position;
	this->endPoint = end;
	this->colour = colour;
	needsToMove = true;
}

vec3 Agent::getPosition()
{
	return position;
}

void Agent::setEndPoint(vec3 end) {
	endPoint = end;
}

void Agent::setPosition(vec3 newPosition)
{
	position = newPosition;
}

Agent::~Agent(void)
{
}


void Agent::update(vector<vec3> neighbours)
{
	if (needsToMove) {
		//v1 (done): Do nothing with neighbours, move 1/200th of the way to the end, with minimum and maximum speeds.
		//v2 (done): Calculate inverse-square separation of neighbours, and cohesion vector towards them.
		//v3 (in-dev): Work out if the agent needs to move any more, and stop it if not.
		//v3 (future): Take into account different attributes - have an AgentProperties class or something that has colour, speed etc, so heterogeneity can be implemented
		//v4 (future): Allow for an adjustable weighting to these vectors, and have a pointer to the crowd velocity so agents' velocities can be clamped.
		//v5 (future): Add some kind of rule that takes into account any other restrictions, e.g. the requirement that the agents walk in line with others.
		vec3 sepVec = separation(neighbours);
		vec3 cohVec = cohesion(neighbours);
		vec3 endVec = pathfind(endPoint);
		vec3 modVec = (sepVec+cohVec+endVec)*10.0f;
		vec3 newPos = position+endVec+ cohVec;
		cout << length(endVec) << endl;
		if (length(endVec)<0.01) {
			needsToMove = false;
		}
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
	} else {
		glPushMatrix();
			glColor3f(1-colour.x, 1-colour.y, 1-colour.z);
			glTranslated(position.x, position.y, position.z);
			glutSolidSphere(1.0f, 20, 20);
		glPopMatrix();
	}
}

vec3 Agent::separation(vector<vec3> neighbours)
{
	//Return the inverse-square sum of all vectors away from nearby neighbours.
	vec3 out = vec3 (0.0, 0.0, 0.0);
	for (vec3 neighbour : neighbours) {
		vec3 separation = vec3(position.x-neighbour.x, 0, position.z-neighbour.z);
		if (length(separation)<5) {
			//TODO: Deal with divide-by-0 errors
			out += vec3(separation.x, 0, separation.z);
		}
	}
	//TODO: Clamp value to some large-ish maximum (so they won't shoot away at lightspeed)
	if (length(out)>0.1f) {
		out = out*(0.1f/length(out));
	}

	/*if (abs(out.x)>0.001 && abs(out.z)>0.001) {
		return (1.0f/out)*0.0025f;
	} else {
		return vec3(0,0,0);
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

vec3 Agent::cohesion(vector<vec3> neighbours)
{
	//Return the sum of all vectors towards neighbours.
	vec3 out = vec3 (0.0, 0.0, 0.0);
	for (vec3 neighbour : neighbours) {
		out += (neighbour-position);
	}
	return out*0.0f;
}

vec3 Agent::pathfind(vec3 endPoint)
{
	//Return a value that points towards the end, but with a length of 0.2.
	//Could scale this value to the agent's size.
	vec3 distance = endPoint-position;
	vec3 dstP = normalize(distance)*length(distance);

	//Is the distance to the end less than 3 units?
	//If so, return a vector in the same direction, but 0.1 of the current length.
	//Otherwise, return a vector in the same direction with a length of 0.2.

	return length(distance)<3 ? distance*0.075f : normalize(distance)*0.2f;
}