#include "stdafx.h"
#include "SketchHandler.h"


SketchHandler::SketchHandler(void) {

}


SketchHandler::~SketchHandler(void) {

}

Formation* SketchHandler::processFormation(vector<glm::vec3> stroke) {
<<<<<<< HEAD
	// Distance-relative resampling
=======
	// Distance-variable resampling
>>>>>>> 553ad2f0cf5c5bc394bb7712041db4652e25c61f
	vector<glm::vec3> newStroke;
	newStroke.push_back(stroke[0]);
	for(vector<glm::vec3>::size_type i = 1; i < stroke.size(); i++) {
		double dist = glm::length(stroke[i] - newStroke[newStroke.size()-1]);
		if (dist > 1) {
			newStroke.push_back(stroke[i]);
		}
	}
<<<<<<< HEAD

	bool lineCross = false;
	int firstLine = 0;
	int secondLine = 0;
	glm::vec3 intersect = glm::vec3(0, 0, 0);
	for(vector<glm::vec3>::size_type j = 1; j < newStroke.size(); j++) {
		for(vector<glm::vec3>::size_type k = (j+2); k < newStroke.size(); k++) {
			float m_1, m_2, c_1, c_2;
			m_1 = ((newStroke[j].z - newStroke[j-1].z) / (newStroke[j].x - newStroke[j-1].x));
			m_2 = ((newStroke[k].z - newStroke[k-1].z) / (newStroke[k].x - newStroke[k-1].x));

			c_1 = newStroke[j].z - (m_1 * newStroke[j].x);
			c_2 = newStroke[k].z - (m_2 * newStroke[k].x);

			intersect.x = (c_2 - c_1) / (m_1 - m_2);
			intersect.z = ((m_1 * c_2) - (m_2 * c_1)) / (m_1 - m_2);
			intersect.y = newStroke[j].y;

=======
	
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
>>>>>>> 553ad2f0cf5c5bc394bb7712041db4652e25c61f
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

<<<<<<< HEAD
=======
			// If it is then check the second line segment before flagging crossing lines
>>>>>>> 553ad2f0cf5c5bc394bb7712041db4652e25c61f
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
<<<<<<< HEAD
		newStroke.erase(newStroke.begin(), newStroke.begin()+firstLine);
=======
		newStroke.erase(newStroke.begin(), newStroke.begin()+(firstLine-1));
		newStroke[0] = intersect;
>>>>>>> 553ad2f0cf5c5bc394bb7712041db4652e25c61f
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

Path SketchHandler::processPath(vector<glm::vec3> stroke) {
	return stroke;
}