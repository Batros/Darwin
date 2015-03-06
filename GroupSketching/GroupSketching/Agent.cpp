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
	SIZE = 0.5f;
	COHESION_STRENGTH = 0.04f;
	SEPARATION_STRENGTH = 1.15*SIZE;
	PATHFIND_STRENGTH = 0.08f;
}

vec3 Agent::getPosition()
{
	return position;
}

vec3 Agent::getColour()
{
	return colour;
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


void Agent::update(vector<Agent*> potentialNeighbours)
{
	if (needsToMove) {
		//v1 (done): Do nothing with neighbours, move 1/200th of the way to the end, with minimum and maximum speeds.
		//v2 (done): Calculate inverse-square separation of neighbours, and cohesion vector towards them.
		//v3 (in-dev): Work out if the agent needs to move any more, and stop it if not
		//v4 (future): Take into account different attributes - have an AgentProperties class or something that has colour, speed etc, so heterogeneity can be implemented
		//v5 (future): Allow for an adjustable weighting to these vectors, and have a pointer to the crowd velocity so agents' velocities can be clamped.
		//v6 (future): Add some kind of rule that takes into account any other restrictions, e.g. the requirement that the agents walk in line with others.

		vector<vec3> cohNeighbours;
		for (int i=0; i<potentialNeighbours.size(); i++) {
			if (potentialNeighbours[i]->getColour() == colour) {
				cohNeighbours.push_back(potentialNeighbours[i]->getPosition());
			}
		}

		vector<vec3> sepNeighbours;
		for (int i=0; i<potentialNeighbours.size(); i++) {
			//if (length(potentialNeighbours[i]->getPosition-position)<5) {
				sepNeighbours.push_back(potentialNeighbours[i]->getPosition());
			//}
		}
		

		vec3 sepVec = separation(sepNeighbours);
		vec3 cohVec = cohesion(cohNeighbours);
		vec3 endVec = pathfind(endPoint);
		vec3 modVec = (sepVec+cohVec+endVec)*10.0f;
		vec3 newPos = position+endVec+cohVec+sepVec;
		float endLen = length(endVec);
		float vecLen = length(endVec+cohVec+sepVec);
		if ((endLen*vecLen)<0.0001) {
			//TODO: This ought to be a function of the distance left to travel and the distance moved.
			//If the distance moved is very small, this should be set to false. But, to avoid coincidences, the distance to go should also be small.
			
			needsToMove = false;
		}
		setPosition(newPos);
		glPushMatrix();
			glColor3f(colour.x, colour.y, colour.z);
			glTranslated(position.x, 0, position.z);
			glutSolidSphere(SIZE, 20, 20);
			glLineWidth(2.5);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(modVec.x,0,modVec.z);
			glEnd();

			glColor3f(1.0, 0.0, 1.0);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(sepVec.x*SIZE*50.0,0,sepVec.z*SIZE*50.0);
			glEnd();

			glColor3f(colour.x, colour.y, colour.z);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(cohVec.x*SIZE*50.0,0,cohVec.z*SIZE*50.0);
			glEnd();

			glColor3f(1.0, 1.0, 0.0);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(endVec.x*SIZE*50.0,0,endVec.z*SIZE*50.0);
			glEnd();
		glPopMatrix();
	} else {
		//Calculate pushing vector - once an agent is where it needs to be, others do not use them for separation vectors.
		//Instead, these agents have strong separation to every other agent, but only very close - if an agent is within 1.5 units, move away strongly.
		//If no agents are that close, go back to your desired spot.
		vector<vec3> pshNeighbours;
		for (int i=0; i<potentialNeighbours.size(); i++) {
			if (length(potentialNeighbours[i]->getPosition()-position)<SIZE) {
				pshNeighbours.push_back(potentialNeighbours[i]->getPosition());
			}
		}
		vec3 pshVec = getPushedBy(pshNeighbours);
		glPushMatrix();
			glColor3f(1-colour.x, 1-colour.y, 1-colour.z);
			glTranslated(position.x, 0, position.z);
			glutSolidSphere(SIZE, 20, 20);
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
		//Possible optimization - precalculate length(separation) so it doesn't have to be calced 2 or 3 times
		if (0<length(separation) && length(separation)<5) {
			//TODO - fix flickering of this vector. If the length to the other one is 4.99, the separation should be tiny. If it is 1, it should basically be trying as hard as it can to get to a distance of 1.

			//We want the vector to be gigantic if they are touching, so it should be an exponential decay curve.
			//Set the length to be Ne^(-Lambda*x-SIZE) and add it to the current vector
			float x = length(separation);
			out += normalize(separation)*(10*exp(-4*(x+(SIZE*2))));
		}
	}

	//Is the length greater than 1?
	//If so, normalize and set its length to the separation strength.
	//Otherwise, multiply it by the separation strength so it is a fraction of its original length.
	return length(out)>1 ? normalize(out)*SEPARATION_STRENGTH : out*SEPARATION_STRENGTH;
}

vec3 Agent::cohesion(vector<vec3> neighbours)
{
	//Return the sum of all vectors towards neighbours.
	vec3 out = vec3 (0.0, 0.0, 0.0);
	for (vec3 neighbour : neighbours) {
		out += (neighbour-position);
	}

	//Decrease the cohesion if it is close to the end point, so they don't get pulled together much in spread-out formations.
	if (length(endPoint-position)<3) {
		out = out*(length(endPoint-position)/3.0f);
	}
	//Is the length greater than 1?
	//If so, normalize and multiply it by the cohesion strength.
	//Otherwise, return the out vector multiplied by the strength.
	return length(out)>1 ? normalize(out)*COHESION_STRENGTH : out*COHESION_STRENGTH;
}

vec3 Agent::pathfind(vec3 endPoint)
{
	//Return a value that points towards the end, but with a length of 0.2.
	//Could scale this value to the agent's size.
	vec3 distance = endPoint-position;

	//Is the distance to the end less than 3 units?
	//If so, return a vector in the same direction, but a fraction of the current length, dependent on the pathfinding strength (starting equal to the strength).
	//Otherwise, return a vector in the same direction with a length equal to the pathfinding strength.

	return length(distance)<3 ? distance*(PATHFIND_STRENGTH/3.0f) : normalize(distance)*PATHFIND_STRENGTH;
}

vec3 Agent::getPushedBy(vector<vec3> neighbours)
{
	vec3 out = vec3(0.0, 0.0, 0.0);

	for (int i=0; i<neighbours.size(); i++) {
	}
		//Move so that you are not touching any of them
	return out;
}