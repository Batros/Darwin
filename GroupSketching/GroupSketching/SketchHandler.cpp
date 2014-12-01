#include "stdafx.h"
#include "SketchHandler.h"


SketchHandler::SketchHandler(void) {

}


SketchHandler::~SketchHandler(void) {

}

Formation* SketchHandler::processFormation(vector<glm::vec3> stroke) {
	Formation* formation = new Formation(stroke);
	return formation;
}

Path SketchHandler::processPath(vector<glm::vec3> stroke) {
	return stroke;
}