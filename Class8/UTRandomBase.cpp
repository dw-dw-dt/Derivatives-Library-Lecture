/* UTRandomBase.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTRandomBase.hpp"
#include "UTMathFunctions.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UTRandomBase::UTRandomBase()
{
}

UTRandomBase::UTRandomBase(unsigned long dimensionality)
	: myDimensionality(dimensionality)
{
}

///////////////////////////////////////////////////////////////////////////////
void UTRandomBase::nextGaussianVector(vector<double>& variates)
{
	nextUniformVector(variates);

	for (unsigned long i = 0; i < myDimensionality; i++)
	{
		double x = variates[i];
		variates[i] = UTMathFunctions::inverseCumulativeNormal(x);
	}
}
///////////////////////////////////////////////////////////////////////////////
