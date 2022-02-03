/* UTProductEuropeanOption.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef  UT_PRODUCT_EUROPEAN_OPTION_H
#define  UT_PRODUCT_EUROPEAN_OPTION_H

#include <string>
#include "UTEnum.h"
#include "UTProductCashflow.h"
#include "UTProductSwap.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//  Abstract base class for the entire European option (call, put, digital, double digital...) hierarchy
class  UTProductEuropeanOptionBase : public virtual UTProductBase
{
public:

	// Destructor.
	virtual ~UTProductEuropeanOptionBase() {}

	// Constructors.
	UTProductEuropeanOptionBase(
		double  expiryTime,
		double notional,
		UT_BuySell  buySell)
		: UTProductBase(), myExpiryTime(expiryTime), myNotional(notional), myBuySell(buySell), myTimeLine(1, expiryTime){}

	// Inherited from  UTProductBase.

	virtual std::string classTag() const = 0;
	virtual double payoff (double spot) const = 0;
	virtual double firstTime() const { return myExpiryTime; }
	virtual double lastTime() const { return myExpiryTime; }

	virtual const std::vector<double>& timeLine() const { return myTimeLine; }
	virtual unsigned long payoffs(const std::vector<double> spotPrices, std::vector<UTCashflows_t> &cashflows) const;
	virtual const std::vector<double> cashflowPayTimes() const;

	// Accessors
	double expiryTime() const { return myExpiryTime; }
	double notional() const { return myNotional; }
	UT_BuySell buySell() const { return myBuySell; }

private:

	double				myExpiryTime;
	double				myNotional;  
	UT_BuySell			myBuySell;

	std::vector<double>	myTimeLine;

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Vanilla Call option
class UTProductEuropeanOptionCall : public UTProductEuropeanOptionBase
{
public:

	// The class name string.
	static const std::string ourClassTag;

	// Destructor.
	virtual ~UTProductEuropeanOptionCall() {}

	// Constructors.
	UTProductEuropeanOptionCall(
		double		 expiryTime,
		double       notional,
		UT_BuySell   buySell,
		double strike)
		: UTProductEuropeanOptionBase(expiryTime, notional, buySell),
		myStrike(strike){}

	// Inherited from UTProductCashflowBase
	virtual std::string classTag() const { return ourClassTag; }

	virtual double payoff(double spot) const { return spot - myStrike > 0.0 ? spot - myStrike: 0.0; }

	// The amount of this bullet cashflow.    
	double strike() const                        { return myStrike; }


private:

	double  myStrike;
};

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Vanilla Put option
class UTProductEuropeanOptionPut : public UTProductEuropeanOptionBase
{
public:

	// The class name string.
	static const std::string ourClassTag;

	// Destructor.
	virtual ~UTProductEuropeanOptionPut() {}

	// Constructors.
	UTProductEuropeanOptionPut(
		double		 expiryTime,
		double       notional,
		UT_BuySell   buySell,
		double strike)
		: UTProductEuropeanOptionBase(expiryTime, notional, buySell),
		myStrike(strike){}

	// Inherited from UTProductCashflowBase
	virtual std::string classTag() const { return ourClassTag; }

	virtual double payoff(double spot) const { return myStrike - spot> 0.0 ? myStrike - spot: 0.0; }

	// The amount of this bullet cashflow.    
	double strike() const                        { return myStrike; }


private:

	double  myStrike;
};

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Vanilla straddle option
class UTProductEuropeanOptionStraddle : public UTProductEuropeanOptionBase, public UTProductLinearBase
{
public:

	// The class name string.
	static const std::string ourClassTag;

	// Destructor.
	virtual ~UTProductEuropeanOptionStraddle() {}

	// Constructors.
	UTProductEuropeanOptionStraddle(
		double		 expiryTime,
		double       notional,
		UT_BuySell   buySell,
		double strike );

	// Inherited from UTProductCashflowBase
	virtual std::string classTag() const { return ourClassTag; }

	// to resolve ambiguity from 2 basess
	virtual double firstTime() const { return myCallOption->firstTime(); }
	virtual double lastTime() const  { return myCallOption->lastTime(); }
	virtual unsigned long payoffs(const std::vector<double> spotPrices, std::vector<UTCashflows_t> &cashflows) const { return UTProductEuropeanOptionBase::payoffs(spotPrices, cashflows); }
	virtual const std::vector<double> cashflowPayTimes() const { return UTProductEuropeanOptionBase::cashflowPayTimes(); }
	virtual const std::vector<double> &timeLine() const { return UTProductEuropeanOptionBase::timeLine(); }

	virtual double payoff(double spot) const { return myCallOption->payoff(spot) + myPutOption->payoff(spot); }

	// The amount of this bullet cashflow.    
	double strike() const                        { return myStrike; }

	// Accessors.
	const std::shared_ptr<const UTProductEuropeanOptionCall> & callOption() const { return myCallOption; }
	const std::shared_ptr<const UTProductEuropeanOptionPut> & putOption() const { return myPutOption; }

private:
	std::shared_ptr<const UTProductEuropeanOptionCall>  myCallOption;
	std::shared_ptr<const UTProductEuropeanOptionPut>  myPutOption;

	double  myStrike;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif // UT_PRODUCT_EUROPEAN_OPTION_H
