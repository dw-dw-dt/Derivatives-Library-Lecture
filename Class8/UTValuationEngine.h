/* UTValuationEngine.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_VALUATION_ENGINE_H
#define UT_VALUATION_ENGINE_H

#include <memory>
#include <vector>

// Forward declaration
class UTModelBase;
class UTModelYieldCurve;
class UTModelBlackSholesDynamics;
class UTProductBase;
class UTProductLinearBase;
class UTProductCashflowBase;
class UTProductCashflowBullet;
class UTProductCashflowRateFixed;
class UTProductCashflowRateFloat;
class UTProductEuropeanOptionBase;
class UTProductEuropeanOptionCall;
class UTProductEuropeanOptionPut;
class UTProductPathDependentAsian;
class UTValuationEngineBase;
class UTRandomBase;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UTValuationEngineBase
{
public:

	// Destructor.
	virtual ~UTValuationEngineBase() {};

	// Constructor.
	UTValuationEngineBase(const UTModelBase & model);


	// Calculates the PV of the Product and accumulate it in the ResultPV object.
	virtual void calculatePV( double& result );

	// Accessors
	const UTModelBase  & modelBase() const { return myModelBase; }

protected:

	const UTModelBase  & myModelBase;

};

///////////////////////////////////////////////////////////////////////////////
class UTValuationEngineAnalyticLinearBase : public UTValuationEngineBase
{
public:

	// Destructor.
	virtual ~UTValuationEngineAnalyticLinearBase() {};

	// Constructor.
	UTValuationEngineAnalyticLinearBase(const UTModelBase & model, const UTProductLinearBase & product);

	// Accessor
	const UTProductLinearBase&  linearProduct() const { return myProductLinear; }

	// Calculates the PV of the Product and accumulate it in the ResultPV object.
	virtual void calculatePV(double& result);

protected:


private:

	// Private functions

	unsigned int size() const { return mySubValuationEngines.size(); }
	UTValuationEngineBase& subValuationEngine(unsigned int i) { return *(mySubValuationEngines[i]); }
	const UTValuationEngineBase& subValuationEngine(unsigned int i) const  { return *(mySubValuationEngines[i]); }

	const UTProductLinearBase & myProductLinear;

	// This valuation engine contains lots of 'smaller' valuation engines: one for each (undetermined) sub-product in the product linear
	std::vector<std::unique_ptr<UTValuationEngineBase> > mySubValuationEngines;


};

///////////////////////////////////////////////////////////////////////////////
class UTValuationEngineCashflowBase : public UTValuationEngineBase
{
public:

	// Destructor.
	virtual ~UTValuationEngineCashflowBase() {};

	// Constructor.
	UTValuationEngineCashflowBase(const UTModelBase & model, const UTProductCashflowBase & product);


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

class UTValuationEngineAnalyticYieldCurveCashflowBullet : public UTValuationEngineCashflowBase
{
public:
	// Destructor.
	virtual ~UTValuationEngineAnalyticYieldCurveCashflowBullet() {}

	// Constructor.
	UTValuationEngineAnalyticYieldCurveCashflowBullet(
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
class UTValuationEngineAnalyticYieldCurveCashflowRateFixed : public UTValuationEngineCashflowBase
{
public:
	// Destructor.
	virtual ~UTValuationEngineAnalyticYieldCurveCashflowRateFixed() {}

	// Constructor.
	UTValuationEngineAnalyticYieldCurveCashflowRateFixed(
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
class UTValuationEngineAnalyticYieldCurveCashflowRateFloat : public UTValuationEngineCashflowBase
{
public:
	// Destructor.
	virtual ~UTValuationEngineAnalyticYieldCurveCashflowRateFloat() {}

	// Constructor.
	UTValuationEngineAnalyticYieldCurveCashflowRateFloat(
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
class UTValuationEngineEuropeanOptionBase : public UTValuationEngineBase
{
public:

	// Destructor.
	virtual ~UTValuationEngineEuropeanOptionBase() {};

	// Constructor.
	UTValuationEngineEuropeanOptionBase(const UTModelBase & model, const UTProductEuropeanOptionBase & product);


	// Calculates the PV of the Product and accumulate it
	virtual void calculatePV(double& result);

protected:

	const UTProductEuropeanOptionBase & myProductEuropeanOptionBase;

	// Calculated values.
	double                                          myValue;
	double                                          myPayment;
	double                                          myPaymentDf;

};
///////////////////////////////////////////////////////////////////////////////
// A Black Dynamics model is used to value the following productss.
//
//  UTProductEuropeanOptionCall
//  UTProductEuropeanOptionPut
//  ......
///////////////////////////////////////////////////////////////////////////////
class UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionCall : public UTValuationEngineEuropeanOptionBase
{
public:
	// Destructor.
	virtual ~UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionCall() {}

	// Constructor.
	UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionCall(
		const UTModelBlackSholesDynamics & model,
		const UTProductEuropeanOptionCall  & product);


private:

	// References to the model and the product
	const UTModelBlackSholesDynamics   & myModel;
	const UTProductEuropeanOptionCall   & myProduct;

};

///////////////////////////////////////////////////////////////////////////////
class UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionPut : public UTValuationEngineEuropeanOptionBase
{
public:
	// Destructor.
	virtual ~UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionPut() {}

	// Constructor.
	UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionPut(
		const UTModelBlackSholesDynamics & model,
		const UTProductEuropeanOptionPut  & product);


private:

	// References to the model and the product
	const UTModelBlackSholesDynamics   & myModel;
	const UTProductEuropeanOptionPut   & myProduct;

};

///////////////////////////////////////////////////////////////////////////////
class UTValuationEngineAnalyticBlackSholesDynamicsPathDependentAsianGeometric : public UTValuationEngineBase
{
public:
	// Destructor.
	virtual ~UTValuationEngineAnalyticBlackSholesDynamicsPathDependentAsianGeometric() {}

	// Constructor.
	UTValuationEngineAnalyticBlackSholesDynamicsPathDependentAsianGeometric(
		const UTModelBlackSholesDynamics & model,
		const UTProductPathDependentAsian  & product);

	// Calculates the PV of the Product and accumulate it
	virtual void calculatePV(double& result);

private:

	// References to the model and the product
	const UTModelBlackSholesDynamics   & myModel;
	const UTProductPathDependentAsian   & myProduct;

	// Calculated values.
	double                                          myValue;
	double                                          myPayment;
	double                                          myPaymentDf;



};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif // UT_VALUATION_ENGINE_H