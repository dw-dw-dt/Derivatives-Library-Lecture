/* UTEuropeanOptionSABR.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_EUROPEAN_OPTION_SABR_H
#define UT_EUROPEAN_OPTION_SABR_H

#include <memory>

#include "UTEuropeanOptionBase.h"

///////////////////////////////////////////////////////////////////////////////
class UTEuropeanOptionSABR : public UTEuropeanOptionBase
{
public:
	enum UT_ExpansionType
	{
		UT_NORMAL = 0,
		UT_LOGNORMAL = 1
	};

	// Destructor
	~UTEuropeanOptionSABR();

	// Default constructor
	UTEuropeanOptionSABR();
	// Constructor
	UTEuropeanOptionSABR(
		double dForward,
		double dStrike,
		double dTimeToExpiry,
		double dSigma = 0.0,  // This is a CEV Volatility !!!
		double dBeta = 1.0,   // CEV coefficient
		double dAlpha = 0.0,  // volatility of volatility
		double dRho = 0.0,    // correlation
		UT_ExpansionType expansiontype = UT_LOGNORMAL
		);

	// Accessors
	double alpha() const { return myAlpha; }
	double beta() const { return myBeta; }
	double rho() const { return myRho; }

	// Pricing Functions
	// The Option Premium (for this Forward, Strike, Volatility type...)
	virtual double premium(UT_CallPut callPut) const;

	// The Option Delta (for this Forward, Strike, Volatility type...) : dPremium/dForward
	virtual double delta(UT_CallPut callPut) const;

	// The Option Gamma (for this Forward, Strike, Volatility type...) : dDelta/dForward
	virtual double gamma(UT_CallPut callPut) const;

	// The Option Vega (for this Forward, Strike, Volatility type...) : dPremium/dSigma
	virtual double vega(UT_CallPut callPut) const;

	// The Option Theta (for this Forward, Strike, Volatility type...) : -dPremium/dTime
	virtual double theta(UT_CallPut callPut) const;

	double equivalentVolatility() const { return myEquivalentVolatility; }

	// Overwrites the sigma in the Option Pricer -- updates all that has to be updated (called by the solvers)
	virtual void overwriteSigma(double newSigma);

private:
	// Accessors
	UT_ExpansionType expansionType() const { return myExpansionType; }
	const UTEuropeanOptionBase& underlyingOption() const { return *myUnderlyingOption; }
	double DEquivalentVolatilityDForward() const { return myDEquivalentVolatilityDForward; }
	double DEquivalentVolatilityDSigma() const { return myDEquivalentVolatilityDSigma; }
	double DEquivalentVolatilityDTime() const { return myDEquivalentVolatilityDTime; }

	// Transformation   
	// Called by constructor - Calculates IN SITU the equivalent volatility and derivatives, as well as all the intermediate results
	void preliminaryCalculations();

private:
	// Private Data   

	// Member variables

	// All the other input variables are stored in the base class
	double  myAlpha;
	double  myBeta;
	double  myRho;

	// This class will perform a volatility expansion w.r.t. LogNormal or Normal volatility, 

	// The type of the expansion: normal or lognormal
	UT_ExpansionType  myExpansionType;

	// The underlying object (LogNormal) used to perform the calculations, once the volatility expansion has been done
	std::unique_ptr<UTEuropeanOptionBase>  myUnderlyingOption;

	// The equivalent (Normal or LogNormal) volatility
	double            myEquivalentVolatility;

	// The derivatives of the equivalent volatility w.r.t. some input parameters
	double            myDEquivalentVolatilityDForward;
	double            myDEquivalentVolatilityDSigma;
	double            myDEquivalentVolatilityDTime;

};

///////////////////////////////////////////////////////////////////////////////
#endif // UT_EUROPEAN_OPTION_SABR_H
