/* UTProductPathDependent.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTProductPathDependent.h"
#include <stdexcept>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// The classs tag
const string UTProductPathDependentAsian::ourClassTag = "Asian Option";

///////////////////////////////////////////////////////////////////////////////
// Constructor: Calculate the procuct time line.
UTProductPathDependentAsian::UTProductPathDependentAsian(
	double		 averageStartTime,
	double		 expiryTime,
	unsigned long numberOfAverage,
	double       notional,
	UT_CallPut callPut,
	UT_BuySell   buySell,
	double strike,
	UT_AverageType averageMethod )
	: UTProductPathDependentBase(notional, buySell),
	myAverageStartTime(averageStartTime), myExpiryTime(expiryTime), myNumberOfAverage(numberOfAverage), myCallPut(callPut), myStrike(strike), myAverageType(averageMethod)
{
	// Check the relation of strat date and end date
	if (averageStartTime >= expiryTime)
	{
		throw runtime_error("UTProductPathDependentAsian: averge start date shouble be before the average end date(= expiry date).");
	}

	for (unsigned long i = 0; i < myNumberOfAverage; ++i)
	{
		myTimeLine.push_back(i * (expiryTime - averageStartTime) / (myNumberOfAverage-1));  //excluding average start date
	}
}

///////////////////////////////////////////////////////////////////////////////
unsigned long UTProductPathDependentAsian::payoffs(const vector<double> spotPrices, vector<UTCashflows_t> &cashflows) const
{
	double sum = 0.0;
	double prod = 1.0;
	double mean;
	if (myAverageType == UT_AverageType::UT_ARITHMETIC)
	{
		for (unsigned long i = 0; i < spotPrices.size(); ++i)
		{
			sum += spotPrices[i];
		}

		mean = sum / myNumberOfAverage;
	}
	else if (myAverageType == UT_AverageType::UT_GEOMETRIC)
	{
		for (unsigned long i = 0; i < spotPrices.size(); ++i)
		{
			prod *= spotPrices[i];
		}

		mean = pow(prod, 1.0 / myNumberOfAverage);

	}
	else
	{
		throw runtime_error("UTProductPathDependentAsian: Unknown average type flag.");

	}

	cashflows[0].first = 0;

	if (myCallPut == UT_CallPut::UT_CALL)
	{
		cashflows[0].second = mean - myStrike > 0.0 ? mean - myStrike : 0.0;
	}
	else if (myCallPut == UT_CallPut::UT_PUT)
	{
		cashflows[0].second= myStrike - mean > 0.0 ? myStrike - mean : 0.0;

	}
	else
	{
		throw runtime_error("UTProductPathDependentAsian: Unknown call/put flag.");
	}

	// do not forget about the notional and but/sell
	cashflows[0].second *= notional() * static_cast<int>(buySell());

	return 1;  //return the number of cashflows.
}

///////////////////////////////////////////////////////////////////////////////
const std::vector<double> UTProductPathDependentAsian::cashflowPayTimes() const 
{
	vector<double> rtn(1, myExpiryTime);
	return rtn;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
