
#include <iostream>
#include <string>
#include <cmath>
#include "UTEnumn.h"
#include "UTPayoff1.h"
#include "UTRandom1.h"

using namespace std;


double SimpleMonteCarlo1(double Expiry,
	double Strike,
	double Spot,
	double(*payoff)(double, double),
	double Vol,
	double r,
	unsigned long NumberOfPaths)
{

	double variance = Vol*Vol*Expiry;
	double rootVariance = sqrt(variance);
	double itoCorrection = -0.5*variance;

	double movedSpot = Spot*std::exp(r*Expiry + itoCorrection);
	double thisSpot;
	double runningSum = 0;

	for (unsigned long i = 0; i < NumberOfPaths; i++)
	{
		double thisGaussian = GetOneGaussianByBoxMuller();
		thisSpot = movedSpot*exp(rootVariance*thisGaussian);
		double thisPayoff = payoff(thisSpot, Strike);
		runningSum += thisPayoff;
	}

	double mean = runningSum / NumberOfPaths;
	mean *= exp(-r*Expiry);
	return mean;
}

int main()
{

	double Expiry;
	string CallPut;
	double Strike;
	double Spot;
	double Vol;
	double r;
	unsigned long NumberOfPaths;

	cout << "\nEnter expiry\n";
	cin >> Expiry;

	cout << "\nEnter call/put\n";
	cin >> CallPut;

	cout << "\nEnter strike\n";
	cin >> Strike;

	cout << "\nEnter spot\n";
	cin >> Spot;

	cout << "\nEnter vol\n";
	cin >> Vol;

	cout << "\nr\n";
	cin >> r;

	cout << "\nNumber of paths\n";
	cin >> NumberOfPaths;

	//convert call/put string into internal type
	UT_CallPut internalCallPut = toCallPut(CallPut);

	double(*payoff)(double, double);

	switch (internalCallPut)
	{
		case UT_CALL:
		{
			payoff = callPayoff;
			break;
		}
		case UT_PUT:
		{
			payoff = putPayoff;
			break;
		}
		case UT_STRADDLE:
		{
			payoff = straddlePayoff;
			break;
		}
		case UT_INVALID_CALL_PUT:
		default:
		{
			payoff = 0;
			break;
		}
	}

	double result = SimpleMonteCarlo1(Expiry,
		Strike,
		Spot,
		payoff,
		Vol,
		r,
		NumberOfPaths);

	cout << "\n The price is " << result << "\n";

	double tmp;
	cin >> tmp;

	return 0;
	
}

/*
*
* Copyright (c) 2002
* Mark Joshi
*
* Permission to use, copy, modify, distribute and sell this
* software for any purpose is hereby
* granted without fee, provided that the above copyright notice
* appear in all copies and that both that copyright notice and
* this permission notice appear in supporting documentation.
* Mark Joshi makes no representations about the
* suitability of this software for any purpose. It is provided
* "as is" without express or implied warranty.

* Diva Analytics
* modified this file (Jan-21-2016)
*/

