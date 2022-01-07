/* UTValuationEngineProduct.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_VALUATION_ENGINE_PRODUCT_H
#define UT_VALUATION_ENGINE_PRODUCT_H

#include <memory>
#include <vector>

// Forward declaration

class UTModelBase;
class UTModelYieldCurve;
class UTProductBase;
class UTProductLinearBase;
class UTProductCashflowBase;
class UTProductCashflowBullet;
class UTProductCashflowRateFixed;
class UTProductCashflowRateFloat;
class UTValuationEngineProductBase;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UTValuationEngineProductBase
{
public:

	// Destructor.
	virtual ~UTValuationEngineProductBase() {};

	// Constructor.
	UTValuationEngineProductBase(const UTModelBase & model);


	// Calculates the PV of the Product and accumulate it in the ResultPV object.
	virtual void calculatePV( double& result );

	// Accessors
	const UTModelBase  & modelBase() const { return myModelBase; }

protected:

	const UTModelBase  & myModelBase;

};

///////////////////////////////////////////////////////////////////////////////
class UTValuationEngineAnalyticProductLinear : public UTValuationEngineProductBase
{
public:

	// Destructor.
	virtual ~UTValuationEngineAnalyticProductLinear() {};

	// Constructor.
	UTValuationEngineAnalyticProductLinear(const UTModelBase & model, const UTProductLinearBase & product);

	// Accessor
	const UTProductLinearBase&  linearProduct() const { return myProductLinear; }

	// Calculates the PV of the Product and accumulate it in the ResultPV object.
	virtual void calculatePV(double& result);

protected:


private:

	// Private functions

	unsigned int size() const { return mySubValuationEngines.size(); }
	UTValuationEngineProductBase& subValuationEngine(unsigned int i) { return *(mySubValuationEngines[i]); }
	const UTValuationEngineProductBase& subValuationEngine(unsigned int i) const  { return *(mySubValuationEngines[i]); }

	const UTProductLinearBase & myProductLinear;

	// This valuation engine contains lots of 'smaller' valuation engines: one for each (undetermined) sub-product in the product linear
	std::vector<std::unique_ptr<UTValuationEngineProductBase> > mySubValuationEngines;


};

///////////////////////////////////////////////////////////////////////////////
class UTValuationEngineProductCashflowBase : public UTValuationEngineProductBase
{
public:

	// Destructor.
	virtual ~UTValuationEngineProductCashflowBase() {};

	// Constructor.
	UTValuationEngineProductCashflowBase(const UTModelBase & model, const UTProductCashflowBase & product);


protected:

	// Accessors
	const UTProductCashflowBase & productCashFlowBase() const { return myProductCashFlowBase; }


private:

	const UTProductCashflowBase & myProductCashFlowBase;


};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// A yield curve model is used to value the following cashflows.
//
//  UTProductCashflowBullet
//  UTProductCashflowRateFixed
//  UTProductCashflowRateFloat

class UTValuationEngineAnalyticYieldCurveProductCashflowBullet : public UTValuationEngineProductCashflowBase
{
public:
	// Destructor.
	virtual ~UTValuationEngineAnalyticYieldCurveProductCashflowBullet() {}

	// Constructor.
	UTValuationEngineAnalyticYieldCurveProductCashflowBullet(
		const UTModelYieldCurve & model,
		const UTProductCashflowBullet  & cashFlow);

	// Calculates the PV of the Product and accumulate it in the ResultPV object, in all the currencies it contains -- NO heap allocation
	virtual void calculatePV(double& resultPv);


private:

	// References to the model and the cashflow.
	const UTModelYieldCurve   & myModel;
	const UTProductCashflowBullet  & myProductCashflow;

	// Calculated values.
	double                                          myValue;
	double                                          myPayment;
	double                                          myPaymentDf;
};

///////////////////////////////////////////////////////////////////////////////
class UTValuationEngineAnalyticYieldCurveProductCashflowRateFixed : public UTValuationEngineProductCashflowBase
{
public:
	// Destructor.
	virtual ~UTValuationEngineAnalyticYieldCurveProductCashflowRateFixed() {}

	// Constructor.
	UTValuationEngineAnalyticYieldCurveProductCashflowRateFixed(
		const UTModelYieldCurve & model,
		const UTProductCashflowRateFixed  & cashFlow);

	// Calculates the PV of the Product and accumulate it in the ResultPV object, in all the currencies it contains -- NO heap allocation
	virtual void calculatePV(double& resultPv);


private:

	// References to the model and the cashflow.
	const UTModelYieldCurve   & myModel;
	const UTProductCashflowRateFixed  & myProductCashflow;

	// Calculated values.
	double                                          myFlowPayment;
	double                                          myFlowValue;
	double                                          myFlowPaymentDf;
	double                                          myRate;

};

///////////////////////////////////////////////////////////////////////////////
class UTValuationEngineAnalyticYieldCurveProductCashflowRateFloat : public UTValuationEngineProductCashflowBase
{
public:
	// Destructor.
	virtual ~UTValuationEngineAnalyticYieldCurveProductCashflowRateFloat() {}

	// Constructor.
	UTValuationEngineAnalyticYieldCurveProductCashflowRateFloat(
		const UTModelYieldCurve & model,
		const UTProductCashflowRateFloat  & cashFlow);

	// Calculates the PV of the Product and accumulate it in the ResultPV object, in all the currencies it contains -- NO heap allocation
	virtual void calculatePV(double& resultPv);


private:

	// References to the model and the cashflow.
	const UTModelYieldCurve   & myModel;
	const UTProductCashflowRateFloat  & myProductCashflow;

	// Calculated values.
	double                                                      myFlowPayment;
	double                                                      myFlowValue;
	double                                                      myFlowPaymentDf;
	double                                                      myFlowPaymentWithZeroSpread;
	double                                                      myFlowValueWithZeroSpread;
	double                                                      myForwardRate;

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif // UT_VALUATION_ENGINE_PRODUCT_H