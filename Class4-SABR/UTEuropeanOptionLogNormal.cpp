/* UTEuropeanOptionLogNormal.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include <float.h>
#include <iostream>

#include "UTMathFunctions.h"
#include "UTEuropeanOptionLogNormal.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

// Destructor
UTEuropeanOptionLogNormal::~UTEuropeanOptionLogNormal()
{
}
///////////////////////////////////////////////////////////////////////////////

// Default constructor
UTEuropeanOptionLogNormal::UTEuropeanOptionLogNormal()
	: UTEuropeanOptionBase()
{
}
///////////////////////////////////////////////////////////////////////////////

// Constructor
UTEuropeanOptionLogNormal::UTEuropeanOptionLogNormal(
	double dForward,
	double dStrike,
	double dTimeToExpiry,
	double dSigma)  // This is a LogNormal Volatility !!!
	: UTEuropeanOptionBase(dForward, dStrike, dTimeToExpiry, dSigma)
{
	// Make sure the Forward is STRICTLY positive
	if (dForward <= 0.0)
	{
		throw runtime_error("UTEuropeanOptionLogNormal: cannot have a negative forward for a LogNormal model!");
	}

	// Now do the preliminary calculations
	preliminaryCalculations();
}

///////////////////////////////////////////////////////////////////////////////

// Overwrites the sigma in the Option Pricer -- updates all that has to be updated (called by the solvers)
void
UTEuropeanOptionLogNormal::overwriteSigma(double newSigma)
{
	UTEuropeanOptionBase::overwriteSigma(newSigma);
	preliminaryCalculations();
}

///////////////////////////////////////////////////////////////////////////////

// Calculates all the intermediate results and numbers, based upon the value of sigma...
void
UTEuropeanOptionLogNormal::preliminaryCalculations()
{
	// First, treat the case when the volatility or time to expiry are zero
	if (stdDev() < ourEpsilon)
	{
		// The Forward is above the Strike
		if (forward() >= strike())
		{
			myDistance = ourInfiniteDistance;
			myD1 = ourInfiniteDistance;
			myD2 = ourInfiniteDistance;
			myCumNormD1 = 1.0;
			myCumNormD2 = 1.0;
			myCumNormD1MinusOne = 0.0;
			myCumNormD2MinusOne = 0.0;
			myGaussD1 = 0.0;
			myDD1DSigma = -ourInfiniteDistance;
		}
		else // The Strike is above the Forward
		{
			myDistance = ourInfiniteDistance;
			myD1 = -ourInfiniteDistance;
			myD2 = -ourInfiniteDistance;
			myCumNormD1 = 0.0;
			myCumNormD2 = 0.0;
			myCumNormD2MinusOne = -1.0;
			myCumNormD1MinusOne = -1.0;
			myGaussD1 = 0.0;
			myDD1DSigma = ourInfiniteDistance;
		}
	}
	else  // Generic Case
	{
		if ((forward()  < ourEpsilon) && (strike()  < ourEpsilon))
		{
			myDistance = myD1 = myD2 = myCumNormD1 = myCumNormD2 = myGaussD1 = .0;
			myCumNormD2MinusOne = -1.0;
			myCumNormD1MinusOne = -1.0;
		}
		else
		{
			if (strike() <= 0.0)
			{
				myDistance = ourInfiniteDistance;
			}
			else
			{
				myDistance = log(forward() / strike()) / stdDev();
			}
			myD1 = myDistance + .5 * stdDev();
			myD2 = myDistance - .5 * stdDev();
			myCumNormD1 = UTMathFunctions::cumulativeNormal(myD1);
			myCumNormD2 = UTMathFunctions::cumulativeNormal(myD2);
			myCumNormD1MinusOne = -UTMathFunctions::cumulativeNormal(-myD1);
			myCumNormD2MinusOne = -UTMathFunctions::cumulativeNormal(-myD2);
			myGaussD1 = UTMathFunctions::normalDensity(myD1);
			myDD1DSigma = -myD2 / sigma();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// The Option Premium 
double
UTEuropeanOptionLogNormal::premium(UT_CallPut callPut) const
{
	// Check the option has not expired -- If it has: return 0.0
	if (optionExpired())
	{
		return 0.0;
	}

	double premiumRtn;

	if (callPut == UT_CALL)  // Call optoin
	{
		premiumRtn = forward() * myCumNormD1 - strike() * myCumNormD2;
	}
	else if (callPut == UT_PUT)  //Put option
	{
		premiumRtn = forward() * myCumNormD1MinusOne - strike() * myCumNormD2MinusOne;
	}
	else if (callPut == UT_STRADDLE)  //straddle
	{
		premiumRtn = forward() * myCumNormD1 - strike() * myCumNormD2;
		premiumRtn *= 2.0;
		premiumRtn -= forward() - strike();
	}
	else
	{
		throw runtime_error( "UTEuropeanOptionLogNormal::premium: Unknown cal/put type.");
	}

	// Returns the Premium
	return UTMathFunctions::max(premiumRtn, intrinsic(callPut));
}

///////////////////////////////////////////////////////////////////////////////
// The Option Vega : dPremium/dSigma
double
UTEuropeanOptionLogNormal::vega(UT_CallPut callPut) const
{
	// Check the option has not expired -- If it has: return 0.0
	if (optionExpired())
	{
		return 0.0;
	}

	double vegaRtn = forward() * sqrtTimeToExpiry() * myGaussD1;

	if (callPut == UT_STRADDLE)
	{
		vegaRtn *= 2.0;
	}

	// Return the Vega
	return vegaRtn;
}


///////////////////////////////////////////////////////////////////////////////
// The Option Delta : dPremium/dForward
double
UTEuropeanOptionLogNormal::delta(UT_CallPut callPut) const
{
	// Check the option has not expired -- If it has: return 0.0
	if (optionExpired())
	{
		return 0.0;
	}

	double deltaRtn = 0.0;

	if (callPut == UT_CALL)
	{
		deltaRtn = myCumNormD1;
	}
	else if (callPut == UT_PUT)
	{
		deltaRtn = myCumNormD1MinusOne;
	}
	else if (callPut == UT_STRADDLE)
	{
		deltaRtn = myCumNormD1 + myCumNormD1MinusOne;
	}

	// Returns the Delta
	return deltaRtn;
}

///////////////////////////////////////////////////////////////////////////////
// The Option Gamma : dDelta/dForward
double
UTEuropeanOptionLogNormal::gamma(UT_CallPut callPut) const
{
	// Check the option has not expired -- If it has: return 0.0
	if (optionExpired())
	{
		return 0.0;
	}

	double gammaRtn = 0.0;

	// Only divide by the stdDev if it is non-zero (in which case, the Gamma will be either 0.0 or infinite)
	if (stdDev() > 0.0)
	{
		gammaRtn = myGaussD1 / forward() / stdDev();
	}

	if (callPut == UT_STRADDLE)
	{
		gammaRtn *= 2.0;
	}

	// Returns the Gamma
	return gammaRtn;
}

//////////////////////////////////////////////////////////////////////////////
// The Option Theta : -dPremium/dTime
double
UTEuropeanOptionLogNormal::theta(UT_CallPut callPut) const
{
	// Check the option has not expired -- If it has: return 0.0
	if (optionExpired())
	{
		return 0.0;
	}

	double thetaRtn;

	if (sqrtTimeToExpiry() != 0.0)
	{
		thetaRtn = -0.5 * forward() * sigma() * myGaussD1 / sqrtTimeToExpiry();
	}
	else
	{
		thetaRtn = 0.0;
	}


	if (callPut == UT_STRADDLE)
	{
		thetaRtn *= 2;
	}

	// Returns the Theta
	return thetaRtn;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////