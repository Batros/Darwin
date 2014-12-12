#include "stdafx.h"
#include "Formation.h"


//Construct Formation with square shape and centre in the middle. This is the default case.
Formation::Formation(void)
{
	Formation(glm::vec3(0.0, 0.0, 0.0));
}

//Construct formation with square shape and centre the parameter single point.
Formation::Formation(glm::vec3 cen)
{
	vector<glm::vec3> squareShape;
	//Left side of the square
	for (double i = -5.0; i < 5.0; i += 2.5) {
		squareShape.push_back(glm::vec3(-5.0, 0.0, i));
	}

	//Top of the square
	for (double i = -5.0; i < 5.0; i += 2.5) {
		squareShape.push_back(glm::vec3(i, 0.0, 5.0));
	}

	//Right side of the square
	for (double i = 5.0; i > -5.0; i -= 2.5) {
		squareShape.push_back(glm::vec3(5.0, 0.0, i));
	}

	//Bottom of the square
	for (double i = 5.0; i > -5.0; i -= 2.5) {
		squareShape.push_back(glm::vec3(i, 0.0, -5.0));
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
	double cenZ = 0.0;
	// Iterate over the boundary and sum the x and y values.
	for (std::vector<glm::vec3>::iterator bpoint = boundary.begin(); bpoint != boundary.end(); ++bpoint) {
		cenX += (*bpoint).x;
		cenY += (*bpoint).y;
		cenZ += (*bpoint).z;
	}
	// Divide the sums by the number of boundary points.
	cenX /= boundary.size();
	cenY /= boundary.size();
	cenZ /= boundary.size();

	// Calculate the relative positions of the boundary points and update the vector.
	for (std::vector<glm::vec3>::iterator bpoint = boundary.begin(); bpoint != boundary.end(); ++bpoint) {
		(*bpoint).x -= cenX;
		(*bpoint).y -= cenY;
		(*bpoint).z -= cenZ;
	}

	// Set the boundary points and the centre point.
	this->setBoundary(boundary);
	this->setCentre(glm::vec3(cenX, cenY, cenZ));
}

// Populate function with input number of agents.
// Finds the oversampled space and fits the number of agents to it.
void Formation::populate(int n)
{
	if (n == 0) {
		this->agentCoords.push_back(glm::vec3(0, 0, 0));
		this->agentCoords.push_back(glm::vec3(0, 0, 0));
		this->agentCoords.push_back(glm::vec3(0, 0, 0));
		this->agentCoords.push_back(glm::vec3(0, 0, 0));
		this->agentCoords.push_back(glm::vec3(0, 0, 0));
		this->agentCoords.push_back(glm::vec3(0, 0, 0));
		return;
	}
	if (n == 1) {
		this->agentCoords.push_back(glm::vec3(-20, 0, 50));
		this->agentCoords.push_back(glm::vec3(-20, 0, -50));
		this->agentCoords.push_back(glm::vec3(20, 0, 30));
		this->agentCoords.push_back(glm::vec3(-20, 0, 30));
		this->agentCoords.push_back(glm::vec3(20, 0, 10));
		this->agentCoords.push_back(glm::vec3(-20, 0, 10));
		return;
	}

	// TODO: Change to flood-fill algorithm with dynamic sampling rate.


	// TODO: Resample bounds to make sure the sampling is constant using the new sampling rate.
	vector<glm::vec3> resampledBoundaryCoords = this->boundaryCoords;

	// Basic sampling rate - distance between two points on the boundary.
	double samplingRate = 0.0;
	for (int i = 0; i < this->boundaryCoords.size() - 1; i++) {
		samplingRate += sqrt((this->boundaryCoords[i].x - this->boundaryCoords[i+1].x)*(this->boundaryCoords[i].x - this->boundaryCoords[i+1].x) + (this->boundaryCoords[i].y - this->boundaryCoords[i+1].y)*(this->boundaryCoords[i].y - this->boundaryCoords[i+1].y));
	}
	
	samplingRate = samplingRate / this->boundaryCoords.size();
	double sampleX = samplingRate;
	double sampleY = samplingRate;
	double sampleZ = samplingRate;

	//cout << sampleX << " " << sampleY << " " << sampleZ << endl;
	// Flood-fill algorithm.
	// Queue of points to check.
	vector<glm::vec3> q;
	// Vector of checked points.
	vector<glm::vec3> c;
	// Vector of filled points
	vector<glm::vec3> fPoints;

	// Initialize q with the boundary.
	q = resampledBoundaryCoords;
	// TODO: if the centre is not in the bounds for some reason, find a different point.
	while (!q.empty()) {
		// Select the point to check as the first point in the queue.
		glm::vec3 checkPoint = q.back();
		c.push_back(checkPoint);
		// Delete the point from the queue.
		q.pop_back();
		// Set left and right as the left and right neighbours of checkPoint.
		glm::vec3 left = glm::vec3(checkPoint.x, checkPoint.y, checkPoint.z);
		glm::vec3 right = glm::vec3(checkPoint.x + sampleX, checkPoint.y, checkPoint.z);
		// Move along the points to the left.
		while (pointInBoundary(left) && !(std::find(c.begin(), c.end(), left) != c.end())) {
			// Add left to fPoints.
			fPoints.push_back(left);
			//c.push_back(left);
			// Set the top and bottom points.
			glm::vec3 top = glm::vec3(left.x, left.y, left.z + sampleZ);
			glm::vec3 bottom = glm::vec3(left.x, left.y, left.z - sampleZ);
			// If top/bottom are in the bounds and have not been checked, add them to the queue and fPoints.
			if (pointInBoundary(top) && !(std::find(c.begin(), c.end(), top) != c.end())) {
				//fPoints.push_back(top);
				q.push_back(top);
				//c.push_back(top);
			}
			if (pointInBoundary(bottom) && !(std::find(c.begin(), c.end(), bottom) != c.end())) {
				//fPoints.push_back(bottom);
				q.push_back(bottom);
				//c.push_back(bottom);
			}
			// Move left along
			left = glm::vec3(left.x - sampleX, left.y, left.z);
		}
		// Repeat for right.
		while (pointInBoundary(right) && !(std::find(c.begin(), c.end(), right) != c.end())) {
			// Add left to fPoints.
			fPoints.push_back(right);
			//c.push_back(right);
			// Set the top and bottom points.
			glm::vec3 top = glm::vec3(right.x, right.y, right.z + sampleZ);
			glm::vec3 bottom = glm::vec3(right.x, right.y, right.z - sampleZ);
			// If top/bottom are in the bounds and have not been checked, add them to the queue and fPoints.
			if (pointInBoundary(top) && !(std::find(c.begin(), c.end(), top) != c.end())) {
				//fPoints.push_back(top);
				q.push_back(top);
				//c.push_back(top);
			}
			if (pointInBoundary(bottom) && !(std::find(c.begin(), c.end(), bottom) != c.end())) {
				//fPoints.push_back(bottom);
				q.push_back(bottom);
				//c.push_back(bottom);
			}
			// Move right along.
			right = glm::vec3(right.x + sampleX, right.y, right.z);
		}
	}

	// Choose the agents from the oversampled list
	// Take away the number of boundary agents to figure out how many agents will be inside the boundary.
	int insideN = n - resampledBoundaryCoords.size();
	//this->agentCoords = fPoints;
	
	//cout << insideN << endl;
	if (insideN > 0) {
		// Add the resampled boundary as the agent coords.
		for (std::vector<glm::vec3>::iterator bpoint = resampledBoundaryCoords.begin(); bpoint != resampledBoundaryCoords.end(); ++bpoint) {
			// Convert to coordinates relative to centre.
			this->agentCoords.push_back((*bpoint));
		}
		
		// Calculate a rate for agents inside.
		int insideRate = fPoints.size() / insideN;
		int leftover = fPoints.size() % insideN;
		if (insideRate != 0) {
			// Add the inside coordinates at a rate of insideRate.
			for (int i = 0; i < fPoints.size(); i += insideRate) {
				this->agentCoords.push_back(fPoints[i]);
			}
		}
		// Add coordinates to make up for integer division.
		for (int i = 1; i < fPoints.size() && leftover > 0; i += insideRate) {
			this->agentCoords.push_back(fPoints[i]);
			leftover -= 1;
		}
	}

	else {
		// Add the resampled boundary as the agent coords. Only take the first n coordinates.
		//cout << "Outside" << endl;
		for (std::vector<glm::vec3>::iterator bpoint = resampledBoundaryCoords.begin(); bpoint != resampledBoundaryCoords.end(); ++bpoint) {
			// Convert to coordinates relative to centre.
			this->agentCoords.push_back((*bpoint));
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
		realAgent = (*agent) + this->centre;
		//cout << "Agent " << (*agent).x << " " << (*agent).y << " " << (*agent).z << endl;
		//cout << "Centre " << this->centre.x << " " << this->centre.y << " " << this->centre.z << endl;
		//cout << "Real Agent " << realAgent.x << " " << realAgent.y << " " << realAgent.z << endl;
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
		realBound = (*bpoint) + this->centre;
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
	int polySides = this->boundaryCoords.size();
	float polyX[1024];
	float polyY[1024];
	for (int k = 0; k < this->boundaryCoords.size(); k++) {
		polyX[k] = this->boundaryCoords[k].x;
		polyY[k] = this->boundaryCoords[k].z;
	}
	int   i, j = polySides - 1;
	bool  oddNodes = false;

	float x = point.x;
	float y = point.z;

	for (i = 0; i<polySides; i++) {
		if (polyY[i]<y && polyY[j] >= y
			|| polyY[j]<y && polyY[i] >= y) {
			if (polyX[i] + (y - polyY[i]) / (polyY[j] - polyY[i])*(polyX[j] - polyX[i])<x) {
				oddNodes = !oddNodes;
			}
		}
		j = i;
	}
	return oddNodes;
	/*
	// Sum of angles algorithm
	// If a point is inside, then the angles it forms with all the lines on the boundary sum up to a multiple of 360.
	double sumAngle = 0.0;
	// Set the point as point b (in our ABC triangle).
	glm::vec3 b = point;
	// Iterate over the sides of the boundary polygon.
	for (int i = 0; i < this->boundaryCoords.size() - 1; i++) {
		// Set points A and C of the triangle to the line in the boundary.
		glm::vec3 a = this->boundaryCoords[i];
		glm::vec3 c = this->boundaryCoords[i + 1];

		glm::vec3 ab = b - a;
		glm::vec3 bc = c - b;

		// Find the length of vectors AB and BC
		double lenab = sqrt(ab.x*ab.x + ab.z*ab.z);
		double lenbc = sqrt(bc.x*bc.x + bc.z*bc.z);
		// Find the dot product of vectors AB and BC
		double dotabbc = ab.x*bc.x + ab.z*bc.z;
		// Find angle theta between AB and BC using arc cos.
		double theta = acos(dotabbc / (lenab*lenbc));
		// Conver theta to degrees.
		theta = theta * 180 / M_PI;
		// Add theta to the sum
		sumAngle += theta;
	}

	// Repeat for the last line
	glm::vec3 a = this->boundaryCoords[this->boundaryCoords.size()-1];
	glm::vec3 c = this->boundaryCoords[0];

	glm::vec3 ab = b - a;
	glm::vec3 bc = c - b;

	// Find the length of vectors AB and BC
	double lenab = sqrt(ab.x*ab.x + ab.z*ab.z);
	double lenbc = sqrt(bc.x*bc.x + bc.z*bc.z);
	// Find the dot product of vectors AB and BC
	double dotabbc = ab.x*bc.x + ab.z*bc.z;
	// Find angle theta between AB and BC using arc cos.
	double theta = acos(dotabbc / (lenab*lenbc));
	// Conver theta to degrees.
	theta = theta * 180 / M_PI;
	// Add theta to the sum
	sumAngle += theta;


	// Check if angle sum is divisible by 360
	//cout << b.x << " " << b.y << " " << b.z << endl;
	//cout << sumAngle << endl;
	if ((int)sumAngle % 360 == 0) {
		//cout << "Inside";
		return true;
	}
	else return false;
	
	*/
}

Formation::~Formation(void)
{
}
