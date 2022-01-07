/* UTValuationEngineProduct.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTEnum.hpp"
#include "UTValuationEngineProduct.hpp"
#include "UTValuationEngineFactory.hpp"
#include "UTProductCashflow.hpp"
#include "UTProductSwap.hpp"
#include "UTModelYieldCurve.hpp"


using namespace std;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//UTValuationEngineProductBase
//
UTValuationEngineProductBase::UTValuationEngineProductBase(const UTModelBase & model)
	: myModelBase(model)
{
}

///////////////////////////////////////////////////////////////////////////////
void UTValuationEngineProductBase::calculatePV( double& resultPv )
{
	throw runtime_error("UTValuationEngineBase::calculatePV() base class function cannot be called.");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//UTValuationEngineProductCashflowBase
//
UTValuationEngineProductCashflowBase::UTValuationEngineProductCashflowBase(
	const UTModelBase           & model,
	const UTProductCashflowBase & product)
	:
	UTValuationEngineProductBase(model),
	myProductCashFlowBase(product)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//UTValuationEngineAnalytivYieldCurveProductLinear
//
UTValuationEngineAnalyticProductLinear::UTValuationEngineAnalyticProductLinear(const UTModelBase& model, const UTProductLinearBase & product)
	:
	UTValuationEngineProductBase(model),
	myProductLinear(product)
{
	unsigned long iSize = myProductLinear.size();

	mySubValuationEngines.resize(myProductLinear.size());
	for (unsigned long i = 0; i < myProductLinear.size(); ++i)
		mySubValuationEngines[i] = unique_ptr<UTValuationEngineProductBase>(UTValuationEngineFactory::newValuationEngineAnalytic(model, *myProductLinear.underlying(i)));
}

///////////////////////////////////////////////////////////////////////////////
// Calculates the PV of the Product and accumulate it 
void
UTValuationEngineAnalyticProductLinear::calculatePV(double& resultPv)
{
	// Loops though each sub-valuation engine to accumulate the PV of each component or leg (or sub-product...)
	for (unsigned int i = 0; i < size(); ++i)
	{
		subValuationEngine(i).calculatePV(resultPv);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//  UTValuationEngineAnalyticYieldCurveProductCashflowBullet
//
UTValuationEngineAnalyticYieldCurveProductCashflowBullet::UTValuationEngineAnalyticYieldCurveProductCashflowBullet(
	const UTModelYieldCurve & model,
	const UTProductCashflowBullet  & cashFlow)
	: UTValuationEngineProductCashflowBase(model, cashFlow),
	myModel(model),
	myProductCashflow(cashFlow)
{
	// The payment date and payment.
	double payTime = myProductCashflow.paymentTime();
	myPayment = myProductCashflow.amount();

	// The value is signed by whether we pay (-) or receive (+).
	if (myProductCashflow.payReceive() == UT_PayReceive::UT_PAY)
	{
		myPayment *= -1.0;
	}

	// Check the payment date is after the value date.
	if (payTime >= 0.0)
	{
		// Get the discount factor from the curve.
		myPaymentDf = myModel.df(payTime);
		myValue = myPayment * myPaymentDf;
	}
	else
	{
		myPaymentDf = 1.0;
		myValue = 0.0;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Accumulates the PV of the current product
void
UTValuationEngineAnalyticYieldCurveProductCashflowBullet::calculatePV(double& resultPv )
{	
	// Accumulate only Pv
	resultPv += myValue;

}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//  UTValuationEngineAnalyticYieldCurveProductCashflowRateFixed
//
UTValuationEngineAnalyticYieldCurveProductCashflowRateFixed::UTValuationEngineAnalyticYieldCurveProductCashflowRateFixed(
	const UTModelYieldCurve & model,
	const UTProductCashflowRateFixed & cashFlow)
	: UTValuationEngineProductCashflowBase(model, cashFlow),
	myModel(model),
	myProductCashflow(cashFlow)
{
	// The rate is just a fixed coupon.
	myRate = myProductCashflow.coupon();

	// The payment
	const double paymentTime = myProductCashflow.paymentTime();
	myFlowPayment = myProductCashflow.notional() * myProductCashflow.accrued() * myRate;

	// The value is signed by whether we pay (-) or receive (+).
	if (myProductCashflow.payReceive() == UT_PayReceive::UT_PAY)
	{
		myFlowPayment *= -1.0;
	}

	// Check the payment date is after the value date.
	if (paymentTime >=0.0)
	{
		// Get the discount factor from the curve.
		myFlowPaymentDf = myModel.df(paymentTime);
		myFlowValue = myFlowPayment * myFlowPaymentDf;
	}
	else
	{
		myFlowPaymentDf = 1.0;
		myFlowValue = 0.0;

	}
}

///////////////////////////////////////////////////////////////////////////////
// Accumulates the PV of the current product
void
UTValuationEngineAnalyticYieldCurveProductCashflowRateFixed::calculatePV( double& resultPv )
{
	// Accumulate only PV
	resultPv += myFlowValue;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//  UTValuationEngineAnalyticYieldCurveProductCashflowRateFloat
//
UTValuationEngineAnalyticYieldCurveProductCashflowRateFloat::UTValuationEngineAnalyticYieldCurveProductCashflowRateFloat(
	const UTModelYieldCurve          & model,
	const UTProductCashflowRateFloat & cashFlow)
	:
	UTValuationEngineProductCashflowBase(model, cashFlow),
	myModel(model),
	myProductCashflow(cashFlow)
{
	const double paymentTime = myProductCashflow.paymentTime();

	// If the payment date is STRICTLY before the model value date, do NOT take it into account
	if (paymentTime <  0.0)
	{
		myFlowPaymentDf = 1.0;
		myFlowPayment = 0.0;
		myFlowPaymentWithZeroSpread = 0.0;
		myForwardRate = 0.0;
	}
	else
		// The payment date is on or after the value date, take the payment into account
	{
		// Calculate the (forward) value of the libor index (payment forward measure).
		myForwardRate = (myModel.df(myProductCashflow.startTime()) / myModel.df(myProductCashflow.endTime()) - 1.0) / myProductCashflow.accrued();

		// The payment, and its 'derivatives' w.r.t. the spread.
		double adjustedNotional = myProductCashflow.notional() * myProductCashflow.accrued();
		myFlowPayment = adjustedNotional * (myForwardRate + myProductCashflow.spread());
		myFlowPaymentWithZeroSpread = adjustedNotional * myForwardRate;

		// Get the discount factor from the curve.
		myFlowPaymentDf = myModel.df(paymentTime);
	}


	// The value is signed by whether we pay (-) or receive (+).
	if (myProductCashflow.payReceive() == UT_PayReceive::UT_PAY)
	{
		myFlowPayment *= -1.0;
		myFlowPaymentWithZeroSpread *= -1.0;
	}

	myFlowValue = myFlowPayment * myFlowPaymentDf;
	myFlowValueWithZeroSpread = myFlowPaymentWithZeroSpread * myFlowPaymentDf;
}

///////////////////////////////////////////////////////////////////////////////
// Accumulates the PV of the current product
void
UTValuationEngineAnalyticYieldCurveProductCashflowRateFloat::calculatePV(double& resultPv)
{
	// Accumulate only PV
	resultPv += myFlowValue;

}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
