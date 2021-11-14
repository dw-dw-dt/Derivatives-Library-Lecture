/* UTEuropeanOptionBase.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include <float.h>
#include <iostream>

#include "UTMathFunctions.hpp"
#include "UTEuropeanOptionLogNormal.hpp"
#include "UTEuropeanOptionBase.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// UTEuropeanOption (static class)

// Static functions

// Returns the intrinsic value of the option
double
UTEuropeanOption::intrinsicValue(double forward, double strike, UT_CallPut callPut)
{
	double intrinsic = 0.0;
	switch (callPut)
	{
	case UT_CALL:     intrinsic = UTMathFunctions::max(forward - strike, 0.0); break;
	case UT_PUT:      intrinsic = UTMathFunctions::max(strike - forward, 0.0); break;
	case UT_STRADDLE: intrinsic = fabs(strike - forward); break;
	default: throw runtime_error("UTEuropeanOption: unknown option type.");
	}
	return intrinsic;
}

///////////////////////////////////////////////////////////////////////////////

// Transformation functions

// Returns the lognormal volatility equivalent to an input normal volatility 
double
UTEuropeanOption::normalToLogNormal(
double forward,
double strike,
double timeToExpiry,
double normVolatility)
{
	double distance = forward - strike;

	if (fabs(distance) < DBL_EPSILON)
	{
		return normVolatility / forward;
	}

	double logDistance = log(forward / strike);

	return logDistance / distance * normVolatility;
}

///////////////////////////////////////////////////////////////////////////////

// Returns the normal volatility equivalent to an input lognormal volatility
double
UTEuropeanOption::logNormalToNormal(
double forward,
double strike,
double timeToExpiry,
double logVolatility)
{
	double distance = (forward - strike);

	if (fabs(distance) < DBL_EPSILON)
	{
		return logVolatility*forward;
	}

	double logDistance = log(forward / strike);

	return distance / logDistance * logVolatility;

}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
double
UTSolveForImpliedVolatility::valueSlopeRatio(double abscissa)
{
	myOption.overwriteSigma(abscissa);
	double numerator = myOption.premium(myCallPut) - myTarget;
	double denominator = myOption.vega(myCallPut);

	return numerator / denominator;
}

/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Initialization of static variables
const double UTEuropeanOptionBase::ourInfiniteDistance = 333333333333.33;
const double UTEuropeanOptionBase::ourEpsilon = 2.0*DBL_EPSILON;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Destructor
UTEuropeanOptionBase::~UTEuropeanOptionBase()
{
}

///////////////////////////////////////////////////////////////////////////////

// Default Constructor
UTEuropeanOptionBase::UTEuropeanOptionBase()
	: myForward(0.0),
	myStrike(0.0),
	myTimeToExpiry(0.0),
	mySigma(0.0)
{
}

///////////////////////////////////////////////////////////////////////////////

// Constructor
UTEuropeanOptionBase::UTEuropeanOptionBase(
	double dForward,
	double dStrike,
	double dTimeToExpiry,
	double dSigma)
	: myForward(dForward),
	myStrike(dStrike),
	myTimeToExpiry(dTimeToExpiry),
	mySigma(dSigma)
{
	// A flag to check if the option has expired
	if (myTimeToExpiry < 0.0)
	{
		myOptionExpired = true;
		myTimeToExpiry = 0.0;
	}
	else
	{
		myOptionExpired = false;
	}

	// Make sure that Sigma is a positive number
	if (mySigma < 0.0)
	{
		throw runtime_error("UTEuropeanOptionBase: cannot have a negative forward for a LogNormal model!");
	}

	// Do the pre-calclulations
	preliminaryCalculations();

}

///////////////////////////////////////////////////////////////////////////////

// Calculates all the intermediate results and numbers, based upon the value of sigma...
void
UTEuropeanOptionBase::preliminaryCalculations()
{
	myVariance = mySigma * mySigma * myTimeToExpiry;
	myStdDev = sqrt(myVariance);
	mySqrtTimeToExpiry = sqrt(myTimeToExpiry);
}

///////////////////////////////////////////////////////////////////////////////
double
UTEuropeanOptionBase::intrinsic(UT_CallPut callPut) const
{
	double intrinsic = 0.0;
	switch (callPut)
	{
	case UT_CallPut::UT_CALL:
		intrinsic = UTMathFunctions::max(myForward - myStrike, 0.0);
		break;
	case UT_CallPut::UT_PUT:
		intrinsic = UTMathFunctions::max(myStrike - myForward, 0.0);
		break;
	case UT_CallPut::UT_STRADDLE:
		intrinsic = fabs(myStrike - myForward);
		break;
	default:
		throw runtime_error("UTEuropeanOptionLogNormal: unknown option type.");

		break;
	}
	return intrinsic;
}

///////////////////////////////////////////////////////////////////////////////

// Overwrites the sigma in the Option Pricer -- updates all that has to be updated (called by the solvers)
void
UTEuropeanOptionBase::overwriteSigma(double newSigma)
{
	mySigma = newSigma;
	preliminaryCalculations();
}

///////////////////////////////////////////////////////////////////////////////

void 
UTEuropeanOptionBase::impliedSigmaInputCheck(double targetPremium, UT_CallPut callPut) const
{
	// Check the premium is not too high
	switch (callPut)
	{
	case UT_CALL:
		if (targetPremium >= fabs(forward()))
			throw runtime_error("UTEuropeanOptionBase::impliedVolatility: the given premium is higher than the option price for infinite implied volatility and thus there is no implied volatility that matches this price.");
		break;
	case UT_PUT:
		if (targetPremium>fabs(strike()))
			throw runtime_error("UTEuropeanOptionBase::impliedVolatility: the given premium is higher than the option price for infinite implied volatility and thus there is no implied volatility that matches this price.");
		break;
	case UT_STRADDLE:
		if (targetPremium>fabs(strike()) + fabs(forward()))
			throw runtime_error("UTEuropeanOptionBase::impliedVolatility: the given premium is higher than the option price for infinite implied volatility and thus there is no implied volatility that matches this price.");
		break;
	default:
		throw runtime_error("UTEuropeanOptionBase: only Call or Put is allowed as the option type for the calculation of implied volatilities.");
	}

	// Check the premium is not too low (below intrinsic)
	if (targetPremium < intrinsic(callPut))
	{
		throw runtime_error("UTEuropeanOptionBase::impliedVolatility: cannot calculate implied volatility if specified premium is less than the minimum possible price.");
	}
}

///////////////////////////////////////////////////////////////////////////////

// Calculates IN SITU the Implied Sigma to match the passed option price....
double
UTEuropeanOptionBase::impliedSigma(double targetPremium, UT_CallPut callPut)
{
	unsigned uMaximumNumberOfIterations = 20;
	double dAbsoluteAccuracy = ourEpsilon;

	impliedSigmaInputCheck(targetPremium, callPut);

	UTSolveForImpliedVolatility impVol(*this, callPut, targetPremium);

	// The first value for the guess: the value of sigma stored in this class
	double rtn = impVol.root(mySigma, dAbsoluteAccuracy, uMaximumNumberOfIterations);

	// Overwrites the current sigma value by the new one.
	overwriteSigma(rtn);

	return rtn;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////