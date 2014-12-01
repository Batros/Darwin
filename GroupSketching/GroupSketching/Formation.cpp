#include "stdafx.h"
#include "Formation.h"


//Construct Formation with square shape and centre in the middle. This is the default case.
Formation::Formation(void)
{
	vector<glm::vec3> squareShape;
	//Left side of the square
	for (double i = -5.0; i <= 5.0; i += 0.25) {		
		squareShape.push_back(glm::vec3(i,-5.0,0.0));
	}
	//Bottom and top of the square
	for (double i = -5.0; i <= 5.0; i += 0.25) {		
		squareShape.push_back(glm::vec3(-5.0,i,0.0));
		squareShape.push_back(glm::vec3(+5.0,i,0.0));
	}
	//Right side of the square
	for (double i = -5.0; i <= 5.0; i += 0.25) {		
		squareShape.push_back(glm::vec3(i,5.0,0.0));
	}
	//Set the formation boundary as the square shape
	this->setBoundary(squareShape);
	//Set chosen point as the formation centre
	this->setCentre(glm::vec3(20.0,20.0,0.0));
}

//Construct formation with square shape and centre the parameter single point.
//TODO: MakeSquare function because the same code is in these 2 constructors.
Formation::Formation(glm::vec3 cen)
{
	vector<glm::vec3> squareShape;
	//Left side of the square
	for (double i = -5.0; i <= 5.0; i += 0.25) {		
		squareShape.push_back(glm::vec3(i,-5.0,0.0));
	}
	//Bottom and top of the square
	for (double i = -5.0; i <= 5.0; i += 0.25) {		
		squareShape.push_back(glm::vec3(-5.0,i,0.0));
		squareShape.push_back(glm::vec3(+5.0,i,0.0));

	}
	//Right side of the square
	for (double i = -5.0; i <= 5.0; i += 0.25) {		
		squareShape.push_back(glm::vec3(i,5.0,0.0));
	}
	//Set the formation boundary as the square shape
	this->setBoundary(squareShape);
	//Set chosen point as the formation centre
	this->setCentre(cen);
}

//Construct formation with boundary coordinates. Calculate the centre based on the input boundary and change the boundary to be relative to the centre.
Formation::Formation(vector<glm::vec3> boundary)
{
	// Find the (x,y) coordinates of the centre (the centroid of the input polygon) as the average x and y of the vertices.
	double cenX = 0.0;
	double cenY = 0.0;
	// Iterate over the boundary and sum the x and y values.
	for (std::vector<glm::vec3>::iterator bpoint = boundary.begin(); bpoint != boundary.end(); ++bpoint) {
		cenX += (*bpoint).x;
		cenY += (*bpoint).y;
	}
	// Divide the sums by the number of boundary points.
	cenX /= boundary.size();
	cenY /= boundary.size();

	// Calculate the relative positions of the boundary points and update the vector.
	for (std::vector<glm::vec3>::iterator bpoint = boundary.begin(); bpoint != boundary.end(); ++bpoint) {
		(*bpoint).x -= cenX;
		(*bpoint).y -= cenY;
	}

	// Set the boundary points and the centre point.
	this->setBoundary(boundary);
	this->setCentre(glm::vec3(cenX,cenY,0.0));
}

// Populate function with input number of agents.
// Finds the oversampled space and fits the number of agents to it.
void Formation::populate(int n)
{
	// TODO: Change to flood-fill algorithm with dynamic sampling rate.


	// TODO: Resample bounds to make sure the sampling is constant using the new sampling rate.
	vector<glm::vec3> resampledBoundaryCoords = this->boundaryCoords;

	// Basic sampling rate - distance between two points on the boundary.
	double samplingRate = sqrt((this->boundaryCoords[0].x - this->boundaryCoords[1].x)*(this->boundaryCoords[0].x - this->boundaryCoords[1].x) + (this->boundaryCoords[0].y - this->boundaryCoords[1].y)*(this->boundaryCoords[0].y - this->boundaryCoords[1].y));

	// Flood-fill algorithm.
	// Queue of points to check.
	vector<glm::vec3> q;
	// Vector of checked points.
	vector<glm::vec3> c;
	// Vector of filled points, initialise with the resampled boundary points.
	vector<glm::vec3> fPoints = resampledBoundaryCoords;

	while (!q.empty()) {
		// Select the point to check as the first point in the queue.
		glm::vec3 checkPoint = q.front();
		// Add the point to the list of checked points.
		c.push_back(checkPoint);
		glm::vec3 left = glm::vec3(checkPoint.x-samplingRate, checkPoint.y, checkPoint.z);
		glm::vec3 right = glm::vec3(checkPoint.x+samplingRate, checkPoint.y, checkPoint.z);
		// while left is in the bounds and hasn't checked.
		while (this->pointInBoundary(left) && !(std::find(c.begin(), c.end(), left) != c.end())) {
		
		}
			
		// TODO: Finish floodfill.
	}
}

