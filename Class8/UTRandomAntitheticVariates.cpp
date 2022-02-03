/* UTRandomAntitheticVariates.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTRandomAntitheticVariates.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UTRandomAntitheticVariates::UTRandomAntitheticVariates(const UTWrapper<UTRandomBase>& generator)
	: UTRandomBase(*generator),
	myGenerator(generator)
{
	myGenerator->reset();
	myOddEven = true;
	myNextVariates.resize(dimensionality());
}

///////////////////////////////////////////////////////////////////////////////
UTRandomBase* UTRandomAntitheticVariates::clone() const
{
	return new UTRandomAntitheticVariates(*this);
}

///////////////////////////////////////////////////////////////////////////////
void UTRandomAntitheticVariates::nextUniformVector(vector<double>& variates)
{
	if (myOddEven)
	{
		myGenerator->nextUniformVector(variates);

		for (unsigned long i = 0; i < dimensionality(); ++i)
			myNextVariates[i] = 1.0 - variates[i];

		myOddEven = false;
	}
	else
	{
		variates = myNextVariates;
		myOddEven = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
void UTRandomAntitheticVariates::setSeed(unsigned long seed)
{
	myGenerator->setSeed(seed);
	myOddEven = true;
}

///////////////////////////////////////////////////////////////////////////////
void UTRandomAntitheticVariates::skip(unsigned long numberOfPaths)
{
	if (numberOfPaths == 0)
		return;

	if (myOddEven)
	{
		myOddEven = false;

		--numberOfPaths;
	}

	myGenerator->skip(numberOfPaths / 2);

	if (numberOfPaths % 2)
	{
		vector<double> tmp(dimensionality());

		nextUniformVector(tmp);
	}

}

///////////////////////////////////////////////////////////////////////////////
void UTRandomAntitheticVariates::resetDimensionality(unsigned long dimensionality)
{
	UTRandomBase::resetDimensionality(dimensionality);

	myNextVariates.resize(dimensionality);

	myGenerator->resetDimensionality(dimensionality);
}

///////////////////////////////////////////////////////////////////////////////
void UTRandomAntitheticVariates::reset()
{
	myGenerator->reset();
	myOddEven = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////