/* UTBisection.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_BISECTION_H
#define UT_BISECTION_H

#include <float.h>
#include <cmath>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Polymorphism solution

class UTBisection
{

public:

	//
	// f(x) is the value of the function at abscissa x whose zeros we are looking for.
	// The error function must be overloaded.
	virtual double error(double x) = 0;

	//
	// Virtual destructor.
	virtual ~UTBisection(){}

	//
	// NOTE: this root finder only considers convergence in the abscissa, i.e. it iterates until further progress in the argument
	// is smaller than absoluteAccuracy.
	//
	double root(double lowerBound, double upperBound, double absoluteAccuracy = 15 * DBL_EPSILON, unsigned long maxIterations = (DBL_DIG * 10) / 3);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif // UT_BISECTION_H
