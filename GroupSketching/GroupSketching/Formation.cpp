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
	// Vector of filled points
	vector<glm::vec3> fPoints;

	// Initialize q with the centre point
	q.push_back(this->centre);
	// TODO: if the centre is not in the bounds for some reason, find a different point.

	while (!q.empty()) {
		// Select the point to check as the first point in the queue.
		glm::vec3 checkPoint = q.back();
		// Delete the point from the queue.
		q.pop_back();
		// Set left and right as the left and right neighbours of checkPoint.
		glm::vec3 left = glm::vec3(checkPoint.x, checkPoint.y, checkPoint.z);
		glm::vec3 right = glm::vec3(checkPoint.x+samplingRate, checkPoint.y, checkPoint.z);
		// Move along the points to the left.
		while(pointInBoundary(left) && !(std::find(c.begin(), c.end(), left) != c.end())) {
			// Add left to fPoints.
			fPoints.push_back(left);
			// Set the top and bottom points.
			glm::vec3 top = glm::vec3(left.x, left.y+samplingRate, left.z);
			glm::vec3 bottom = glm::vec3(left.x, left.y-samplingRate, left.z);
			// If top/bottom are in the bounds and have not been checked, add them to the queue and fPoints.
			while(pointInBoundary(top) && !(std::find(c.begin(), c.end(), top) != c.end())) {
				fPoints.push_back(top);
				q.push_back(top);
				c.push_back(top);
			}
			while(pointInBoundary(bottom) && !(std::find(c.begin(), c.end(), bottom) != c.end())) {
				fPoints.push_back(bottom);
				q.push_back(bottom);
				c.push_back(bottom);
			}
			// Move left along
			left = glm::vec3(left.x-samplingRate, left.y, left.z);
		}
		// Repeat for right.
		while(pointInBoundary(right) && !(std::find(c.begin(), c.end(), right) != c.end())) {
			// Add left to fPoints.
			fPoints.push_back(right);
			// Set the top and bottom points.
			glm::vec3 top = glm::vec3(right.x, right.y+samplingRate, right.z);
			glm::vec3 bottom = glm::vec3(right.x, right.y-samplingRate, right.z);
			// If top/bottom are in the bounds and have not been checked, add them to the queue and fPoints.
			while(pointInBoundary(top) && !(std::find(c.begin(), c.end(), top) != c.end())) {
				fPoints.push_back(top);
				q.push_back(top);
				c.push_back(top);
			}
			while(pointInBoundary(bottom) && !(std::find(c.begin(), c.end(), bottom) != c.end())) {
				fPoints.push_back(bottom);
				q.push_back(bottom);
				c.push_back(bottom);
			}
			// Move right along.
			right = glm::vec3(right.x+samplingRate, right.y, right.z);
		}
	}

	// Choose the agents from the oversampled list
	// Take away the number of boundary agents to figure out how many agents will be inside the boundary.
	int insideN = n-resampledBoundaryCoords.size();


	if (insideN > 0) {
		// Add the resampled boundary as the agent coords.
		for (std::vector<glm::vec3>::iterator bpoint = resampledBoundaryCoords.begin(); bpoint != resampledBoundaryCoords.end(); ++bpoint) {
			// Convert to coordinates relative to centre.
			this->agentCoords.push_back(glm::vec3((*bpoint).x-this->centre.x,(*bpoint).y-this->centre.y,(*bpoint).z-this->centre.z));
		}
		// Calculate a rate for agents inside.
		int insideRate = fPoints.size()/insideN;
		int leftover = fPoints.size()%insideN;

		// Add the inside coordinates at a rate of insideRate.
		for (int i = 0; i < fPoints.size(); i += insideRate) {
			// Convert to coordinates relative to centre.
			this->agentCoords.push_back(glm::vec3(fPoints[i].x-this->centre.x,fPoints[i].y-this->centre.y,fPoints[i].z-this->centre.z));
		}
		// Add coordinates to make up for integer division.
		for (int i = 1; i < fPoints.size() && leftover > 0; i += insideRate) {
			this->agentCoords.push_back(glm::vec3(fPoints[i].x-this->centre.x,fPoints[i].y-this->centre.y,fPoints[i].z-this->centre.z));
			leftover -= 1;
		}
	}

	else {
		// Add the resampled boundary as the agent coords. Only take the first n coordinates.
		for (std::vector<glm::vec3>::iterator bpoint = resampledBoundaryCoords.begin(); bpoint != resampledBoundaryCoords.end(); ++bpoint) {
			// Convert to coordinates relative to centre.
			this->agentCoords.push_back(glm::vec3((*bpoint).x-this->centre.x,(*bpoint).y-this->centre.y,(*bpoint).z-this->centre.z));
			n -= 1;
			if (n == 0)
				break;
		}
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
	// The two formations need to be lined up.
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
	// Ray casting algorithm
	// Count how many times a constant ray from the point intercepts the polygon.
	// If the number is odd, then the point is inside. Outside otherwise.
	// NB: This algorithm only works for 2-D coordinates. Need angle sum algorithm for 3-D.

	// Set count to 0.
	int count = 0;

	// Generate a constant point going up from the givent point (to calculate the ray).
	glm::vec3 r = glm::vec3(point.x, point.y+1.0, point.z);

	// Iterate over the sides of the boundary polygon.
	for (int i = 1; i < this->boundaryCoords.size(); i++) {
		glm::vec3 q = this->boundaryCoords[i-1];
		glm::vec3 s = this->boundaryCoords[i];

		double epsilon = 1>>16;
		// Check if the lines are parallel
		if (abs(((q.x - s.x)*(point.y - r.y)-(q.y - s.y)*(point.x - r.x))) <= epsilon) {
			if (point.x == q.x)
				count += 1;
			continue;
		}

		// Calculate the (x,y) coordinates of the interception point.
		double Px = ((q.x*s.y-s.x*q.y)*(point.x-r.x)-(q.x-s.x)*(point.x*r.y-point.y*r.x))/((q.x - s.x)*(point.y - r.y)-(q.y - s.y)*(point.x - r.x));
		double Py = ((q.x*s.y-s.x*q.y)*(point.y-r.y)-(q.y-s.y)*(point.x*r.y-point.y*r.x))/((q.x - s.x)*(point.y - r.y)-(q.y - s.y)*(point.x - r.x));

		// Check if the interception poin is inside the line by checking the crossproduct and comparing the dotproduct to the length of the line.
		double crossProduct = (Py-q.y)*(s.x-q.x)-(Px-q.x)*(s.y-q.y);
		if (abs(crossProduct) <= epsilon) {
			double dotProduct = (Px-q.x)*(s.x-q.x)+(Py-q.y)*(s.y-q.y);
			double lineSqLength = (s.x-q.x)*(s.x-q.x)+(s.y-q.y)*(s.y-q.y);
			if (dotProduct > lineSqLength) {
				count += 1;
			}
		}
	}

	// If count is odd the point is inside. Outside otherwise.
	if (count % 2) {
		return true;
	}
	else {
		return false;
	}
}

Formation::~Formation(void)
{
}
