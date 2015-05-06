#include "stdafx.h"
#include <iostream>
#include "AgentFocus.h"
using namespace std;
using namespace glm;


AgentFocus::AgentFocus(vec3 position) {
	this->position = position;
	this->colour = vec3(1.0, 0.0, 0.0);
	pathFound = false;
	facing = 0.0f;
	needsToMove = false;
	SIZE = 0.35f;
	SEPARATION_STRENGTH = 1.0f*SIZE;
	PATHFIND_STRENGTH = 0.25f;
	speed = 0.1f;
	type = 1;
}

AgentFocus::AgentFocus(vec3 position, vec3 end, vec3 colour) {
	this->position = position;
	this->endPoint = end;
	this->colour = colour;
	pathFound = false;
	facing = 0.0f;
	needsToMove = true;
	SIZE = 0.35f;
	SEPARATION_STRENGTH = 1.0f*SIZE;
	PATHFIND_STRENGTH = 0.25f;
	speed = 0.1f;
	type = 1;
}

float AgentFocus::getSize() {
	return this->SIZE;
}

float AgentFocus::getSpeed() {
	return speed;
}

void AgentFocus::setSpeedLimit(float s) {
	this->speedLimit = s;
}

vec3 AgentFocus::getEndPoint()
{
	return endPoint;
}

vec3 AgentFocus::getPosition()
{
	return position;
}

vec3 AgentFocus::getColour()
{
	return colour;
}

void AgentFocus::setEndPoint(vec3 end) {
	endPoint = end;
	needsToMove = true;
}

void AgentFocus::setPosition(vec3 newPosition)
{
	position = newPosition;
}

void AgentFocus::setColour(vec3 newColour)
{
	colour = newColour;
}

AgentFocus::~AgentFocus(void)
{
}

void AgentFocus::setPath(vector<vec3> path) {
	if (path.size() > 0) {
		this->nextPath = path[0];
		path.erase(path.begin());
		this->path = path;
	}
	else {
		pathFound = true;
	}
}

void AgentFocus::drawBase() {
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
	glBindTexture(GL_TEXTURE_2D, texture);

	float xPos = SIZE/2;
	float xNeg = -SIZE/2;
	float zPos = SIZE;
	float zNeg = -SIZE;
	float zPosEx = SIZE*1.5;
	float zNegEx = -SIZE*1.5;

	float height = SIZE/2;

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xNeg, 0.0f, zNeg);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xPos, 0.0f, zNeg);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xNeg, 0.0f, zPos);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xPos, 0.0f, zPos);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xNeg, height, zPosEx);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xPos, height, zPosEx);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xNeg, height, zNegEx);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xPos, height, zNegEx);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xNeg, 0.0f, zNeg);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xPos, 0.0f, zNeg);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xPos, height, zNegEx);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xPos, 0.0f, zNeg);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xPos, height, zPosEx);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xPos, 0.0f, zPos);

	glEnd();

	glBegin(GL_TRIANGLE_STRIP);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xNeg, height, zNegEx);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xNeg, 0.0f, zNeg);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xNeg, height, zPosEx);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xNeg, 0.0f, zPos);

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void AgentFocus::drawTop() {
	glEnable(GL_TEXTURE_2D);
	
	float xPos = SIZE*0.25;
	float xNeg = -SIZE*0.25;
	float zPos = SIZE*0.75;
	float zNeg = -SIZE*0.75;

	float height = SIZE/2;
	float topHeight = SIZE*0.75;


	////
	// BACK
	////

	glBegin(GL_TRIANGLE_STRIP);
	glBindTexture(GL_TEXTURE_2D, texture);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xNeg, height, zNeg);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xNeg, topHeight, zNeg);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xPos, height, zNeg);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xPos, topHeight, zNeg);

	glEnd();

	////
	// LEFT
	////

	glBegin(GL_TRIANGLE_STRIP);
	glBindTexture(GL_TEXTURE_2D, texture);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xNeg, height, zNeg);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xNeg, topHeight, zNeg);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xNeg, height, zPos);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xNeg, topHeight, zPos);

	glEnd();

	////
	// RIGHT
	////

	glBegin(GL_TRIANGLE_STRIP);
	glBindTexture(GL_TEXTURE_2D, texture);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xPos, height, zNeg);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xPos, topHeight, zNeg);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xPos, height, zPos);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xPos, topHeight, zPos);

	glEnd();

	////
	// FRONT
	////

	glBegin(GL_TRIANGLE_STRIP);
	glBindTexture(GL_TEXTURE_2D, texture);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xNeg, height, zPos);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xNeg, topHeight, zPos);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xPos, height, zPos);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xPos, topHeight, zPos);

	glEnd();

	////
	// TOP
	////

	glBegin(GL_TRIANGLE_STRIP);
	glBindTexture(GL_TEXTURE_2D, texture);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xNeg, topHeight, zNeg);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xPos, topHeight, zNeg);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xNeg, topHeight, zPos);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xPos, topHeight, zPos);

	glEnd();
}

