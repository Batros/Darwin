#include "stdafx.h"
#include "SketchHandler.h"


SketchHandler::SketchHandler(void) {

}


SketchHandler::~SketchHandler(void) {

}

Formation* SketchHandler::processFormation(vector<glm::vec3> stroke) {
	// Distance-variable resampling
	vector<glm::vec3> newStroke;
	newStroke.push_back(stroke[0]);
	for(vector<glm::vec3>::size_type i = 1; i < stroke.size(); i++) {
		double dist = glm::length(stroke[i] - newStroke[newStroke.size()-1]);
		if (dist > 1) {
			newStroke.push_back(stroke[i]);
		}
	}
	
	// Line crossing handler
	bool lineCross = false;
	glm::vec3 intersect = glm::vec3(0, 0, 0);
	int firstLine = 0;
	int secondLine = 0;
	for(vector<glm::vec3>::size_type j = 1; j < newStroke.size(); j++) {
		for(vector<glm::vec3>::size_type k = (j+2); k < newStroke.size(); k++) {
			float m_1, m_2, c_1, c_2;
			m_1 = (newStroke[j].z - newStroke[j-1].z) / (newStroke[j].x - newStroke[j-1].x);
			m_2 = (newStroke[k].z - newStroke[k-1].z) / (newStroke[k].x - newStroke[k-1].x);

			c_1 = (newStroke[j].z - (m_1 * newStroke[j].x));
			c_2 = (newStroke[k].z - (m_2 * newStroke[k].x));

			intersect.x = (c_2 - c_1) / (m_1 - m_2);
			intersect.z = ((m_1 * c_2 - m_2 * c_1)) / (m_1 - m_2);
			intersect.y = newStroke[j].y;

			// Check if intesect point is on the first line segment
			bool intersectOnFirstLine = false;
			if (intersect.x <= newStroke[j].x) {
				if (intersect.x >= newStroke[j-1].x) {
					intersectOnFirstLine = true;
				}
			}
			else {
				if (intersect.x <= newStroke[j-1].x) {
					intersectOnFirstLine = true;
				}
			}
			// If it is then check the second line segment before flagging crossing lines
			if (intersectOnFirstLine) {
				if (intersect.x <= newStroke[k].x) {
					if (intersect.x >= newStroke[k-1].x) {
						lineCross = true;
						firstLine = j;
						secondLine = k;
						break;
					}
				}
				else {
					if (intersect.x <= newStroke[k-1].x) {
						lineCross = true;
						firstLine = j;
						secondLine = k;
						break;
					}
				}
			}
		}
	}

	if (lineCross) {
		newStroke.erase(newStroke.begin()+secondLine, newStroke.end());
		newStroke.erase(newStroke.begin(), newStroke.begin()+firstLine);
	}

	Formation* formation = new Formation(newStroke);
	return formation;

	/* FIXED RESAMPLING
	int resample = 3;
	vector<glm::vec3> newStroke;
	for(vector<glm::vec3>::size_type i = 0; i != stroke.size(); i++) {
		if (resample==3) {
			resample = 0;
			newStroke.push_back(stroke[i]);
		}
		else {
			resample++;
		}
	}
	Formation* formation = new Formation(newStroke);
	return formation;
	*/
}

bool SketchHandler::processSubFormation(vector<glm::vec3> stroke, vector<glm::vec3> parent) {
	bool subInsideParent = true;
	for(vector<glm::vec3>::size_type i = 0; i < stroke.size(); i++) {
		if (!pointInBoundary(stroke[i], parent)) {
			subInsideParent = false;
			break;
		}
	}
	return subInsideParent;
}

bool SketchHandler::pointInBoundary(glm::vec3 point, vector<glm::vec3> boundary) {
	bool inBoundary = false;
	float m_point = 1;
	float c_point = point.z - (m_point * point.x);
	for(vector<glm::vec3>::size_type i = 1; i < boundary.size(); i++) {
		float m_line, c_line;
		m_line = (boundary[i].z - boundary[i-1].z) / (boundary[i].x - boundary[i-1].x);
		c_line = (boundary[i].z - (m_line * boundary[i].x));

		glm::vec3 intersect = glm::vec3(0, 0, 0);

		intersect.x = (c_line - c_point) / (m_point - m_line);
		intersect.z = ((m_point * c_line) - (m_line * c_point)) / (m_point - m_line);
		intersect.y = boundary[i].y;

		// Only look one side of the point
		if (intersect.x > point.x) {
			// Check if intesect point is on the line segment
			if (intersect.x <= boundary[i].x) {
				if (intersect.x >= boundary[i-1].x) {
					inBoundary = !inBoundary;
				}
			}
			else {
				if (intersect.x <= boundary[i-1].x) {
					inBoundary = !inBoundary;
				}
			}
		}
	}
	return inBoundary;
}

Path SketchHandler::processPath(vector<glm::vec3> stroke, vector<glm::vec3> f1, vector<glm::vec3> f2) {
	bool startInBoundary = pointInBoundary(stroke[0], f1);
	bool endInBoundary = pointInBoundary(stroke[stroke.size()-1], f2);

	if (startInBoundary && endInBoundary) {
		return stroke;
	}
	else {
		vector<glm::vec3> blankPath;
		return blankPath;
	}
}