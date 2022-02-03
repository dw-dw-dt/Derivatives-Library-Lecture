/* UTModelFactory.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include <memory>

#include "UTEnum.hpp"
#include "UTModelFactory.hpp"
#include "UTProductSwap.hpp"
#include "UTProductEuropeanOption.hpp"
#include "UTEuropeanOptionLogNormal.hpp"
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

		// 1) polymorphism solution
		//UTSolveForModelComponent solver(*pYieldCurveModel, vanillaSwap, 0.0,i);
		//double rate = solver.root(-0.1, 1.0);

		// 2) Template solution
		UTSolveForModelComponent2 solver(*pYieldCurveModel, vanillaSwap, 0.0, i);
		double rate = bisection(solver, -0.1, 1.0);


	}

	// Calibration is done! So return the calibrated model
	return pYieldCurveModel;

}

///////////////////////////////////////////////////////////////////////////////
unique_ptr<UTModelBlackSholesDynamics>
UTModelFactory::newModelBlackSholesDynamics(
	double spotPrice,
	const vector<double> &    optionMaturities,
	const vector<double> &    impVols,
	const shared_ptr<const UTModelYieldCurve>& subYieldCurveModel,
	const UTModelBlackSholesDynamics::UT_InterpolationMethod& interpMethod,
	const UTModelBlackSholesDynamics::UT_InterpolateeType& interpolateeType)
{
	// Check the size of the 2 vectors
	if (optionMaturities.size() != impVols.size())
	{
		throw runtime_error("UTModelFactory: The size of option maturities and imp Vols should be the same.");
	}

	// Create a temporal Black Dynamics model with inputed interp method
	unique_ptr<UTModelBlackSholesDynamics> pBlackSholesDynamicsModel(new UTModelBlackSholesDynamics(spotPrice, optionMaturities, impVols, interpolateeType, interpMethod));
	pBlackSholesDynamicsModel->setModelYieldCurve(*subYieldCurveModel);

	//We are assuming that the option maturities and vols are ordered correctly...
	for (unsigned int i = 0; i < optionMaturities.size(); ++i)
	{
		// create product.
		UTProductEuropeanOptionCall vanillCallOption(optionMaturities[i], 1.0, UT_BuySell::UT_BUY, pBlackSholesDynamicsModel->forwardPrice(optionMaturities[i]));

		double maturity = optionMaturities[i];
		double fwdPrice = pBlackSholesDynamicsModel->forwardPrice(maturity);
		UTEuropeanOptionLogNormal black(fwdPrice, fwdPrice, maturity, impVols[i]);  //ATM forward
		double df = pBlackSholesDynamicsModel->df(maturity);

		// 1) polymorphism solution
		//UTSolveForModelComponent solver(*pBlackSholesDynamicsModel, vanillCallOption, black.premium(UT_CallPut::UT_CALL)*df, i);
		//double rate = solver.root(0.00001/*low*/, 3.0/*upper*/);

		// 2) Template solution
		UTSolveForModelComponent2 solver(*pBlackSholesDynamicsModel, vanillCallOption, black.premium(UT_CallPut::UT_CALL)*df, i);
		double rate = bisection(solver, 0.00001/*low*/, 3.0/*upper*/);


	}

	// Calibration is done! So return the calibrated model
	return pBlackSholesDynamicsModel;

}


//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
double
UTSolveForModelComponent::error(double x)
{
	// Pricing
	myModel.setComponent(myComponentNumber,x);

	// Usually Calibration is done to analytic prices...
	unique_ptr<UTValuationEngineBase>pricer(UTValuationEngineFactory::newValuationEngineAnalytic(myModel, myProduct)); 
	double pv = 0.0;
	pricer->calculatePV(pv);

	return pv - myTarget;
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
double
UTSolveForModelComponent2::error(double x)
{
	// Pricing
	myModel.setComponent(myComponentNumber, x);

	// Usually Calibratio is done to analytic prices...
	unique_ptr<UTValuationEngineBase>pricer(UTValuationEngineFactory::newValuationEngineAnalytic(myModel, myProduct));
	double pv = 0.0;
	pricer->calculatePV(pv);

	return pv - myTarget;
}
//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
