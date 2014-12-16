#include "stdafx.h"
#include "SketchHandler.h"


SketchHandler::SketchHandler(void) {

}


SketchHandler::~SketchHandler(void) {

}

Formation* SketchHandler::processFormation(vector<glm::vec3> stroke) {
	vector<glm::vec3> newStroke;
	newStroke.push_back(stroke[0]);
	for(vector<glm::vec3>::size_type i = 1; i < stroke.size(); i++) {
		double dist = glm::length(stroke[i] - newStroke[newStroke.size()-1]);
		if (dist > 1) {
			newStroke.push_back(stroke[i]);
		}
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