void AgentFocus::drawGun() {
	//GLUquadricObj *quadratic;
	//quadratic = gluNewQuadric();
	//gluCylinder(quadratic, SIZE*0.1, SIZE*0.1, SIZE*0.75, 32, 32);

	glEnable(GL_TEXTURE_2D);
	
	float xPos = SIZE*0.1;
	float xNeg = -SIZE*0.1;
	float zPos = SIZE*1.35;
	float zNeg = SIZE*0.75;

	float height = SIZE/2;
	float topHeight = SIZE*0.75;


	////
	// BACK
	////

	glBegin(GL_TRIANGLE_STRIP);
	glBindTexture(GL_TEXTURE_2D, texture);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xNeg, height, zNeg);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xNeg, topHeight, zNeg);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xPos, height, zNeg);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xPos, topHeight, zNeg);

	glEnd();

	////
	// LEFT
	////

	glBegin(GL_TRIANGLE_STRIP);
	glBindTexture(GL_TEXTURE_2D, texture);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xNeg, height, zNeg);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xNeg, topHeight, zNeg);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xNeg, height, zPos);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xNeg, topHeight, zPos);

	glEnd();

	////
	// RIGHT
	////

	glBegin(GL_TRIANGLE_STRIP);
	glBindTexture(GL_TEXTURE_2D, texture);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xPos, height, zNeg);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xPos, topHeight, zNeg);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xPos, height, zPos);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xPos, topHeight, zPos);

	glEnd();

	////
	// FRONT
	////

	glBegin(GL_TRIANGLE_STRIP);
	glBindTexture(GL_TEXTURE_2D, texture);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xNeg, height, zPos);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xNeg, topHeight, zPos);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xPos, height, zPos);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xPos, topHeight, zPos);

	glEnd();

	////
	// TOP
	////

	glBegin(GL_TRIANGLE_STRIP);
	glBindTexture(GL_TEXTURE_2D, texture);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2d(0.0, 0.0);
	glVertex3f(xNeg, topHeight, zNeg);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2d(0.0, 1.0);
	glVertex3f(xPos, topHeight, zNeg);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2d(1.0, 0.0);
	glVertex3f(xNeg, topHeight, zPos);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2d(1.0, 1.0);
	glVertex3f(xPos, topHeight, zPos);

	glEnd();
}

void AgentFocus::draw() {
	glPushMatrix();
		if (type==2) {
			glColor3f(1.0, 0.5, 0.5);
		}
		else {
			glColor3f(1.0, 1.0, 1.0);
		}
		glTranslated(position.x, 0, position.z);
		glRotated(facing, 0.0, 1.0, 0.0);
		drawBase();

		glColor3f(0.35f, 0.35f, 0.35f);
		drawTop();

		glColor3f(0.5f, 0.5f, 0.5f); 
		drawGun();
	glPopMatrix();
}

