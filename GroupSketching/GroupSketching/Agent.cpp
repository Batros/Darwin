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
	COHESION_STRENGTH = 0.06f;
	SEPARATION_STRENGTH = 0.04f;
	PATHFIND_STRENGTH = 0.2f;
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
		//v3 (in-dev): Work out if the agent needs to move any more, and stop it if not
		//v4 (future): Take into account different attributes - have an AgentProperties class or something that has colour, speed etc, so heterogeneity can be implemented
		//v5 (future): Allow for an adjustable weighting to these vectors, and have a pointer to the crowd velocity so agents' velocities can be clamped.
		//v6 (future): Add some kind of rule that takes into account any other restrictions, e.g. the requirement that the agents walk in line with others.
		vec3 sepVec = separation(neighbours);
		vec3 cohVec = cohesion(neighbours);
		vec3 endVec = pathfind(endPoint);
		vec3 modVec = (sepVec+cohVec+endVec)*10.0f;
		cout << sepVec.x << ", " << sepVec.z << endl;
		vec3 newPos = position+endVec+cohVec+sepVec;
		if (length(endVec)<0.01) {
			//TODO: This ought to be a function of the distance left to travel and the distance moved.
			//If the distance moved is very small, this should be set to false. But, to avoid coincidences, the distance to go should also be small.
			
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

			glColor3f(1.0, 0.0, 1.0);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(sepVec.x*100.0,0,sepVec.z*100.0);
			glEnd();

			glColor3f(0.0, 0.0, 1.0);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(cohVec.x*100.0,0,cohVec.z*100.0);
			glEnd();

			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(endVec.x*10.0,0,endVec.z*10.0);
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

	//Get the sum of all vectors from neighbours within a small radius to the agent.
	vec3 out = vec3 (0.0, 0.0, 0.0);
	for (vec3 neighbour : neighbours) {
		vec3 separation = vec3(position.x-neighbour.x, 0, position.z-neighbour.z);
		if (0<length(separation) && length(separation)<5) {

			//Set the length to be 1/length and add it to the current vector
			out += normalize(separation)*(1.0f/length(separation));
		}
	}
	//If the length is greater than 1, normalize it
	if (length(out)>1) {
		out = normalize(out);
	}

	//Is the length greater than 1?
	//If so, normalize and set its length to the separation strength.
	//Otherwise, multiply it by the separation strength so it is a fraction of its original length.
	cout << out.x << ", " << out.y << ", " << out.z << ": " << length(out) << endl;
	return length(out)>1 ? normalize(out)*SEPARATION_STRENGTH : out*SEPARATION_STRENGTH;
}

vec3 Agent::cohesion(vector<vec3> neighbours)
{
	//Return the sum of all vectors towards neighbours.
	vec3 out = vec3 (0.0, 0.0, 0.0);
	for (vec3 neighbour : neighbours) {
		out += (neighbour-position);
	}

	//Is the length of "out" greater than 1?
	//If so, normalize out and multiply it by the cohesion strength.
	//Otherwise, return the out vector multiplied by the strength.
	return length(out)>1 ? normalize(out)*COHESION_STRENGTH : out*COHESION_STRENGTH;
}

vec3 Agent::pathfind(vec3 endPoint)
{
	//Return a value that points towards the end, but with a length of 0.2.
	//Could scale this value to the agent's size.
	vec3 distance = endPoint-position;
	vec3 dstP = normalize(distance)*length(distance);

	//Is the distance to the end less than 3 units?
	//If so, return a vector in the same direction, but a fraction of the current length, dependent on the pathfinding strength (starting equal to the strength).
	//Otherwise, return a vector in the same direction with a length equal to the pathfinding strength.

	return length(distance)<3 ? distance*(PATHFIND_STRENGTH/3.0f) : normalize(distance)*PATHFIND_STRENGTH;
}