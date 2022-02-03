/* UTRandomBase.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_RANDOM_BASE_H
#define UT_RANDOM_BASE_H

#include <vector>

class UTRandomBase
{
public:

	UTRandomBase();

	UTRandomBase(unsigned long Dimensionality);

	unsigned long dimensionality() const{ return myDimensionality; }

	virtual UTRandomBase* clone() const = 0;
	virtual void nextUniformVector(std::vector<double>& variates) = 0;
	virtual void skip(unsigned long numberOfPaths) = 0;
	virtual void setSeed(unsigned long Seed) = 0;
	virtual void reset() = 0;

	virtual void nextGaussianVector(std::vector<double>& variates);
	virtual void resetDimensionality(unsigned long dimensionality) { myDimensionality = dimensionality; }

private:
	unsigned long myDimensionality;

};


#endif
