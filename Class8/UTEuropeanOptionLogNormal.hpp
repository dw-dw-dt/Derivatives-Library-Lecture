/* UTEuropeanOptionLogNormal.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_EUROPEAN_OPTION_LOGNORMAL_H
#define UT_EUROPEAN_OPTION_LOGNORMAL_H

#include "UTEuropeanOptionBase.hpp"


class UTEuropeanOptionLogNormal : public UTEuropeanOptionBase
{
public:

	// Destructor
	~UTEuropeanOptionLogNormal();

	// Constructor
	UTEuropeanOptionLogNormal();
	UTEuropeanOptionLogNormal(
		double dForward,
		double dStrike,
		double dTimeToExpiry,
		double dSigma = 0.0);  // This is a LogNormal Volatility !!!


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

	// This is distance = log(F/K) / stdDev;
	double myDistance;
	double myD1;
	double myD2;
	double myCumNormD1;
	double myCumNormD2;
	double myCumNormD1MinusOne;
	double myCumNormD2MinusOne;
	double myGaussD1;
	double myDD1DSigma;

};

#endif // UT_EUROPEAN_OPTION_LOGNORMAL_H
