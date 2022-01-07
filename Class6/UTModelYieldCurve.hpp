/* UTModelYieldCurve.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_MODEL_YIELD_CURVE_H
#define UT_MODEL_YIELD_CURVE_H

#include <string>
#include <vector>
#include "UTModelBase.hpp"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UTModelYieldCurve : public UTModelBase
{

public:

	enum UT_InterpolateeType
	{
		UT_FWD_RATE = 0,
		UT_ZERO_RATE = 1,
		UT_MINUS_LOG_DF = 2
	};

	enum UT_InterpolationMethod
	{
		UT_FLAT = 0,
		UT_CUBIC_SPLINE = 1,
		UT_LINEAR = 2
	};

	static std::string const ourClassTag;

	// Destructor.
	virtual ~UTModelYieldCurve();

	// Defalut Constructor: construct the default flat yield curve at 3%
	UTModelYieldCurve();

	// Constructor with user specified flat rate
	UTModelYieldCurve(double flatRate);

	//Cinstructor with user specified yield curve
	UTModelYieldCurve(
		const std::vector<double>&	 timeLine,
		const std::vector<double>&	 rates,
		UT_InterpolateeType interpolateeType  = UT_FWD_RATE,
		UT_InterpolationMethod interpolationMethod = UT_FLAT);

	//Clone
	virtual UTModelBase* clone() const;

	// Functions.
	virtual std::string classTag() const { return ourClassTag; }

	// Return the discount factor given a date
	virtual double df( double time) const;

	// Return the forward rate given two dates  
	virtual double forwardRate(double startTime, double endTime) const;

	// Return log of DF
	double lnDf(double time) const;

private:

	// solver in the calibration only use this.
	virtual void setComponent(unsigned int i, double component) { myRates[i] = component; }

	//Yield Curve
	std::vector<double> myTimeLine;
	std::vector<double> myRates;

	UT_InterpolateeType myInterpolatee;
	UT_InterpolationMethod myInterpolationMethod;

};

#endif // UT_MODEL_YIELD_CURVE_H