void AgentFocus::update(vector<AgentFocus*> potentialNeighbours, float sepMod)
{
	if (needsToMove) {
		vec3 endVec = pathfind(endPoint);

		vector<vec3> sepNeighbours;
		vector<float> strength;
		for (AgentFocus* neighbour : potentialNeighbours) {
			if (length(neighbour->getPosition()-position)<5) {
				sepNeighbours.push_back(neighbour->getPosition());
				//If the distance left to travel is greater than 2, return 2. If it is less than SIZE, return SIZE. Otherwise return the distance.
				length(endVec)>2 ? strength.push_back(2) : (length(endVec)<SIZE ? strength.push_back(SIZE) : strength.push_back(length(endVec)));
			}
		}
		
		bool skipPath = false;
		if (positionsStack.size() > 8) {
			skipPath =  true;
			float prevDist = length(positionsStack[0]-nextPath);
			for (int i=1; i<5; i++) {
				float distToPath = length(positionsStack[i]-nextPath);
				if (distToPath < prevDist) {
					skipPath = false;
					break;
				}
			}
		}

		// Path following
		vec3 vectorToPath;
		if (pathFound) {
			vectorToPath = vec3(0.0,0.0,0.0);
		}
		else {
			vectorToPath = nextPath - position;
			if (skipPath || length(vectorToPath) < 2) {
				if (path.size() > 0) {
					nextPath = path[0];
					positionsStack.clear();
					path.erase(path.begin());
					vectorToPath = normalize(nextPath - position)*0.35f;
				}
				else {
					vectorToPath = vec3(0.0,0.0,0.0);
					pathFound = true;
				}
			}
			else {
				vectorToPath = normalize(nextPath - position)*0.35f;
			}
		}
		vec3 sepVec = separation(sepNeighbours, strength)*sepMod;
		vec3 deltaPos;
		if (!pathFound) {
			deltaPos = sepVec + vectorToPath;
		}
		else {
			deltaPos = endVec + sepVec;
		}
		deltaPos = normalize(deltaPos);

		vec3 newPos = position;
		float desiredFace = atan2f(deltaPos.x, deltaPos.z)*(180/M_PI);
		

		if (desiredFace > 360) {
			desiredFace -= 360;
		}
		else if (desiredFace < 0) {
			desiredFace += 360;
		}

		if (facing > 360) {
			facing -= 360;
		}
		else if (facing < 0) {
			facing += 360;
		}

		float angVel = 10.0f;
		if (abs(desiredFace-facing) > angVel) {
			if (abs(desiredFace - facing) < 180) {
				// normal
				if (desiredFace > facing) {
					facing += angVel;
				}
				else {
					facing -= angVel;
				}
			}
			else {
				if (desiredFace < facing) {
					facing += angVel;
				}
				else {
					facing -= angVel;
				}
			}
		}
		else {
			facing = desiredFace;
		}

		newPos = position+(deltaPos*speedLimit);

		float distFromEnd = length(position-endPoint);
		if (distFromEnd < 0.1) {
			needsToMove = false;
		}
		else if (positionsStack.size()==10) {
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
			
			if ((distanceTravelled<(totalLength/90.0)) && (distFromEnd < 10)) {
				needsToMove = false;
			}
		} else {
			//Otherwise just push the new position onto the stack
			positionsStack.push_back(newPos);
		}
		setPosition(newPos);

		draw();
	} else {
		/* OLD CODE
		//Calculate pushing vector - once an agent is where it needs to be, others do not use them for separation vectors.
		//Instead, these agents have strong separation to every other agent, but only very close - if an agent is within 1.5 units, move away strongly.
		//If no agents are that close, go back to your desired spot.
		vector<vec3> pshNeighbours;
		for (AgentFocus* neighbour : potentialNeighbours) {
			if (length(neighbour->getPosition()-position)<(3*SIZE)) {
				pshNeighbours.push_back(neighbour->getPosition());
			}
		}
		vec3 pshVec = getPushedBy(pshNeighbours);
		


		if (potentialNeighbours.size() > 0) {
			vector<vec3> sepNeighbours;
			vector<float> strength;
			for (AgentFocus* neighbour : potentialNeighbours) {
				if (length(neighbour->getPosition()-position)<(2*SIZE)) {
					sepNeighbours.push_back(neighbour->getPosition());
					strength.push_back(1);
				}
			}
			if (sepNeighbours.size() > 0) {
				vec3 sepVec = separation(sepNeighbours, strength)*sepMod;
				vec3 deltaPos = normalize(sepVec);

				vec3 newPos = position;
				float desiredFace = atan2f(deltaPos.x, deltaPos.z)*(180/M_PI);
		

				if (desiredFace > 360) {
					desiredFace -= 360;
				}
				else if (desiredFace < 0) {
					desiredFace += 360;
				}

				if (facing > 360) {
					facing -= 360;
				}
				else if (facing < 0) {
					facing += 360;
				}

				float angVel = 10.0f;
				if (abs(desiredFace-facing) > angVel) {
					if (abs(desiredFace - facing) < 180) {
						// normal
						if (desiredFace > facing) {
							facing += angVel;
						}
						else {
							facing -= angVel;
						}
					}
					else {
						if (desiredFace < facing) {
							facing += angVel;
						}
						else {
							facing -= angVel;
						}
					}
				}
				else {
					facing = desiredFace;
				}

				newPos = position+(deltaPos*speed);

				setPosition(newPos);
			}
		}
		*/
		draw();
	}
}

