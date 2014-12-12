/* I declare that this code is my own work and,
except where specified, all code has been written
by me*/
/* Author Thomas Allen
tallen1@sheffield.ac.uk */

#ifndef __MYMATHS_H_INCLUDED__
#define __MYMATHS_H_INCLUDED__

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

/*
I didn't find this function anywhere so I just defined a function for converting to radians.
This is only used in Player for calculations involving looking directions.
*/
double radsOf (double angle) {
	return (angle*(M_PI/180.0));
}

#endif // __MYMATHS_H_INCLUDED__