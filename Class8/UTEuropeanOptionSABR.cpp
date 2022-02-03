/* UTEuropeanOptionSABR.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTMathFunctions.h"
#include "UTEuropeanOptionSABR.h"
#include "UTEuropeanOptionLogNormal.h"
#include "UTEuropeanOptionNormal.h"
#include <stdexcept>

using namespace std;

///////////////////////////////////////////////////////////////////////////////

// Destructor
UTEuropeanOptionSABR::~UTEuropeanOptionSABR()
{
}

///////////////////////////////////////////////////////////////////////////////

// Default constructor
UTEuropeanOptionSABR::UTEuropeanOptionSABR()
	: UTEuropeanOptionBase()
{
}

///////////////////////////////////////////////////////////////////////////////

// Constructor
UTEuropeanOptionSABR::UTEuropeanOptionSABR(
	double dForward,
	double dStrike,
	double dTimeToExpiry,
	double dSigma,			// This is a CEV Volatility !!!
	double dBeta,			// CEV coefficient
	double dAlpha,			// volatility of volatility
	double dRho,            // correlation
	UT_ExpansionType expansiontype)			
	: UTEuropeanOptionBase(dForward, dStrike, dTimeToExpiry, dSigma),
	myAlpha(dAlpha),
	myBeta(dBeta),
	myRho(dRho),
	myExpansionType(expansiontype)
{
	// Make sure the Forward is STRICTLY positive when expansion type is lognormal
	if (myExpansionType && forward() <= 0.0)
	{
		throw runtime_error("UTEuropeanOptionSABR :: cannot have a negative Forward with lognormal expansion");
	}

	// Make sure that the absolute value of rho is less than 1.
	if (!(fabs(myRho) < 1.0))
		throw runtime_error("UTEuropeanOptionSABR :: absolut value of correlation must be less than one");

	// Calculates (IN SITU) the equivalent volatility and Derivatives
	preliminaryCalculations();
}

///////////////////////////////////////////////////////////////////////////////
// Overwrites the sigma in the Option Pricer -- updates all that has to be updated (called by the solvers)
void
UTEuropeanOptionSABR::overwriteSigma(double newSigma)
{
	UTEuropeanOptionBase::overwriteSigma(newSigma);
	preliminaryCalculations();

}

///////////////////////////////////////////////////////////////////////////////
// Calculates all the terms needed for equivalent volatility transformations, as well as derivatives !!!!
void
UTEuropeanOptionSABR::preliminaryCalculations()
{
	// First, treat the case when the volatility or time to expiry are zero
	if (stdDev() < ourEpsilon)
	{
		myEquivalentVolatility = 0.0;

		// Re-Builds the associated underlying option pricer, using the equivalent volatility that has just been calculated
		if (myExpansionType == UT_LOGNORMAL)
		{
			myUnderlyingOption = unique_ptr<UTEuropeanOptionBase>(new UTEuropeanOptionLogNormal(forward(), strike(), timeToExpiry(), myEquivalentVolatility));
		}
		else
		{
			myUnderlyingOption = unique_ptr<UTEuropeanOptionBase>(new UTEuropeanOptionNormal(forward(), strike(), timeToExpiry(), myEquivalentVolatility));
		}
		return;
	}

	// Re-Builds the associated underlying option pricer, using the equivalent volatility that has just been calculated
	if (myExpansionType == UT_LOGNORMAL)
	{
		//These are homeworks.
		myEquivalentVolatility = 0.3; 
		myDEquivalentVolatilityDForward = 0.0; 
		myDEquivalentVolatilityDSigma = 0.0;  
		myDEquivalentVolatilityDTime = 0.0;  

		myUnderlyingOption = unique_ptr<UTEuropeanOptionBase>(new UTEuropeanOptionLogNormal(forward(), strike(), timeToExpiry(), myEquivalentVolatility));
	}
	else
	{
		//These are homeworks.
		myEquivalentVolatility = 0.3 * 0.01;  //This is a homework.
		myDEquivalentVolatilityDForward = 0.0; //This is a homework.
		myDEquivalentVolatilityDSigma = 0.0;  //This is a homework.
		myDEquivalentVolatilityDTime = 0.0;  //This is a homework.

		myUnderlyingOption = unique_ptr<UTEuropeanOptionBase>(new UTEuropeanOptionNormal(forward(), strike(), timeToExpiry(), myEquivalentVolatility));
	}
}

///////////////////////////////////////////////////////////////////////////////
// The Option Premium
double
UTEuropeanOptionSABR::premium(UT_CallPut callPut) const
{
	// Delegates
	double premiumRtn = underlyingOption().premium(callPut);

	// Returns the Premium
	return UTMathFunctions::max(premiumRtn, intrinsic(callPut));
}

///////////////////////////////////////////////////////////////////////////////
// The Option Delta : dPremium/dForward
double
UTEuropeanOptionSABR::delta(UT_CallPut callPut) const
{
	// Get the underlying delta : dUndP / dF;
	double underlyingDelta = underlyingOption().delta(callPut);

	// Get the underlying vega : dUndP / dEqSig;
	double underlyingVega = underlyingOption().vega(callPut);

	// Constructs the 'real' delta : dP/dF = dUndP/dF + dUndP/dEqSig * dEqSig/dF
	double deltaRtn = underlyingDelta + underlyingVega * DEquivalentVolatilityDForward();

	// Returns the Full Delta
	return deltaRtn;
}

///////////////////////////////////////////////////////////////////////////////
// The Option Vega : dPremium/dSigma
double
UTEuropeanOptionSABR::vega(UT_CallPut callPut) const
{
	// Get the underlying vega : dUndP / dEqSig;
	double underlyingVega = underlyingOption().vega(callPut);

	// Renormalises the Vega to get it wrt to the Real Volatility: dP/dSig = dUndP/dEqSig * dEqSig/dSig
	double vegaRtn = underlyingVega * DEquivalentVolatilityDSigma();

	// Return the Final Vega
	return vegaRtn;
}



///////////////////////////////////////////////////////////////////////////////
// The Option Theta : -dPremium/dTimeToExpiry
double
UTEuropeanOptionSABR::theta(UT_CallPut callPut) const
{
	// Get the underlying theta : -dUndP / dT;
	double underlyingTheta = underlyingOption().theta(callPut);

	// Get the underlying vega : dUndP / dEqSig;
	double underlyingVega = underlyingOption().vega(callPut);

	// Constructs the 'real' theta : -dP/dT = -dUndP/dT - dUndP/dEqSig * dEqSig/dT
	double thetaRtn = underlyingTheta - underlyingVega * DEquivalentVolatilityDTime();

	// Returns the Full Theta
	return thetaRtn;
}

///////////////////////////////////////////////////////////////////////////////
// The Option Gamma (for this Forward, Strike, Volatility type...) : dDelta/dForward
double
UTEuropeanOptionSABR::gamma(UT_CallPut callPut) const
{
	return 0.0;  //Please implement
}

///////////////////////////////////////////////////////////////////////////////
