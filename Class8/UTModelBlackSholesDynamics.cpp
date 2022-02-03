/* UTModelBlackSholesDynamics.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTModelBlackSholesDynamics.hpp"
#include "UTModelYieldCurve.hpp"
#include "UTWrapper.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Static data.

// The class tag
const string UTModelBlackSholesDynamics::ourClassTag = "Black Sholes Dynamics Model";

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UTModelBlackSholesDynamics::~UTModelBlackSholesDynamics()
{
}

///////////////////////////////////////////////////////////////////////////////
UTModelBlackSholesDynamics::UTModelBlackSholesDynamics()
	: UTModelBase(),
	mySpot(100.0), myTimeLine(1, 1.0), myVols(1, 0.3), myInterpolatee(UT_INST_VOL), myInterpolationMethod(UT_FLAT)
{
}
///////////////////////////////////////////////////////////////////////////////
UTModelBlackSholesDynamics::UTModelBlackSholesDynamics(double spot, double flatVol, double flatRate)
	: UTModelBase(),
	mySpot(spot), myTimeLine(1, 1.0), myVols(1, flatVol), myInterpolatee(UT_INST_VOL), myInterpolationMethod(UT_FLAT), myYieldCurve(new UTModelYieldCurve(flatRate))
{
}

///////////////////////////////////////////////////////////////////////////////
UTModelBlackSholesDynamics::UTModelBlackSholesDynamics(double spot,
	const std::vector<double>&	 timeLine,
	const std::vector<double>&	 vols,
	UT_InterpolateeType interpolateeType,
	UT_InterpolationMethod interpolationMethod)
	: UTModelBase(),
	mySpot(spot), myTimeLine(timeLine), myVols(vols), myInterpolatee(interpolateeType), myInterpolationMethod(interpolationMethod)
{
}
///////////////////////////////////////////////////////////////////////////////
UTModelBase* UTModelBlackSholesDynamics::clone() const
{
	return new UTModelBlackSholesDynamics(*this);
}

///////////////////////////////////////////////////////////////////////////////
double UTModelBlackSholesDynamics::df(double time) const
{
	return myYieldCurve->df(time);
}

///////////////////////////////////////////////////////////////////////////////
double UTModelBlackSholesDynamics::forwardRate(double startTime, double endTime) const
{
	return myYieldCurve->forwardRate(startTime, endTime);
}

///////////////////////////////////////////////////////////////////////////////
double UTModelBlackSholesDynamics::forwardPrice(double time) const
{
	return mySpot / myYieldCurve->df(time);
}

///////////////////////////////////////////////////////////////////////////////
double UTModelBlackSholesDynamics::logDrift(double startTime, double endTime) const
{
	// Assuming no dividend, 
	double rtn = myYieldCurve->lnDf(endTime) - myYieldCurve->lnDf(startTime);
	rtn -= 0.5 * logVariance(startTime, endTime);
	return rtn;
}

///////////////////////////////////////////////////////////////////////////////
double UTModelBlackSholesDynamics::logVariance(double startTime, double endTime) const
{
	//Assuming flat instantanious vol interpolation..

	if (endTime < startTime)
	{
		return 0.0;
	}

	// Get the size of the time line.
	auto gridSize = myTimeLine.size();

	// Initialize some locals
	double previousTime = startTime;
	double currentTime = 0.0;
	double sum = 0.0;
	unsigned int i = 0;

	while ((currentTime = myTimeLine[i]) < endTime && i < gridSize - 1)
	{
		if (currentTime > previousTime)
		{
			sum += myVols[i] * myVols[i] * (currentTime - previousTime);
			previousTime = currentTime;
		}
		++i;
	}

	return sum + myVols[i] * myVols[i] * (endTime - previousTime);


}

///////////////////////////////////////////////////////////////////////////////
double UTModelBlackSholesDynamics::vol(double time) const
{
	return sqrt(logVariance(0, time) / time);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
