#pragma once
class Agent
{
	int x, y, z;
	public:
		Agent(int, int, int);
		void printX();
		void printY();
		void printZ();
		double getX(void);
		double getY(void);
		double getZ(void);
		~Agent(void);
};

