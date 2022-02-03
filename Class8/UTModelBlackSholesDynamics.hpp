/* UTModelBlackSholesDynamics.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_MODEL_BLACK_SHOLES_DYNAMICS_H
#define UT_MODEL_BLACK_SHOLES_DYNAMICS_H

#include <string>
#include <vector>
#include "UTModelBase.hpp"
#include "UTModelYieldCurve.hpp"
#include "UTWrapper.hpp"


class UTModelYieldCurve;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UTModelBlackSholesDynamics : public UTModelBase
{

public:

	enum UT_InterpolateeType
	{
		UT_INST_VOL = 0,
		UT_VARIANCE = 1,
		UT_INST_VOL_SQUARED = 2
	};

	enum UT_InterpolationMethod
	{
		UT_FLAT = 0,
		UT_LINEAR = 1
	};

	static std::string const ourClassTag;

	// Destructor.
	virtual ~UTModelBlackSholesDynamics();

	// Defalut Constructor: construct the default flat vol curve at 30%
	UTModelBlackSholesDynamics();

	// Constructor with user specified flat vol and flat rate
	UTModelBlackSholesDynamics(double spot, double floatVol, double flatRate);

	// Constructor with user specified vol curve
	UTModelBlackSholesDynamics(
		double spot,
		const std::vector<double>&	 timeLine,
		const std::vector<double>&	 vols,
		UT_InterpolateeType interpolateeType = UT_INST_VOL,
		UT_InterpolationMethod interpolationMethod = UT_FLAT);

	//Set sub yield curve model
	void setModelYieldCurve(const UTModelYieldCurve& yieldCurveModel) {
		myYieldCurve = static_cast<UTModelYieldCurve*>(yieldCurveModel.clone());
	};
	

	// Clone
	virtual UTModelBase* clone() const;

	// Functions.

	virtual std::string classTag() const { return ourClassTag; }

	// Return the discount factor given a date
	virtual double df(double time) const;

	// Return the forward rate given two dates  
	virtual double forwardRate(double startTime, double endTime) const;

	// Return the forward price given a date  
	double forwardPrice(double time) const;

	double logDrift(double startTtime, double endTime) const;

	double logVariance(double startTime, double endTime) const;

	double vol(double time) const;

private:

	// solver in the calibration only use this.
	virtual void setComponent(unsigned int i, double component) { myVols[i] = component; }

	// This Black Dynamics Model contains a sub-model that will do all the underlying calculations : df, forward, ...
	UTWrapper<UTModelYieldCurve>        myYieldCurve;

	double mySpot;

	//Instantanious Vol Curve
	std::vector<double> myTimeLine;
	std::vector<double> myVols;

	UT_InterpolateeType myInterpolatee;
	UT_InterpolationMethod myInterpolationMethod;

};

#endif // UT_MODEL_BLACK_SHOLES_DYNAMICS_H

