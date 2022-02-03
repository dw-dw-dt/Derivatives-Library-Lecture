/* UTParkMiller.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_RANDOM_PARK_MILLER_H
#define UT_RANDOM_PARK_MILLER_H

#include <vector>

#include "UTRandomBase.h"

class UTParkMiller
{
private:

	static const long a;
	static const long m;
	static const long q;
	static const long r;

public:

	UTParkMiller(long seed = 1);

	long getOneRandomInteger();
	void setSeed(long seed);

	static unsigned long max();
	static unsigned long min();

private:

	long mySeed;

};

class UTRandomParkMiller : public UTRandomBase
{
public:

	UTRandomParkMiller();
	UTRandomParkMiller(unsigned long dimensionality, unsigned long seed = 1);

	virtual UTRandomBase* clone() const;
	virtual void nextUniformVector(std::vector<double>& variates);
	virtual void skip(unsigned long numberOfPaths);
	virtual void setSeed(unsigned long Seed);
	virtual void reset();
	virtual void resetDimensionality(unsigned long dimensionality);

private:

	UTParkMiller myGenerator;
	unsigned long myInitialSeed;
	double myReciprocal;

};

#endif //UT_RANDOM_PARK_MILLER_H
