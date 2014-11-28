#include "stdafx.h"
#include <iostream>
#include "Agent.h"
using namespace std;


Agent::Agent(int xCoord, int yCoord, int zCoord) {
	x = xCoord;
	y = yCoord;
	z = zCoord;
}


Agent::~Agent(void)
{
}

void Agent::printX () {cout << x << endl;}
void Agent::printY () {cout << y << endl;}
void Agent::printZ () {cout << z << endl;}

double Agent::getX () {return x;}
double Agent::getY () {return y;}
double Agent::getZ () {return z;}