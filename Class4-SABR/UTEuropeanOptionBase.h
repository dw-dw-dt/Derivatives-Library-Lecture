/* UTEuropeanOptionBase.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_EUROPEAN_OPTION_BASE_H
#define UT_EUROPEAN_OPTION_BASE_H

#include <vector>

#include "UTEnumn.h"
#include "UTNewton.h"

//
// UTEuropeanOption (static class)
//
class UTEuropeanOption
{
public:

	static double  intrinsicValue(double forward, double strike, UT_CallPut callPut);

	// Transformation functions

	// Returns the lognormal volatility equivalent to an input normal volatility  (accuracy to the 4th order)
	 static double logNormalToNormal(
		double forward,
		double strike,
		double timeToExpiry,
		double logVolatility);

	// Returns the normal volatility equivalent to an input lognormal volatility
	// This function is calculated as the inversed (Taylor expansion) of the previous one
	 static double normalToLogNormal(
		double forward,
		double strike,
		double timeToExpiry,
		double normVolatility);

	static const int    ourMaxExpansionIteration;
};


///////////////////////////////////////////////////////////////////////////////

class UTEuropeanOptionBase
{
public:

	// Destructor
	 virtual ~UTEuropeanOptionBase();

	 // Constructor
	 UTEuropeanOptionBase();
	 UTEuropeanOptionBase(
		double dForward,
		double dStrike,
		double dTimeToExpiry,
		double dSigma = 0.0);

public:

	// Accessors
	 double forward() const { return myForward; }
	 double strike() const { return myStrike; }
	 double sigma() const { return mySigma; }
	 double timeToExpiry() const { return myTimeToExpiry; }

	 bool optionExpired() const { return myOptionExpired; }
	 double sqrtTimeToExpiry() const { return mySqrtTimeToExpiry; }
	 double variance() const { return myVariance; }
	 double stdDev() const { return myStdDev; }


	// Functions

	 double intrinsic(UT_CallPut callPut) const;

	// The Option Premium (for this Forward, Strike,...)
	 virtual double premium(UT_CallPut callPut) const = 0;

	// The Option Delta (for this Forward, Strike, ...) : dPremium/dForward -- Will depend upon each framework
	 virtual double delta(UT_CallPut callPut) const = 0;

	// The Option Gamma (for this Forward, Strike, Volatility type...) dDelta/dForward -- Will depend upon each framework
	 virtual double gamma(UT_CallPut callPut) const = 0;

	// The Option Vega (for this Forward, Strike,...) dPremium/dSigma -- The scale will depend upon each framework (this is the derivative w.r.t the framework volatility)
	 virtual double vega(UT_CallPut callPut) const = 0;

	// The Option Theta (for this Forward, Strike, Volatility type...) : -dPremium/dTime
	 virtual double theta(UT_CallPut callPut) const = 0;

	// Calculates IN SITU the Implied Sigma to match the passed in option price....
	// Non-virtual function - relies upon virtual function
	 double impliedSigma(double premium, UT_CallPut callPut);

	 void impliedSigmaInputCheck(double targetPremium, UT_CallPut callPut) const;

	// Overwrites the sigma in the Option Pricer -- updates all that has to be updated (called by the solvers)
	 virtual void overwriteSigma(double newSigma);

protected:

	static const double ourInfiniteDistance;
	static const double ourEpsilon;

private:

	// Called by constructor - Calculates all the intermediate results and numbers based on the value of sigma
	 void preliminaryCalculations();

	// Private Data

	// Member variables
	double myForward;
	double myStrike;
	double myTimeToExpiry;
	bool   myOptionExpired;

	// The volatility (the sigma of the 'model' or framework)
	double mySigma;

	// Pre-calculated Variables

	double mySqrtTimeToExpiry;
	// The Variance as Sigma*Sigma*T with the vol
	double myVariance;
	// The StdDev as Sqrt(Variance) with the vol
	double myStdDev;


};

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//Helper Class for implied vol calculation
class UTSolveForImpliedVolatility : public UTNewton
{
	// Object for this class: Solve for the implied volatility from given premium

public:

	virtual ~UTSolveForImpliedVolatility() {}

	UTSolveForImpliedVolatility(UTEuropeanOptionBase& option, UT_CallPut callPut, double target)
		: UTNewton(),
		myOption(option),
		myCallPut(callPut),
		myTarget(target){}

	// Overloaded penalty function
	virtual double valueSlopeRatio(double abscissa);

private:

	UTEuropeanOptionBase&		myOption;
	UT_CallPut myCallPut;
	double myTarget;
};

///////////////////////////////////////////////////////////////////////////////
#endif // UT_EUROPEAN_OPTION_BASE_H
