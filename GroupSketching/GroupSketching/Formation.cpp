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
	boundary.push_back(boundary.front());
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

//Construct formation with boundary coordinates and exclusive boundary. Calculate the centre based on the input boundary and change the boundary to be relative to the centre.
Formation::Formation(vector<glm::vec3> boundary,vector<glm::vec3> exclusiveBoundary)
{
	boundary.push_back(boundary.front());
	exclusiveBoundary.push_back(exclusiveBoundary.front());
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
	this->setExclusiveBoundary(exclusiveBoundary);
}


// Populate function for boundary.
// Resamples the boundary points so that all agents are equidistant.
vector<glm::vec3> Formation::populateBoundary(double stepSize) {
	vector<glm::vec3> resampledBoundaryCoords;

	// Iterate over the boundary, keeping a running counter of the distance since the last agent point located.
	double runningDist = 0;
	for (int i = 0; i < this->boundaryCoords.size() - 1; i++) {
		double startX = this->boundaryCoords[i].x;
		double startZ = this->boundaryCoords[i].z;
		double lineLength = sqrt((this->boundaryCoords[i].x - this->boundaryCoords[i+1].x)*(this->boundaryCoords[i].x - this->boundaryCoords[i+1].x) + (this->boundaryCoords[i].z - this->boundaryCoords[i+1].z)*(this->boundaryCoords[i].z - this->boundaryCoords[i+1].z));
		double oldRunningDist = runningDist;
		runningDist += lineLength;
		// If the running distance is greater than the step size a new agent has been found. Calculate its position.
		while (runningDist >= stepSize) {
			// Calculate how much of the distance has been passed in the current line.
			double dist = stepSize - oldRunningDist;
			oldRunningDist = 0;
			runningDist -= stepSize;
			double endX;
			double endZ;
			
			// Calculate line vector and its length. Normalize the vector and use it to calculate the end points along the line.
			double vx = this->boundaryCoords[i+1].x - startX;
			double vz = this->boundaryCoords[i+1].z - startZ;
			double lenV = sqrt(vx*vx + vz*vz);
			vx /= lenV;
			vz /= lenV;
			endX = startX + vx * (lenV + dist);
			endZ = startZ + vz * (lenV + dist);

			// Add the point to the resampled boundary coords and change startX and startY.
			glm::vec3 bPoint = glm::vec3(endX, 0.0, endZ);
			startX = endX;
			startZ = endZ;
			resampledBoundaryCoords.push_back(bPoint);
		}
	}

	return resampledBoundaryCoords;
}

// Bounding rectangle algorithm to create oversampled point space for agents.
vector<glm::vec3> Formation::boundingRect(double stepSize) {
	// Find rectMinX/Y and MaxX/Y
	double rectMinX = this->boundaryCoords[0].x;
	double rectMaxX = this->boundaryCoords[0].x;
	double rectMinY = this->boundaryCoords[0].y;
	double rectMaxY = this->boundaryCoords[0].y;
	for (std::vector<glm::vec3>::iterator bpoint = this->boundaryCoords.begin(); bpoint != this->boundaryCoords.end(); ++bpoint) {
		if ((*bpoint).x < rectMinX)
			rectMinX = (*bpoint).x;
		if ((*bpoint).x > rectMaxX)
			rectMaxX = (*bpoint).x;
		if ((*bpoint).y < rectMinY)
			rectMinY = (*bpoint).y;
		if ((*bpoint).y > rectMaxY)
			rectMaxY = (*bpoint).y;
	}
	// Vector of filled points
	vector<glm::vec3> fPoints;
	// Iterate over points [MinX-MaxX,MinY-MaxY]. For any such point within the boundary, add it to fPoints.
	for (double y = rectMinY; y < rectMaxY; y += stepSize) {
		for (double x = rectMinX; x < rectMaxX; x += stepSize) {
			glm::vec3 point = glm::vec3(x,0.0,y);
			if (pointInBoundary(point)) {
				fPoints.push_back(point);
			}
		}
	}
	return fPoints;
}


