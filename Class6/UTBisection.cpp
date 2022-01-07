/* UTBisection.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include <stdexcept>

#include "UTBisection.hpp"

using namespace std;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
double UTBisection::root(double x1, double x2, double accuracy, unsigned long maxIterations)
{
	double f1 = error(x1);
	double f2 = error(x2);

	double xmid, fmid;
	for (unsigned long i = 0; i<maxIterations; ++i)
	{
		xmid = 0.5 * (x1 + x2);
		fmid = error(xmid);
		if (fmid == 0.0)
		{
			return xmid;
		}
		else if (f1*fmid < 0.0)
		{
			x2 = xmid;
			f2 = fmid;
		}
		else
		{
			x1 = xmid;
			f1 = fmid;
		}
		if (fabs(x2 - x1) <= accuracy || fmid == 0.0)
			return 0.5 * (x2 + x1);
	}

	throw runtime_error("UTBisection: Root search did not converge.");

	// Should never get here
	return 0.0;
}
