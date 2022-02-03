/* UTNewton.h
*
* Copyright (c) 2016
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

	//
	//
	// ONLY TO BE USED FOR SUPERSMOOTH FUNCTIONS!!!
	//

	//
	// valueSlopeRatio() is the function that calculates the penalty (or error) function divided by the slope at that point.
	//
	// Newton (or Newton-Raphson) Method:
	//
	//                              f(x[n])
	//  x[n+1]      =    x[n]   -  ---------
	//                              f'(x[n])
	//
	// f(x) is the value of the function at abscissa x whose zeros we are looking for.
	//
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