// Populate function with input another formation.
// Used with formation transition.
// Finds the oversampled space and fits the agents of the input formation to it.
// Attempts to minimise position change between the formations.
// Default: work as in populate(n) without considering position change.
void Formation::populate(Formation* formation)
{
	// Call populate(n) with input the number of agents in the input formation.
	this->populate(formation->getAgentCoords().size());
}

// Populate function with input set of real world agent coordinates.
// Used with box select.
// Does not change the positions of the agents.
// Error checks the input for positions outside of the boundaries.
void Formation::populate(vector<glm::vec3> coords)
{
	// Boolean flag whether there is a point outside the bounds.
	bool flag = false;

	// If an agent is not in the boundary, set the flag to true and stop checking.
	for (std::vector<glm::vec3>::iterator agent = coords.begin(); agent != coords.end(); ++agent) {
		if (!this->pointInBoundary((*agent))) {
			flag = true;
			break;
		}
	}
	
	// TODO: If there is a point outside the bounds, return an error or move the point in?
	if (flag) {

	}

	// If all points are inside the bounds, set them as the agent coordinates
	else {
		this->setAgentCoords(coords);
	}
}

// Return the agent coordinates in real world space.
vector<glm::vec3> Formation::getAgentCoords()
{
	vector<glm::vec3> realCoords;
	// Iterate over the agent coords.
	for (std::vector<glm::vec3>::iterator agent = this->agentCoords.begin(); agent != this->agentCoords.end(); ++agent) {
		// For every agent calculate the real world coords (by adding the centre).
		glm::vec3 realAgent;
		realAgent.x = (*agent).x + this->centre.x;
		realAgent.y = (*agent).y + this->centre.y;
		realAgent.z = (*agent).z + this->centre.z;
		realCoords.push_back(realAgent);
	}
	// Return the real world coords.
	return realCoords;
}

// Return the boundary coordinates in real world space.
vector<glm::vec3> Formation::getBoundary()
{
	vector<glm::vec3> realCoords;
	// Iterate over the boundary coords
	for (std::vector<glm::vec3>::iterator bpoint = this->boundaryCoords.begin(); bpoint != this->boundaryCoords.end(); ++bpoint) {
		// For every boundary point calculate the real world coords (by adding the centre).
		glm::vec3 realBound;
		realBound.x = (*bpoint).x + this->centre.x;
		realBound.y = (*bpoint).y + this->centre.y;
		realBound.z = (*bpoint).z + this->centre.z;
		realCoords.push_back(realBound);
	}
	// Return the real world coords.
	return realCoords;
}

// Getter function for the formation's centre.
glm::vec3 Formation::getCentre()
{
	return this->centre;
}

// Setter function for the agent coordinates.
void Formation::setAgentCoords(vector<glm::vec3> coords)
{
	this->agentCoords = coords;
}

// Setter function for the boundary coordinates.
void Formation::setBoundary(vector<glm::vec3> coords)
{
	this->boundaryCoords = coords;
}

// Setter function for the centre.
void Formation::setCentre(glm::vec3 cen)
{
	this->centre = cen;
}

// Check if a point is in the bounds. Using Ray casting algorithm.
bool Formation::pointInBoundary(glm::vec3 point)
{
	//TODO
	// Generate a line from the point going in an arbitrary direction.
	// Check how many edges of the bounds polygon it intercepts.
	// If even the point is outside, inside otherwise.
	// NB: Points that lie on the boundary need to be checked separately.

	return true;
}

Formation::~Formation(void)
{
}