vec3 AgentFocus::separation(vector<vec3> neighbours, vector<float> strength)
{
	//Return the inverse-square sum of all vectors away from nearby neighbours.

	//Get the sum of all vectors from neighbours within a small radius to the agent.
	vec3 out = vec3 (0.0, 0.0, 0.0);
	for (unsigned int i=0; i<neighbours.size(); i++) {
		vec3 neighbour = neighbours[i];
		vec3 separation = vec3(position.x-neighbour.x, 0, position.z-neighbour.z);
		float x = length(separation);
		if (x > 0) {
			//TODO - fix flickering of this vector. If the length to the other one is 4.99, the separation should be tiny. If it is 1, it should basically be trying as hard as it can to get to a distance of 1.

			//We want the vector to be gigantic if they are touching, so it should be an exponential decay curve.
			//Set the length to be Ne^(-Lambda*x-SIZE) and add it to the current vector
			
			out += normalize(separation)*(10*exp(-4*(x-(SIZE*2))))*strength[i];
		}
	}

	//Is the length greater than 0.5?
	//If so, normalize and set its length to the separation strength.
	//Otherwise, multiply it by the separation strength so it is a fraction of its original length.
	return length(out)>0.5 ? normalize(out)*SEPARATION_STRENGTH*0.5f : out*SEPARATION_STRENGTH;
}

vec3 AgentFocus::pathfind(vec3 endPoint)
{
	//Return a value that points towards the end, but with a length of 0.2.
	//Could scale this value to the agent's size.
	vec3 distance = endPoint-position;

	//Is the distance to the end less than 3 units?
	//If so, return a vector in the same direction, but a fraction of the current length, dependent on the pathfinding strength (starting equal to the strength).
	//Otherwise, return a vector in the same direction with a length equal to the pathfinding strength.

	return length(distance)<3 ? distance*(PATHFIND_STRENGTH/3.0f) : normalize(distance)*PATHFIND_STRENGTH;
}

vec3 AgentFocus::getPushedBy(vector<vec3> neighbours)
{
	vec3 out = vec3(0.0, 0.0, 0.0);

	for (vec3 neighbourVec : neighbours) {
		//Move so that you are not touching any of them
		cout << length(neighbourVec-getPosition()) << endl;
	}
	return out;
}

bool AgentFocus::isStillMoving() {
	return needsToMove;
}

vec3 AgentFocus::randomVec() {
	//Return the previous random vector, modified by a small value. This one should be accelerated, rather than moved.
	float ranX = (rand() % 100) / 100.0f;
	float ranY = (rand() % 100) / 100.0f;
	return vec3(0.0, 0.0, 0.0);
}

void AgentFocus::setType(int t) {
	this->type = t;
	if (t==1) {
		speed = 0.1;
		SIZE = 0.35f;
	}
	else if (t==2) {
		speed = 0.2;
		SIZE = 0.25f;
	}
}