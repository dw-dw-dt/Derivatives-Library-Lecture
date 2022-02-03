/* UTProductCashflows.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef  UT_PRODUCT_CASHFLOW_H
#define  UT_PRODUCT_CASHFLOW_H

#include <string>
#include <vector>
#include "UTProductBase.h"

enum class UT_PayReceive;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//  Abstract base class for the entire cashflow hierarchy
class  UTProductCashflowBase : public UTProductBase
{
public:

	// Destructor.
	virtual ~UTProductCashflowBase() {}

	// Constructors.
	UTProductCashflowBase(
		double  paymentTime,
		UT_PayReceive       payReceive)
		: UTProductBase(), myPaymentTime(paymentTime), myPayReceive(payReceive){}

	// Accessors
	 double paymentTime() const { return myPaymentTime; }
	 UT_PayReceive payReceive() const { return myPayReceive; }

private:

	double				myPaymentTime;
	 UT_PayReceive      myPayReceive;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Bullet cashflow
class UTProductCashflowBullet : public UTProductCashflowBase
{
public:

	// The class name string.
	 static const std::string ourClassTag;

	// Destructor.
	 virtual ~ UTProductCashflowBullet() {}

	// Constructors.
	 UTProductCashflowBullet(
		 double				paymentTime,
		 UT_PayReceive      payReceive,
		 double             amount )
		: UTProductCashflowBase(paymentTime, payReceive),
		myAmount(amount) {}

	// Inherited from UTProductCashflowBase
	 virtual std::string classTag() const { return ourClassTag; }
	 virtual double firstTime() const { return paymentTime(); }
	 virtual double lastTime() const  { return paymentTime(); }

	// The amount of this bullet cashflow.    
	 double amount() const                        { return myAmount; }


private:

	double  myAmount;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// IR Fixed cashflow 
class UTProductCashflowRateFixed : public UTProductCashflowBase
{
public:

	// The class name string.
	 static std::string const ourClassTag;

	// Destructor.
	 virtual ~ UTProductCashflowRateFixed() {}

	// Constructors.
	 UTProductCashflowRateFixed(
		double						startTime,
		double                      endTime,
		UT_PayReceive               payReceive,
		double                      notional,
		double                      coupon )
		: UTProductCashflowBase(endTime, payReceive),
		myStartTime(startTime), myEndTime(endTime), myCoupon(coupon), myNotional(notional) {}

	// Inherited from UTProductCashflowBase.
	 virtual std::string classTag() const         { return ourClassTag; }
	 virtual double firstTime() const { return myStartTime; }
	 virtual double lastTime() const  { return myEndTime; }

	 // Accessors
	 double coupon() const { return myCoupon; }
	 double paymentTime() const { return myEndTime; }
	 double endTime() const { return myEndTime; }
	 double startTime() const { return myStartTime; }
	 double notional() const { return myNotional; }

	 double accrued() const { return myEndTime - myStartTime; }


private:
	double	myStartTime;
	double  myEndTime;
	double	myCoupon;
	double	myNotional;

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// IR Float cashflow 
class UTProductCashflowRateFloat : public UTProductCashflowBase
{
public:

	// The class name string.
	static std::string const ourClassTag;

	// Destructor.
	virtual ~UTProductCashflowRateFloat() {}

	// Constructors.
	 UTProductCashflowRateFloat(
		 double						 startTime,
		 double                      endTime,
		 UT_PayReceive               payReceive,
		 double                      notional,
		 double                      coupon)
		 : UTProductCashflowBase(endTime, payReceive),
		 myStartTime(startTime), myEndTime(endTime), mySpread(coupon), myNotional(notional) {}

	// Inherited from UTProductCashflowBase.
	virtual std::string classTag() const  { return ourClassTag; }
	virtual double firstTime() const { return myStartTime; }
	virtual double lastTime() const  { return myEndTime; }

	// Accessors
	double spread() const { return mySpread; }
	double paymentTime() const { return myEndTime; }
	double endTime() const { return myEndTime; }
	double startTime() const { return myStartTime; }
	double notional() const { return myNotional; }

	double accrued() const { return myEndTime - myStartTime; }


private:
	double	myStartTime;
	double  myEndTime;
	double  mySpread;
	double  myNotional;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif // UT_PRODUCT_CASHFLOW_H