// Flood-fill algorithm to create oversampled point space for agents.
vector<glm::vec3> Formation::floodFill(double stepSize, vector<glm::vec3>q) {
	// Vector of filled points
	vector<glm::vec3> fPoints;
	// Vector of checked points.
	vector<glm::vec3> c;
	// Set sampling rate as the step size.
	double sampleX = stepSize;
	double sampleY = stepSize;
	double sampleZ = stepSize;

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
		while (pointInBoundary(left) && (std::find(c.begin(), c.end(), left) == c.end())) {
			// Add left to fPoints.
			fPoints.push_back(left);
			//c.push_back(left);
			// Set the top and bottom points.
			glm::vec3 top = glm::vec3(left.x, left.y, left.z + sampleZ);
			glm::vec3 bottom = glm::vec3(left.x, left.y, left.z - sampleZ);
			// If top/bottom are in the bounds and have not been checked, add them to the queue and fPoints.
			if (pointInBoundary(top) && (std::find(c.begin(), c.end(), top) == c.end())) {
				//fPoints.push_back(top);
				q.push_back(top);
				//c.push_back(top);
			}
			if (pointInBoundary(bottom) && (std::find(c.begin(), c.end(), bottom) == c.end())) {
				//fPoints.push_back(bottom);
				q.push_back(bottom);
				//c.push_back(bottom);
			}
			// Move left along
			left = glm::vec3(left.x - sampleX, left.y, left.z);
		}
		// Repeat for right.
		while (pointInBoundary(right) && (std::find(c.begin(), c.end(), right) == c.end())) {
			// Add left to fPoints.
			fPoints.push_back(right);
			//c.push_back(right);
			// Set the top and bottom points.
			glm::vec3 top = glm::vec3(right.x, right.y, right.z + sampleZ);
			glm::vec3 bottom = glm::vec3(right.x, right.y, right.z - sampleZ);
			// If top/bottom are in the bounds and have not been checked, add them to the queue and fPoints.
			if (pointInBoundary(top) && (std::find(c.begin(), c.end(), top) == c.end())) {
				//fPoints.push_back(top);
				q.push_back(top);
				//c.push_back(top);
			}
			if (pointInBoundary(bottom) && (std::find(c.begin(), c.end(), bottom) == c.end())) {
				//fPoints.push_back(bottom);
				q.push_back(bottom);
				//c.push_back(bottom);
			}
			// Move right along.
			right = glm::vec3(right.x + sampleX, right.y, right.z);
		}
	}


	return fPoints;
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

	// Perform population with a very small step size.
	// Used to solve the formula sqrt(i)/b = c, where i is the number of inside agents and b is the number of boundary agents.
	// c becomes property for the shape of the formation.
	// Using the values calculated with a small step the optimal values for boundary agents and step size can be found.

	// Calculate the boundary perimeter.
	double boundaryPerimeter = 0.0;
	for (int i = 0; i < this->boundaryCoords.size() - 1; i++) {
		boundaryPerimeter += sqrt((this->boundaryCoords[i].x - this->boundaryCoords[i+1].x)*(this->boundaryCoords[i].x - this->boundaryCoords[i+1].x) + (this->boundaryCoords[i].y - this->boundaryCoords[i+1].y)*(this->boundaryCoords[i].y - this->boundaryCoords[i+1].y));
	}

	// Populate the boundary (resample)
	// Choose small step size.
	double stepSize = boundaryPerimeter/n;

	// Resample the boundary coords to boundaryAgents number of equidistant agents.
	vector<glm::vec3> resampledBoundaryCoords = this->populateBoundary(stepSize);
	// Add the starting point of the boundary to the resampled boundary coordinates.
	resampledBoundaryCoords.push_back(this->boundaryCoords[0]);

	// Flood-fill algorithm.
	// Queue of points to check.
	vector<glm::vec3> q;
	// Vector of filled points
	vector<glm::vec3> fPoints;

	// Initialize q with the boundary.
	q = resampledBoundaryCoords;
	fPoints = this->boundingRect(stepSize);
	

	// Calculate the c constant for the given shape.
	int boundaryAgents = resampledBoundaryCoords.size();
	int insideAgents = fPoints.size();
	int totalAgents = n;
	double c = (sqrt(insideAgents)) / ((double)boundaryAgents);
	// Optimise the number of boundary agents given the total agents.
	double optimalBoundaryAgents = (-1 + sqrt(1+4*totalAgents*c*c)) / (2*c*c);
	
	// Introduce a certain percent to control the step size so that more agents are generated (oversampled space).
	double percentControl = 1.0;
	stepSize = (boundaryPerimeter*percentControl) / optimalBoundaryAgents;

	// Repeat populate with the now optimal step size.
	// Resample the boundary coords to boundaryAgents number of equidistant agents.
	resampledBoundaryCoords.clear();
	resampledBoundaryCoords = this->populateBoundary(stepSize);
	// Add the starting point of the boundary to the resampled boundary coordinates.
	resampledBoundaryCoords.push_back(this->boundaryCoords[0]);

	// Flood-fill algorithm.
	// Queue of points to check.
	q.clear();
	// Vector of filled points
	fPoints.clear();

	// Initialize q with the boundary.
	q = resampledBoundaryCoords;
	fPoints = this->boundingRect(stepSize);
	

	// Choose the agents from the oversampled list
	// Take away the number of boundary agents to figure out how many agents will be inside the boundary.
	int insideN = n - resampledBoundaryCoords.size();
	
	//cout << insideN << endl;
	if (insideN > 0) {
		// Add the resampled boundary as the agent coords.
		for (std::vector<glm::vec3>::iterator bpoint = resampledBoundaryCoords.begin(); bpoint != resampledBoundaryCoords.end(); ++bpoint) {
			// Convert to coordinates relative to centre.
			this->agentCoords.push_back((*bpoint));
		}
		
		// Select random coordinates
		vector<int> t;
		int cnt = insideN;

		if (fPoints.size() > 0) {
			while (cnt > 0) {
				int ran = rand() % fPoints.size();
				if ((std::find(t.begin(), t.end(), ran) == t.end())) {
					this->agentCoords.push_back(fPoints[ran]);
					t.push_back(ran);
					cnt--;
				}
				
			}
		}
	}

	else {
		// Add the resampled boundary as the agent coords.
		//cout << "Outside" << endl;
		// Select random coordinates
		vector<int> t;
		int cnt = n;
		if (fPoints.size() > 0) {
			while (cnt > 0) {
				int ran = rand() % resampledBoundaryCoords.size();
				if ((std::find(t.begin(), t.end(), ran) == t.end())) {
					this->agentCoords.push_back(resampledBoundaryCoords[ran]);
					t.push_back(ran);
					cnt--;
				}
			}
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
// Assumes all agents are within the boundary.
// Changes real coordinates to relative ones.
void Formation::populate(vector<glm::vec3> coords)
{
	// Change agent positions to relative ones.
	glm::vec3 cent = this->getCentre();
	for (int i = 0; i < coords.size(); i++) {
		coords[i].x -= cent.x;
		coords[i].y -= cent.y;
		coords[i].z -= cent.z;
	}

	// Add agents to the formation.
	this->setAgentCoords(coords);
}


// Check if a point is in the bounds. Using Ray casting algorithm.
bool Formation::pointInBoundary(glm::vec3 point)
{
	// Ray casting algorithm
	// Count how many times a constant ray from the point intercepts the polygon.
	// If the number is odd, then the point is inside. Outside otherwise.
	// NB: This algorithm only works for 2-D coordinates. Need angle sum algorithm for 3-D.
	
	// If an exclusive boundary is present, check that point is not inside it.
	if (this->exclusiveBoundaryCoords.size() > 0) {
		int boundarySides = this->exclusiveBoundaryCoords.size();
		float boundaryX[1024];
		float boundaryY[1024];
		// Convert x-z space to x-y space
		for (int k = 0; k < this->exclusiveBoundaryCoords.size(); k++) {
			boundaryX[k] = this->exclusiveBoundaryCoords[k].x;
			boundaryY[k] = this->exclusiveBoundaryCoords[k].z;
		}
		int   i, j = boundarySides - 1;
		bool  oddNodes = false;

		float x = point.x;
		float y = point.z;

		for (i = 0; i<boundarySides; i++) {
			if (boundaryY[i]<y && boundaryY[j] >= y
				|| boundaryY[j]<y && boundaryY[i] >= y) {
				if (boundaryX[i] + (y - boundaryY[i]) / (boundaryY[j] - boundaryY[i])*(boundaryX[j] - boundaryX[i])<x) {
					oddNodes = !oddNodes;
				}
			}
			j = i;
		}
		if (oddNodes)
			return false;
	}

	// If the point is not inside the exclusive boundary (or such a boundary is not present), proceed as normal.
	int boundarySides = this->boundaryCoords.size();
	float boundaryX[1024];
	float boundaryY[1024];
	// Convert x-z space to x-y space
	for (int k = 0; k < this->boundaryCoords.size(); k++) {
		boundaryX[k] = this->boundaryCoords[k].x;
		boundaryY[k] = this->boundaryCoords[k].z;
	}
	int   i, j = boundarySides - 1;
	bool  oddNodes = false;

	float x = point.x;
	float y = point.z;

	for (i = 0; i<boundarySides; i++) {
		if (boundaryY[i]<y && boundaryY[j] >= y
			|| boundaryY[j]<y && boundaryY[i] >= y) {
			if (boundaryX[i] + (y - boundaryY[i]) / (boundaryY[j] - boundaryY[i])*(boundaryX[j] - boundaryX[i])<x) {
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

// Return the boundary coordinates in real world space.
vector<glm::vec3> Formation::getExclusiveBoundary()
{
	vector<glm::vec3> realCoords;
	// Iterate over the boundary coords
	for (std::vector<glm::vec3>::iterator bpoint = this->exclusiveBoundaryCoords.begin(); bpoint != this->exclusiveBoundaryCoords.end(); ++bpoint) {
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

// Setter function for the exdclusive boundary coordinates.
void Formation::setExclusiveBoundary(vector<glm::vec3> coords)
{
	this->exclusiveBoundaryCoords = coords;
}

// Setter function for the centre.
void Formation::setCentre(glm::vec3 cen)
{
	this->centre = cen;
}

Formation::~Formation(void)
{
}
