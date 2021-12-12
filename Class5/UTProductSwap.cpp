/* UTProductSwap.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTEnum.hpp"
#include "UTProductSwap.hpp"
#include <stdexcept>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// The classs tag
const string UTProductLegVanilla::ourClassTag = "Vanilla Leg";
const string UTProductSwapVanilla::ourClassTag = "Vanilla Swap";

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//UTProductLinearBase
//
UTProductLinearBase::UTProductLinearBase()
	: UTProductBase(),
	myUnderlyings()
{
}

///////////////////////////////////////////////////////////////////////////////
UTProductLinearBase::UTProductLinearBase(const vector<shared_ptr<const UTProductBase> >& underlyings)
	: UTProductBase(),
	myUnderlyings(underlyings)
{
}

///////////////////////////////////////////////////////////////////////////////
double UTProductLinearBase::firstTime() const
{
	unsigned int iSize = size();
	// Compare with the first time of the cashflows.  
	double rtn = 100000.0;
	for (unsigned int i = 0; i < iSize; ++i)
	{
		const shared_ptr<const UTProductBase>& theUnderlying = underlying(i);
		if (!theUnderlying)
		{
			throw runtime_error("UTProductLegVanilla: Invalid underlying.");
		}

		double cFirstTime;
		if (rtn >(cFirstTime = theUnderlying->firstTime()))
		{
			rtn = cFirstTime;
		}
	}

	return rtn;
}

///////////////////////////////////////////////////////////////////////////////
double UTProductLinearBase::lastTime() const
{
	unsigned int iSize = size();
	// Compare with the first time of the cashflows.  
	double rtn = -100000.0;
	for (unsigned int i = 0; i < iSize; ++i)
	{
		const shared_ptr<const UTProductBase>& theUnderlying = underlying(i);
		if (!theUnderlying)
		{
			throw runtime_error("UTProductLegVanilla: Invalid underlying.");
		}

		double cLastTime;
		if (rtn <(cLastTime = theUnderlying->lastTime()))
		{
			rtn = cLastTime;
		}
	}

	return rtn;
}

///////////////////////////////////////////////////////////////////////////////
void
UTProductLinearBase::setLinearProductUnderlyings(const vector< shared_ptr< const UTProductBase > >& underlyings)
{
	myUnderlyings = underlyings;
}

///////////////////////////////////////////////////////////////////////////////
void
UTProductLinearBase::setLinearProductUnderlying(shared_ptr< const UTProductBase >& underlying)
{
	myUnderlyings = vector< shared_ptr< const UTProductBase > >(1, underlying);
}

///////////////////////////////////////////////////////////////////////////////
// When complete cashflow objects are given.
UTProductLegVanilla::UTProductLegVanilla(const vector<shared_ptr<const UTProductCashflowBase> >& cashflows)
	: UTProductLinearBase(UTProductLinearBase::convertProductVectorToBase(cashflows)),
	myCashflows(cashflows)
{
}

///////////////////////////////////////////////////////////////////////////////
UTProductLegVanilla::UTProductLegVanilla(
	const UT_FixedFloat&					  fixedFloat,
	double                                    adjStartTime,
	double       			                  adjEndTime,
	double									  couponOrSpread,
	double									  period,
	double                                    notional,
	const UT_PayReceive&					  payReceive)
	: UTProductLinearBase()
{
	// Check the consistency of the input times..
	if (adjEndTime <= adjStartTime)
	{
		throw runtime_error("UTProductLegVanilla: Start time shoule be less than end time.");
	}

	vector<shared_ptr<const UTProductCashflowBase> > cashflows;


	// Calculate cashflow size
	unsigned int cashflowSize = unsigned int((adjEndTime - adjStartTime) / period +1.0 - 100.0*DBL_EPSILON);

	// Create schedule backwards so that the stub in front is short.
	vector<double> startTimes;
	vector<double> endTimes;
	double endTime = adjEndTime;
	for (unsigned int i = 0; i < cashflowSize - 1; ++i)  // excluding the first cashflow
	{
		double startTime = endTime - period;
		startTimes.push_back(startTime);
		endTimes.push_back(endTime);
		endTime = startTime;
	}

	startTimes.push_back(adjStartTime);  // Make sure that the leg start time is the cashflow start time
	endTimes.push_back(endTime);

	myCashflows.clear();  //Just in case..
	if (fixedFloat == UT_FixedFloat::UT_FIXED)
	{
		for (unsigned int i = 0; i < cashflowSize; ++i)
		{
			shared_ptr<UTProductCashflowBase> cashflow(new UTProductCashflowRateFixed(
				startTimes[cashflowSize - 1-i],
				endTimes[cashflowSize - 1-i],
				payReceive,
				notional,
				couponOrSpread ));

			myCashflows.push_back(cashflow);
		}
	}
	else
	{
		for (unsigned int i = 0; i < cashflowSize; ++i)
		{
			shared_ptr<UTProductCashflowBase> cashflow(new UTProductCashflowRateFloat(
				startTimes[cashflowSize -1- i],
				endTimes[cashflowSize -1- i],
				payReceive,
				notional,
				couponOrSpread ) );

			myCashflows.push_back(cashflow);
		}
	}

	setLinearProductUnderlyings( UTProductLinearBase::convertProductVectorToBase(myCashflows) );
}

///////////////////////////////////////////////////////////////////////////////
// Operators.
const shared_ptr<const UTProductCashflowBase>& UTProductLegVanilla::operator[] (unsigned long index) const
{
	return cashflows()[index];
}

///////////////////////////////////////////////////////////////////////////////
UTProductSwapVanilla::UTProductSwapVanilla(
	const shared_ptr <const UTProductLegVanilla>&  fixedLeg,
	const shared_ptr <const UTProductLegVanilla>&  floatLeg)
	: UTProductLinearBase(),
	myFixedLeg(fixedLeg),
	myFloatLeg(floatLeg)
{
	vector< shared_ptr< const UTProductBase > > underlyings(2);
	underlyings[0] = myFixedLeg;
	underlyings[1] = myFloatLeg;

	setLinearProductUnderlyings(underlyings);
}

///////////////////////////////////////////////////////////////////////////////
UTProductSwapVanilla::UTProductSwapVanilla(
	double                                    adjStartTime,
	double       			                  adjEndTime,
	double									  coupon,
	double									  fixedPeriod,
	double									  floatPeriod,
	double                                    notional,
	const UT_PayReceive&					  payReceiveFixed)
	: UTProductLinearBase()
{
	// Check the consistency of the input times..
	if (adjEndTime <= adjStartTime)
	{
		throw runtime_error("UTProductSwapVanilla: Start time shoule be less than end time.");
	}

	myFixedLeg = shared_ptr<UTProductLegVanilla>(new UTProductLegVanilla(
		UT_FixedFloat::UT_FIXED,
		adjStartTime,
		adjEndTime,
		coupon,
		fixedPeriod,
		notional,
		payReceiveFixed) );

	myFloatLeg = shared_ptr<UTProductLegVanilla>(new UTProductLegVanilla(
		UT_FixedFloat::UT_FLOAT,
		adjStartTime,
		adjEndTime,
		0.0,
		floatPeriod,
		notional,
		payReceiveFixed == UT_PayReceive::UT_RECEIVE ? UT_PayReceive::UT_PAY : UT_PayReceive::UT_RECEIVE));

	vector< shared_ptr< const UTProductBase > > underlyings(2);
	underlyings[0] = myFixedLeg;
	underlyings[1] = myFloatLeg;

	setLinearProductUnderlyings(underlyings);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
