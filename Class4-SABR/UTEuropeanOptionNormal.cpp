/* UTEuropeanOptionNormal.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTMathFunctions.hpp"
#include "UTEuropeanOptionNormal.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

// Destructor
UTEuropeanOptionNormal::~UTEuropeanOptionNormal()
{
}
///////////////////////////////////////////////////////////////////////////////

// Default Constructor
UTEuropeanOptionNormal::UTEuropeanOptionNormal()
	: UTEuropeanOptionBase()
{
}
///////////////////////////////////////////////////////////////////////////////

// Constructor
UTEuropeanOptionNormal::UTEuropeanOptionNormal(
	double dForward,
	double dStrike,
	double dTimeToExpiry,
	double dSigma)  // This is a Normal (Gaussian) Volatility !!!
	: UTEuropeanOptionBase(dForward, dStrike, dTimeToExpiry, dSigma)
{
	// Do the preliminary  calclulations, based upon the value of sigma
	preliminaryCalculations();

}

///////////////////////////////////////////////////////////////////////////////

// Overwrites the sigma in the Option Pricer -- updates all that has to be updated (called by the solvers)
void
UTEuropeanOptionNormal::overwriteSigma(double newSigma)
{
	UTEuropeanOptionBase::overwriteSigma(newSigma);
	preliminaryCalculations();
}


///////////////////////////////////////////////////////////////////////////////

// Calculates all the intermediate results and numbers, based upon the value of sigma...
void
UTEuropeanOptionNormal::preliminaryCalculations()
{
	// First, treat the case when the volatility or time to expiry are zero
	if (stdDev() < ourEpsilon)
	{
		// The Forward is above the Strike
		if (forward() >= strike())
		{
			myDistance = ourInfiniteDistance;
			myCumNormD = 1.0;
			myGaussD = 0.0;
			myDDistanceDSigma = -ourInfiniteDistance;
			myCumNormDMinusOne = 0.0;
		}
		else
			// The Strike is above the Forward
		{
			myDistance = -ourInfiniteDistance;
			myCumNormD = 0.0;
			myGaussD = 0.0;
			myDDistanceDSigma = +ourInfiniteDistance;
			myCumNormDMinusOne = -1.0;
		}
	}
	else
		// Generic Case
	{
		myDistance = (forward() - strike()) / stdDev();
		myCumNormD = UTMathFunctions::cumulativeNormal(myDistance);
		myCumNormDMinusOne = -UTMathFunctions::cumulativeNormal(-myDistance);
		myGaussD = UTMathFunctions::normalDensity(myDistance);
		myDDistanceDSigma = -myDistance / sigma();
	}
}

///////////////////////////////////////////////////////////////////////////////
// The Option Premium
double
UTEuropeanOptionNormal::premium(UT_CallPut callPut) const
{
	// Check the option has not expired -- If it has: return 0.0
	if (optionExpired())
	{
		return 0.0;
	}

	double premiumRtn = 0.0;

	double intrinsic = forward() - strike();
	if (callPut == UT_CALL)
	{
		premiumRtn = stdDev() * myGaussD + intrinsic * myCumNormD;
	}
	else if (callPut == UT_PUT)
	{
		premiumRtn = intrinsic * myCumNormDMinusOne +stdDev() * myGaussD;
	}
	else if (callPut == UT_STRADDLE)
	{
		premiumRtn = 2.0 * stdDev() * myGaussD + intrinsic * (2.0 * myCumNormD - 1.0);
	}
	else
	{
		throw runtime_error("UTEuropeanOptionNormal: Unknown option Type");
	}

	// Returns the Premium
	return UTMathFunctions::max(premiumRtn, UTEuropeanOptionBase::intrinsic(callPut));
}

///////////////////////////////////////////////////////////////////////////////
// The Option Vega : dPremium/dSigma
double
UTEuropeanOptionNormal::vega(UT_CallPut callPut) const
{
	// Check the option has not expired -- If it has: return 0.0
	if (optionExpired())
	{
		return 0.0;
	}

	double vegaRtn = 0.0;

	vegaRtn = sqrtTimeToExpiry() * myGaussD;

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
UTEuropeanOptionNormal::delta(UT_CallPut callPut) const
{
	// Check the option has not expired -- If it has: return 0.0
	if (optionExpired())
	{
		return 0.0;
	}

	double deltaRtn = 0.0;

	if (callPut == UT_CALL)
	{
		deltaRtn = myCumNormD;
	}
	else if (callPut == UT_PUT)
	{
		deltaRtn = myCumNormDMinusOne;
	}
	else if (callPut ==UT_STRADDLE)
	{
		deltaRtn = myCumNormD + myCumNormDMinusOne;
	}

	//Return Delta
	return deltaRtn;
}

///////////////////////////////////////////////////////////////////////////////
// The Option Gamma : dDelta/dForward
double
UTEuropeanOptionNormal::gamma(UT_CallPut callPut) const
{
	// Check the option has not expired -- If it has: return 0.0
	if (optionExpired())
	{
		return 0.0;
	}

	double gammaRtn;

	//
	// Only divide by the stdDev if it is non-zero (in which case, the Gamma will be either 0.0 or infinite)
	if (stdDev() != 0.0)
	{
		gammaRtn = myGaussD / stdDev();
	}
	else
	{
		gammaRtn = 0.0;
	}

	if (callPut == UT_STRADDLE)
	{
		gammaRtn *= 2.0;
	}

	// Returns the Gamma
	return gammaRtn;
}

///////////////////////////////////////////////////////////////////////////////
// The Option Theta : -dPremium/dTime
double
UTEuropeanOptionNormal::theta(UT_CallPut callPut) const
{
	// Check the option has not expired -- If it has: return 0.0
	if (optionExpired())
	{
		return 0.0;
	}

	double thetaRtn;

	if (sqrtTimeToExpiry() != 0.0)
	{
		thetaRtn = -0.5 * sigma() * myGaussD / sqrtTimeToExpiry();
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
