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
	SEPARATION_STRENGTH = 1.0f*SIZE;
	PATHFIND_STRENGTH = 0.25f;
}

vec3 Agent::getEndPoint()
{
	return endPoint;
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

void Agent::setColour(vec3 newColour)
{
	colour = newColour;
}

Agent::~Agent(void)
{
}


void Agent::update(vector<Agent*> potentialNeighbours, float urgency, vec3 crdVec)
{
	if (needsToMove) {
		//v1 (done): Do nothing with neighbours, move 1/200th of the way to the end, with minimum and maximum speeds.
		//v2 (done): Calculate inverse-square separation of neighbours, and cohesion vector towards them.
		//v3 (in-dev): Work out if the agent needs to move any more, and stop it if not
		//v4 (future): Take into account different attributes - have an AgentProperties class or something that has colour, speed etc, so heterogeneity can be implemented
		//v5 (future): Allow for an adjustable weighting to these vectors, and have a pointer to the crowd velocity so agents' velocities can be clamped.
		//v6 (future): Add some kind of rule that takes into account any other restrictions, e.g. the requirement that the agents walk in line with others.
		
		vec3 endVec = pathfind(endPoint);

		vector<vec3> sepNeighbours;
		vector<float> strength;
		for (Agent* neighbour : potentialNeighbours) {
			if (length(neighbour->getPosition()-position)<5) {
				sepNeighbours.push_back(neighbour->getPosition());
				//If the distance left to travel is greater than 2, return 2. If it is less than SIZE, return SIZE. Otherwise return the distance.
				length(endVec)>2 ? strength.push_back(2) : (length(endVec)<SIZE ? strength.push_back(SIZE) : strength.push_back(length(endVec)));
			}
		}
		
		//TODO - make them move faster when they get closer to the end
		vec3 sepVec = separation(sepNeighbours, strength);
		//Add a rule - the closer they are to their end point, the closer they are allowed to get to each other)
		vec3 modVec = (sepVec+endVec)*10.0f;
		vec3 rndVec = randomVec();
		vec3 newPos = position+((endVec+sepVec)*urgency);
		float endLen = length(endVec);
		float vecLen = length(endVec+sepVec);
		if (positionsStack.size()==50) {
			//Check the previous 50 updates - the distance travelled and the length of the vectors.
			//If the distance travelled is small compared to the lengths, it is likely the agent is stuck jittering back and forth, so set it as "reached destination".

				
			positionsStack.push_back(newPos);
			vec3 oldPos = positionsStack.front();
			double distanceTravelled = length(oldPos-newPos);
			double totalLength = 0;
			for (unsigned int i=0; i<positionsStack.size()-1; i++) {
				totalLength += length(positionsStack[i+1]-positionsStack[i]);
			}
			positionsStack.erase(positionsStack.begin());
			if (distanceTravelled<(totalLength/90.0)) {
				needsToMove = false;
			}
		} else {
			//Otherwise just push the new position onto the stack
			positionsStack.push_back(newPos);
		}
		//Also check to see how close it is to its destination. If it is close, and the speed is low, set it as reached.
		if (vecLen*length(endPoint-position)<0.001) {
			needsToMove = false;
		}
		setPosition(newPos);
		glPushMatrix();
			glColor3f(colour.x, colour.y, colour.z);
			glTranslated(position.x, 0, position.z);
			glutSolidSphere(SIZE, 20, 20);
			GLUquadricObj *quadratic;
			quadratic = gluNewQuadric();
			double tn = 180*atan2(modVec.x+crdVec.x, modVec.z+crdVec.z)/M_PI;
			glRotated(tn, 0.0, 1.0, 0.0);
			gluCylinder(quadratic, 0.5f, 0, 2.0f, 32, 32);
			glLineWidth(5.5);/*
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
			*/
			glColor3f(1.0, 1.0, 0.0);
			glBegin(GL_LINES);
			glVertex3f(0,0,0);
			vec3 direction = normalize(modVec)*SIZE*5.0f;
			glVertex3f(direction.x,0,direction.z);
			glEnd();
		glPopMatrix();
	} else {
		//Calculate pushing vector - once an agent is where it needs to be, others do not use them for separation vectors.
		//Instead, these agents have strong separation to every other agent, but only very close - if an agent is within 1.5 units, move away strongly.
		//If no agents are that close, go back to your desired spot.
		/*vector<vec3> pshNeighbours;
		for (Agent* neighbour : potentialNeighbours) {
			if (length(neighbour->getPosition()-position)<SIZE) {
				pshNeighbours.push_back(neighbour->getPosition());
			}
		}
		vec3 pshVec = getPushedBy(pshNeighbours);*/
		glPushMatrix();
			glColor3f(1-colour.x, 1-colour.y, 1-colour.z);
			glTranslated(position.x, 0, position.z);
			glutSolidSphere(SIZE, 20, 20);
		glPopMatrix();
	}
}

vec3 Agent::separation(vector<vec3> neighbours, vector<float> strength)
{
	//Return the inverse-square sum of all vectors away from nearby neighbours.

	//Get the sum of all vectors from neighbours within a small radius to the agent.
	vec3 out = vec3 (0.0, 0.0, 0.0);
	for (unsigned int i=0; i<neighbours.size(); i++) {
		vec3 neighbour = neighbours[i];
		vec3 separation = vec3(position.x-neighbour.x, 0, position.z-neighbour.z);
		//Possible optimization - precalculate length(separation) so it doesn't have to be calced 2 or 3 times
		if (0<length(separation) && length(separation)<5) {
			//TODO - fix flickering of this vector. If the length to the other one is 4.99, the separation should be tiny. If it is 1, it should basically be trying as hard as it can to get to a distance of 1.

			//We want the vector to be gigantic if they are touching, so it should be an exponential decay curve.
			//Set the length to be Ne^(-Lambda*x-SIZE) and add it to the current vector
			float x = length(separation);
			out += normalize(separation)*(10*exp(-4*(x-(SIZE*2))))*strength[i];
		}
	}

	//Is the length greater than 0.5?
	//If so, normalize and set its length to the separation strength.
	//Otherwise, multiply it by the separation strength so it is a fraction of its original length.
	return length(out)>0.5 ? normalize(out)*SEPARATION_STRENGTH*0.5f : out*SEPARATION_STRENGTH;
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

/*vec3 Agent::getPushedBy(vector<vec3> neighbours)
{
	vec3 out = vec3(0.0, 0.0, 0.0);

	for (vec3 neighbourVec : neighbours) {
		//Move so that you are not touching any of them
	}
	return out;
}*/

bool Agent::isStillMoving() {
	return needsToMove;
}

vec3 Agent::randomVec() {
	//Return the previous random vector, modified by a small value. This one should be accelerated, rather than moved.
	float ranX = (rand() % 100) / 100.0f;
	float ranY = (rand() % 100) / 100.0f;
	return vec3(0.0, 0.0, 0.0);
}