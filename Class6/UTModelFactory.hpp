#ifndef UT_MODEL_FACTORY_H
#define UT_MODEL_FACTORY_H


#include <vector>

/* UTModelFactory.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTModelYieldCurve.hpp"
#include "UTProductCashflow.hpp"
#include "UTBisection.hpp"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// UTModelFactory
//
class UTModelFactory
{
public:

	// Destructor.
	virtual ~UTModelFactory() {}

	// Constructor.
	UTModelFactory() {}

	// Creates a 'calibrated' yield curve model
	static std::unique_ptr<UTModelYieldCurve> newModelYieldCurve(
		const std::vector<double> &    swapMaturities,
		const std::vector<double> &    swapRates,
		const UTModelYieldCurve::UT_InterpolationMethod& interpMethod = UTModelYieldCurve::UT_FLAT,
		const UTModelYieldCurve::UT_InterpolateeType& interpolateeType = UTModelYieldCurve::UT_FWD_RATE );

};

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//Helper Class for model calibration
class UTSolveForModelComponent : public UTBisection
{
	// Object for this class: Calibration of the given model

public:

	virtual ~UTSolveForModelComponent() {}

	UTSolveForModelComponent(UTModelBase &model, const UTProductBase& product, double target, unsigned int componentNumber)
		: UTBisection(),
		myModel(model),
		myProduct(product),
		myTarget(target),
		myComponentNumber(componentNumber){}

	// Overloaded penalty function
	virtual double error (double x);

private:

	UTModelBase &	myModel;
	const UTProductBase&  myProduct;
	double myTarget;
	unsigned int myComponentNumber;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif  // UT_MODEL_FACTORY_H
