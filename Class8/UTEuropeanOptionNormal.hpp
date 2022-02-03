/* UTEuropeanOptionNormal.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_EUROPEAN_OPTION_NORMAL_H
#define UT_EUROPEAN_OPTION_NORMAL_H

#include "UTEuropeanOptionBase.hpp"


class UTEuropeanOptionNormal : public UTEuropeanOptionBase
{
public:

	// Destructor
	~UTEuropeanOptionNormal();

	// Constructor
	UTEuropeanOptionNormal();
	UTEuropeanOptionNormal(
		double dForward,
		double dStrike,
		double dTimeToExpiry,
		double dSigma = 0.0);  // This is a Normal (Gaussian) Volatility !!!

public:

	// The Option Premium
	virtual double premium(UT_CallPut callPut) const;

	// The Option Delta : dPremium/dForward
	virtual double delta(UT_CallPut callPut) const;

	// The Option Gamma : dDelta/dForward
	virtual double gamma(UT_CallPut callPut) const;

	// The Option Vega : dPremium/dSigma
	virtual double vega(UT_CallPut callPut) const;

	// The Option Theta : -dPremium/dTime
	virtual double theta(UT_CallPut callPut) const;

	// Overwrites the sigma in the Option Pricer -- updates all that has to be updated (called by the solvers)
	virtual void overwriteSigma(double newSigma);

private:

	// Called by constructor - Calculates all the intermediate results and numbers
	void preliminaryCalculations();

	// Private Data

	// Member variables (pre-calculated data)

	// This is distance = (F - K) / stdDev;
	double myDistance;
	double myCumNormD;
	double myCumNormDMinusOne;
	double myGaussD;
	double myDDistanceDSigma;

};

///////////////////////////////////////////////////////////////////////////////
#endif // UT_EUROPEAN_OPTION_LOGNORMAL_H
