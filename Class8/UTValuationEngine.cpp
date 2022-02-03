/* UTValuationEngine.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTEnum.h"
#include "UTValuationEngine.h"
#include "UTValuationEngineFactory.h"
#include "UTProductCashflow.h"
#include "UTProductSwap.h"
#include "UTProductEuropeanOption.h"
#include "UTProductPathDependent.h"
#include "UTModelYieldCurve.h"
#include "UTModelBlackSholesDynamics.h"
#include "UTEuropeanOptionLogNormal.h"
#include <stdexcept>

using namespace std;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//UTValuationEngineBase
//
UTValuationEngineBase::UTValuationEngineBase(const UTModelBase & model)
	: myModelBase(model)
{
}

///////////////////////////////////////////////////////////////////////////////
void UTValuationEngineBase::calculatePV( double& resultPv )
{
	throw runtime_error("UTValuationEngineBase::calculatePV() base class function cannot be called.");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//UTValuationEngineCashflowBase
//
UTValuationEngineCashflowBase::UTValuationEngineCashflowBase(
	const UTModelBase           & model,
	const UTProductCashflowBase & product)
	:
	UTValuationEngineBase(model),
	myProductCashFlowBase(product)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//UTValuationEngineAnalyticYieldCurveLinear
//
UTValuationEngineAnalyticLinearBase::UTValuationEngineAnalyticLinearBase(const UTModelBase& model, const UTProductLinearBase & product)
	:
	UTValuationEngineBase(model),
	myProductLinear(product)
{
	unsigned long iSize = myProductLinear.size();

	mySubValuationEngines.resize(myProductLinear.size());
	for (unsigned long i = 0; i < myProductLinear.size(); ++i)
		mySubValuationEngines[i] = unique_ptr<UTValuationEngineBase>(UTValuationEngineFactory::newValuationEngineAnalytic(model, *myProductLinear.underlying(i)));
}

///////////////////////////////////////////////////////////////////////////////
// Calculates the PV of the Product and accumulate it 
void
UTValuationEngineAnalyticLinearBase::calculatePV(double& resultPv)
{
	// Loops though each sub-valuation engine to accumulate the PV of each component or leg (or sub-product...)
	for (unsigned int i = 0; i < size(); ++i)
	{
		subValuationEngine(i).calculatePV(resultPv);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//  UTValuationEngineAnalyticYieldCurveCashflowBullet
//
UTValuationEngineAnalyticYieldCurveCashflowBullet::UTValuationEngineAnalyticYieldCurveCashflowBullet(
	const UTModelYieldCurve & model,
	const UTProductCashflowBullet  & cashFlow)
	: UTValuationEngineCashflowBase(model, cashFlow),
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
UTValuationEngineAnalyticYieldCurveCashflowBullet::calculatePV(double& resultPv )
{	
	// Accumulate only Pv
	resultPv += myValue;

}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//  UTValuationEngineAnalyticYieldCurveCashflowRateFixed
//
UTValuationEngineAnalyticYieldCurveCashflowRateFixed::UTValuationEngineAnalyticYieldCurveCashflowRateFixed(
	const UTModelYieldCurve & model,
	const UTProductCashflowRateFixed & cashFlow)
	: UTValuationEngineCashflowBase(model, cashFlow),
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
UTValuationEngineAnalyticYieldCurveCashflowRateFixed::calculatePV( double& resultPv )
{
	// Accumulate only PV
	resultPv += myFlowValue;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//  UTValuationEngineAnalyticYieldCurveCashflowRateFloat
//
UTValuationEngineAnalyticYieldCurveCashflowRateFloat::UTValuationEngineAnalyticYieldCurveCashflowRateFloat(
	const UTModelYieldCurve          & model,
	const UTProductCashflowRateFloat & cashFlow)
	:
	UTValuationEngineCashflowBase(model, cashFlow),
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
		myForwardRate = myModel.forwardRate(myProductCashflow.startTime(), myProductCashflow.endTime());

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
UTValuationEngineAnalyticYieldCurveCashflowRateFloat::calculatePV(double& resultPv)
{
	// Accumulate only PV
	resultPv += myFlowValue;

}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//UTValuationEngineEuropeanOptionBase
//
UTValuationEngineEuropeanOptionBase::UTValuationEngineEuropeanOptionBase(
	const UTModelBase           & model,
	const UTProductEuropeanOptionBase & product)
	:
	UTValuationEngineBase(model),
	myProductEuropeanOptionBase(product)
{
}

///////////////////////////////////////////////////////////////////////////////
// Accumulates the PV of the current product
void
UTValuationEngineEuropeanOptionBase::calculatePV(double& resultPv)
{
	// Accumulate only Pv
	resultPv += myValue;

}
//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionCall
//
UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionCall::UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionCall(
	const UTModelBlackSholesDynamics& model,
	const UTProductEuropeanOptionCall & product)
	: UTValuationEngineEuropeanOptionBase(model, product),
	myModel(model),
	myProduct(product)
{
	double expiry = myProduct.expiryTime();

	UTEuropeanOptionLogNormal black(myModel.forwardPrice(expiry), myProduct.strike(), expiry, myModel.vol(expiry));

	myPayment = product.notional() * black.premium(UT_CallPut::UT_CALL);
	myPaymentDf = myModel.df(expiry);
	myValue = myPayment * myPaymentDf;

}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionPut
//
UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionPut::UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionPut(
	const UTModelBlackSholesDynamics& model,
	const UTProductEuropeanOptionPut & product)
	: UTValuationEngineEuropeanOptionBase(model, product),
	myModel(model),
	myProduct(product)
{
	double expiry = myProduct.expiryTime();

	UTEuropeanOptionLogNormal black(myModel.forwardPrice(expiry), myProduct.strike(), expiry, myModel.vol(expiry));

	myPayment = product.notional() * black.premium(UT_CallPut::UT_PUT);
	myPaymentDf = myModel.df(expiry);
	myValue = myPayment * myPaymentDf;

}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//UTValuationEngineAnalyticBlackSholesDynamicsPathDependentAsianGeometric
//
UTValuationEngineAnalyticBlackSholesDynamicsPathDependentAsianGeometric::UTValuationEngineAnalyticBlackSholesDynamicsPathDependentAsianGeometric(
	const UTModelBlackSholesDynamics& model,
	const UTProductPathDependentAsian & product)
	: UTValuationEngineBase(model),
	myModel(model),
	myProduct(product)
{
	if (product.averageType() != UT_AverageType::UT_GEOMETRIC)
	{
		throw runtime_error("UTValuationEngineAnalyticBlackSholesDynamicsPathDependentAsianGeometric: Only geometric average can have a closed form.");
	}

	// Calculate all the quantities including PV. 
	double expiry = myProduct.expiryTime();
	auto timeLine = myProduct.timeLine();
	auto numberOfaverage = timeLine.size();

	// Calculate today's "average foward"
	double tmp = 1.0;
	double diagonalVolSum = 0;
	for (unsigned long i = 0; i < numberOfaverage; ++i)
	{
		tmp *= myModel.forwardPrice(timeLine[i]);
		diagonalVolSum += model.logVariance(0.0, timeLine[i]);
	}
	double averageFwd = pow(tmp, 1.0 / numberOfaverage);

	double nonDiagonalVolSum = 0;
	for (unsigned long i = 0; i < numberOfaverage; ++i)
	{
		for (unsigned long j = 0; j < i; ++j)
		{
			nonDiagonalVolSum += 2.0* model.logVariance(0.0, timeLine[j]);
		}
	}

	double totalVolSum = diagonalVolSum + nonDiagonalVolSum;
	double vol = sqrt(totalVolSum / expiry) / numberOfaverage;
	double dividend = 0.5 *  diagonalVolSum / numberOfaverage  * (1.0 / numberOfaverage - 1) + 0.5*nonDiagonalVolSum / numberOfaverage / numberOfaverage;
	double forward = averageFwd * exp(dividend);

	UTEuropeanOptionLogNormal black(forward, myProduct.strike(), expiry, vol);

	if (product.callPut() == UT_CallPut:: UT_CALL)
	{
		myPayment = product.notional() * static_cast<int>(product.buySell())* black.premium(UT_CallPut:: UT_CALL);

	}
	else
	{
		myPayment = product.notional() * static_cast<int>(product.buySell())*black.premium(UT_CallPut::UT_PUT);

	}
	myPaymentDf = myModel.df(expiry);
	myValue = myPayment * myPaymentDf;

}

///////////////////////////////////////////////////////////////////////////////
// Accumulates the PV of the current product
void
UTValuationEngineAnalyticBlackSholesDynamicsPathDependentAsianGeometric::calculatePV(double& resultPv)
{
	// Accumulate only Pv
	resultPv += myValue;

}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
