/* UTProductPathDependent.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef  UT_PRODUCT_PATH_DEPENDENT_H
#define  UT_PRODUCT_PATH_DEPENDENT_H

#include <string>
#include <vector>

#include "UTEnum.hpp"
#include "UTProductCashflow.hpp"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//  Abstract base class for the entire path dependent products
class  UTProductPathDependentBase : public UTProductBase
{
public:

	// Destructor.
	virtual ~UTProductPathDependentBase() {}

	// Constructors.
	UTProductPathDependentBase(
		double notional,
		UT_BuySell  buySell)
		: UTProductBase(), myNotional(notional), myBuySell(buySell){}

	// Inherited from  UTProductBase.
	virtual std::string classTag() const = 0;
	virtual const std::vector<double>& timeLine() const { return myTimeLine; }

	// Accessors
	double notional() const { return myNotional; }
	UT_BuySell buySell() const { return myBuySell; }

protected:

	std::vector<double>	myTimeLine;

private:

	double				myNotional;
	UT_BuySell			myBuySell;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//  Asian option (Arithmetic or Geometric)
class UTProductPathDependentAsian : public UTProductPathDependentBase
{
public:

	// The class name string.
	static const std::string ourClassTag;

	// Destructor.
	virtual ~UTProductPathDependentAsian() {}

	// Constructors.
	UTProductPathDependentAsian(
		double		 averageStartTime,
		double		 expiryTime,
		unsigned long numberOfAverage,
		double       notional,
		UT_CallPut callPut,
		UT_BuySell   buySell,
		double strike,
		UT_AverageType averageType = UT_AverageType::UT_ARITHMETIC );


	// Inherited from UTProductBase
	virtual std::string classTag() const { return ourClassTag; }
	virtual double firstTime() const { return myAverageStartTime; }
	virtual double lastTime() const { return myExpiryTime; }
	virtual unsigned long payoffs(const std::vector<double> spotPrices, std::vector<UTCashflows_t> &cashflows) const;
	virtual const std::vector<double> cashflowPayTimes() const;


	// accessors   
	double strike() const                        { return myStrike; }
	double averageStartTime() const              { return myAverageStartTime; }
	double expiryTime() const                    { return myExpiryTime; }
	unsigned long numberOfAverage() const        { return myNumberOfAverage; }
	const UT_CallPut& callPut() const            { return myCallPut; }
	const UT_AverageType& averageType() const    { return myAverageType; }

private:

	double myAverageStartTime;
	double myExpiryTime;
	unsigned long myNumberOfAverage;
	UT_CallPut myCallPut;
	double  myStrike;
	UT_AverageType myAverageType;

};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif // UT_PRODUCT_PATH_DEPENDENT_H
