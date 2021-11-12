/* UTMathFunctions.cpp
*
* Copyright(c) 2016
* Diva Analytics
*/

#ifndef UT_NEWTON_H
#define UT_NEWTON_H

#include <float.h>
#include <cmath>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UTNewton
{

public:

	
	virtual double valueSlopeRatio(double abscissa) = 0;

	//
	// Virtual destructor.
	virtual ~UTNewton(){}

	//
	// NOTE: this root finder only considers convergence in the abscissa, i.e. it iterates until further progress in the argument
	// is smaller than absoluteAccuracy !!!
	//
	double root(double initialGuess, double absoluteAccuracy = 15 * DBL_EPSILON, unsigned long maxIterations = (DBL_DIG * 10) / 3 )
	{
		double epsilon;
		while (fabs(epsilon = valueSlopeRatio(initialGuess)) >= absoluteAccuracy && maxIterations--)
			initialGuess -= epsilon;
		return initialGuess;
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif // UT_NEWTON_H
