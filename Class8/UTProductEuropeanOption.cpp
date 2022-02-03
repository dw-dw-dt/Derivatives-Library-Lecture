/* UTProductEuropeanOption.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTProductEuropeanOption.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// The classs tag
const string UTProductEuropeanOptionCall::ourClassTag = "European Call Option";
const string UTProductEuropeanOptionPut::ourClassTag = "European Put Option";
const string UTProductEuropeanOptionStraddle::ourClassTag = "European Straddle Option";

///////////////////////////////////////////////////////////////////////////////
const vector<double> UTProductEuropeanOptionBase::cashflowPayTimes() const
{
	vector<double> rtn(1, myExpiryTime);
	return rtn;
}

///////////////////////////////////////////////////////////////////////////////
unsigned long UTProductEuropeanOptionBase::payoffs(const vector<double> spotPrices, vector<UTCashflows_t> &cashflows) const
{

	cashflows[0].first = 0; //time index

	cashflows[0].second = static_cast<double>(myBuySell) * myNotional * payoff(spotPrices[0]);  //Call virtual function for the payoff

	return 1;  //return the number of cashflows.
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UTProductEuropeanOptionStraddle::UTProductEuropeanOptionStraddle(
	double		 expiryTime,
	double       notional,
	UT_BuySell   buySell,
	double strike)
	: UTProductEuropeanOptionBase(expiryTime, notional, buySell), UTProductLinearBase(),
	myStrike(strike)
{

	myCallOption = shared_ptr<UTProductEuropeanOptionCall>(new UTProductEuropeanOptionCall(
		expiryTime,
		notional,
		buySell,
		strike));

	myPutOption = shared_ptr<UTProductEuropeanOptionPut>(new UTProductEuropeanOptionPut(
		expiryTime,
		notional,
		buySell,
		strike));

	vector< shared_ptr< const UTProductBase > > underlyings(2);
	underlyings[0] = myCallOption;
	underlyings[1] = myPutOption;

	setLinearProductUnderlyings(underlyings);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

