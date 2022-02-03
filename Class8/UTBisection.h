/* UTBisection.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_BISECTION_H
#define UT_BISECTION_H

#include <float.h>
#include <cmath>
#include <stdexcept>

using namespace std;

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
// Template solution

template<typename T>
double bisection(T & theObject, double x1 /*low*/, double x2 /*high*/, double accuracy = 15 * DBL_EPSILON, unsigned long maxIterations = (DBL_DIG * 10) / 3)
{
	double f1 = theObject.error(x1);
	double f2 = theObject.error(x2);

	double xmid, fmid;
	for (unsigned long i = 0; i<maxIterations; ++i)
	{
		xmid = 0.5 * (x1 + x2);
		fmid = theObject.error(xmid);
		if (fmid == 0.0)
		{
			return xmid;
		}
		else if (f1*fmid < 0.0)
		{
			x2 = xmid;
			f2 = fmid;
		}
		else
		{
			x1 = xmid;
			f1 = fmid;
		}
		if (fabs(x2 - x1) <= accuracy || fmid == 0.0)
			return 0.5 * (x2 + x1);
	}

	throw runtime_error("bisection: Root search did not converge.");

	// Should never get here
	return 0.0;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif // UT_BISECTION_H
