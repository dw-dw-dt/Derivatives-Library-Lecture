// UTProductBase.cpp
//
#include "UTProductBase.hpp"
#include <stdexcept>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
const vector<double>& UTProductBase::timeLine() const
{
	throw runtime_error("UTProductBase::timeLine() base class function cannot be called.");
}

///////////////////////////////////////////////////////////////////////////////
const vector<double> UTProductBase::cashflowPayTimes() const
{
	throw runtime_error("UTProductBase::cashflowPayTimes() base class function cannot be called.");
}

///////////////////////////////////////////////////////////////////////////////
unsigned long UTProductBase::payoffs(const vector<double> spotPrices, vector<UTCashflows_t> &cashflows) const
{
	throw runtime_error("UTProductBase::payoffs() base class function cannot be called.");
}
