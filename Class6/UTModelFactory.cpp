/* UTModelFactory.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include <memory>

#include "UTEnum.hpp"
#include "UTModelFactory.hpp"
#include "UTProductSwap.hpp"
#include "UTValuationEngineFactory.hpp"
#include "UTBisection.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unique_ptr<UTModelYieldCurve>
UTModelFactory::newModelYieldCurve(
const vector<double> &    swapMaturities,
const vector<double> &    swapRates,
const UTModelYieldCurve::UT_InterpolationMethod& interpMethod,
const UTModelYieldCurve::UT_InterpolateeType& interpolateeType)
{
	// Check the size of the 2 vectors
	if (swapMaturities.size() != swapRates.size())
	{
		throw runtime_error("UTModelFactory: The size of swap maturities and rates should be the same.");
	}

	// Create a temporal yield curve model with inputed interp method
	unique_ptr<UTModelYieldCurve> pYieldCurveModel(new UTModelYieldCurve(swapMaturities, swapRates, interpolateeType, interpMethod));

	//We are assuming that the swap maturities and rates are ordered correctly...
	for (unsigned int i = 0; i < swapMaturities.size(); ++i)
	{
		// create product.
		// Assuming the swap convention is like JPY swaps..
		UTProductSwapVanilla vanillaSwap(0, swapMaturities[i], swapRates[i], 0.5, 0.5, 10000.0, UT_PayReceive:: UT_RECEIVE);

		UTSolveForModelComponent solver(*pYieldCurveModel, vanillaSwap, 0.0,i);
		double rate = solver.root(-0.1, 1.0);


	}

	// Calibration is done! So return the calibrated model
	return pYieldCurveModel;

}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
double
UTSolveForModelComponent::error(double x)
{
	// Pricing
	myModel.setComponent(myComponentNumber,x);

	// Usually Calibration is done to analytic prices...
	unique_ptr<UTValuationEngineProductBase>pricer(UTValuationEngineFactory::newValuationEngineAnalytic(myModel, myProduct)); 
	double pv = 0.0;
	pricer->calculatePV(pv);

	return pv - myTarget;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
