/* UTModelYieldCurve.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTModelYieldCurve.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Static data.

// The class tag
const string UTModelYieldCurve::ourClassTag = "Yield Curve Model";

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UTModelYieldCurve::~UTModelYieldCurve()
{
}

///////////////////////////////////////////////////////////////////////////////
//Default constructor: Default yield curve of flat 3%
UTModelYieldCurve::UTModelYieldCurve()
	: UTModelBase(), 
	myTimeLine(1, 1.0), myRates(1, 0.03), myInterpolatee(UT_FWD_RATE), myInterpolationMethod(UT_FLAT)
{
}

///////////////////////////////////////////////////////////////////////////////
UTModelYieldCurve::UTModelYieldCurve(double flatRate)
	: UTModelBase(),
	myTimeLine(1, 1.0), myRates(1, flatRate), myInterpolatee(UT_FWD_RATE), myInterpolationMethod(UT_FLAT)
{
}

///////////////////////////////////////////////////////////////////////////////
UTModelYieldCurve::UTModelYieldCurve(const vector<double>&	 timeLine,
	const vector<double>&	 rates,
	UT_InterpolateeType interpolateeType,
	UT_InterpolationMethod interpolationMethod)
	: UTModelBase(),
	myTimeLine(timeLine), myRates(rates), myInterpolatee(interpolateeType), myInterpolationMethod(interpolationMethod)
{
}
///////////////////////////////////////////////////////////////////////////////
UTModelBase* UTModelYieldCurve::clone() const
{
	return new UTModelYieldCurve(*this);
}

///////////////////////////////////////////////////////////////////////////////
double UTModelYieldCurve::df(double time) const
{
	return exp(-1.0 * lnDf(time));
}

///////////////////////////////////////////////////////////////////////////////
double UTModelYieldCurve::forwardRate(double startTime, double endTime) const
{
	double rtn = df(startTime) / df(endTime) - 1.0;
	return rtn / (endTime - startTime);
}

///////////////////////////////////////////////////////////////////////////////
double UTModelYieldCurve::lnDf(double time) const
{
	//Assuming piecewise flat forward rate interpolation..

	if (time < 0.0)
	{
		return 0.0;
	}

	// Get the size of the time line.
	auto gridSize = myTimeLine.size();
	double previousTime = 0.0;
	double currentTime = 0.0;
	double sum = 0.0;
	unsigned i = 0;

	for (i = 0; i < gridSize; ++i)
	{
		currentTime = myTimeLine[i];
		if (currentTime <= time)
		{
			sum += myRates[i] * (currentTime - previousTime);
			previousTime = currentTime;
		}
		else
		{
			//This means interpolation
			return sum + myRates[i] * (time - previousTime);

		}
	}

	//This means exterpolation (i.e.,  input time > last grid point of time)
	return sum + myRates[i-1] * (time - previousTime);
			
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
