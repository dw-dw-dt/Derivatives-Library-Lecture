/* UTRandomAntitheticVariates.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_RANDOM_ANTITHETIC_VARIATES_H
#define UT_RANDOM_ANTITHETIC_VARIATES_H

#include <vector>

#include "UTRandomBase.hpp"
#include "UTWrapper.hpp"

class UTRandomAntitheticVariates : public UTRandomBase
{

public:

	UTRandomAntitheticVariates(const UTWrapper<UTRandomBase>& generator);

	virtual UTRandomBase* clone() const;

	virtual void nextUniformVector(std::vector<double>& variates);

	virtual void skip(unsigned long numberOfPaths);

	virtual void setSeed(unsigned long seed);

	virtual void resetDimensionality(unsigned long dimensionality);

	virtual void reset();

private:

	UTWrapper<UTRandomBase> myGenerator;

	bool myOddEven;

	std::vector<double> myNextVariates;
};

#endif  //UT_RANDOM_ANTITHETIC_VARIATES_H
