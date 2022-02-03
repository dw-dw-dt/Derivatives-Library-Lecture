/* UTValuationEngineMonteCarlo.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTValuationEngineMonteCarlo.h"
#include "UTProductCashflow.h"
#include "UTProductSwap.h"
#include "UTProductEuropeanOption.h"
#include "UTRandomBase.h"
#include "UTModelYieldCurve.h"
#include "UTModelBlackSholesDynamics.h"
#include "UTEuropeanOptionLogNormal.h"


using namespace std;


//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//UTValuationEngineMonteCarlo

UTValuationEngineMonteCarlo::UTValuationEngineMonteCarlo(
	const UTModelBase & model,
	const UTProductBase & product,
	const UTWrapper<UTRandomBase> & generator,
	unsigned long numberOfPaths )
	: UTValuationEngineBase(model),
	myProductBase(product),
	myGenerator(generator),
	myNumberOfPaths(numberOfPaths),
	myDf(product.cashflowPayTimes().size())
{

	for (unsigned long i = 0; i < myDf.size(); ++i)
	{
		myDf[i] = modelBase().df(myProductBase.cashflowPayTimes()[i]);
	}
	
	myCashflows.resize(product.cashflowPayTimes().size());

}

///////////////////////////////////////////////////////////////////////////////
//return PV from one spot path
double UTValuationEngineMonteCarlo::pvFromSinglePath(const vector<double> &spotValues) const
{
	// Get the payoffs from spot prices
	unsigned long numberOfFlows = myProductBase.payoffs(spotValues, myCashflows);
	double pv = 0.0;
	for (unsigned i = 0; i < numberOfFlows; ++i)
	{
		pv += myCashflows[i].second * myDf[myCashflows[i].first];
	}
	return pv;
}

///////////////////////////////////////////////////////////////////////////////
void UTValuationEngineMonteCarlo::run()
{
	// temporary spot prices from one path
	vector<double> spotPrices(myProductBase.timeLine().size());

	// Do the monte carlo simulation
	double sum = 0.0;
	for (unsigned i = 0; i < myNumberOfPaths; ++i)
	{
		getSinglePath(spotPrices);  //virtual function!!
		sum += pvFromSinglePath(spotPrices);
		
	}

	myValue = sum / myNumberOfPaths;
}

///////////////////////////////////////////////////////////////////////////////
// Accumulates the PV of the current product
void
UTValuationEngineMonteCarlo::calculatePV(double& resultPv)
{
	// Accumulate only Pv
	resultPv += myValue;

}
//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//UTValuationEngineMonteCarloBlackSholesDynamics

UTValuationEngineMonteCarloBlackSholesDynamics::UTValuationEngineMonteCarloBlackSholesDynamics(
	const UTModelBlackSholesDynamics & model,
	const UTProductBase & product,
	const UTWrapper<UTRandomBase> & numberGenerator,
	unsigned long numberOfPaths )
	: UTValuationEngineMonteCarlo(model, product, numberGenerator, numberOfPaths),
	myModel(model)
{
	//A bit of house keeping
	vector<double> times(product.timeLine());
	myNumberOfTimes = times.size();
	myDrifts.resize(myNumberOfTimes);
	myStandardDeviations.resize(myNumberOfTimes);
	myVariates.resize(myNumberOfTimes);
	generator()->resetDimensionality(myNumberOfTimes);



	//Get the log drift and log variance from the model
	double variance = myModel.logVariance(0.0, times[0]);
	myStandardDeviations[0] = sqrt(variance);
	myDrifts[0] = myModel.logDrift(0.0, times[0]);

	for (unsigned long i = 1; i < myNumberOfTimes; ++i)
	{
		variance = myModel.logVariance(times[i-1], times[i]);
		myStandardDeviations[i] = sqrt(variance);
		myDrifts[i] = myModel.logDrift(times[i - 1], times[i]);
	}

	//get the log of spot price from the model
	myLogSpot = log(myModel.forwardPrice(0.0));

	run();  // do the monter Carlo to calculate PV

}

///////////////////////////////////////////////////////////////////////////////
void UTValuationEngineMonteCarloBlackSholesDynamics::getSinglePath(vector<double> &spotValues)
{
	generator()->nextGaussianVector(myVariates);

	double logSpot = myLogSpot;

	for (unsigned i = 0; i < myNumberOfTimes; ++i)
	{
		logSpot += myDrifts[i] + myStandardDeviations[i] * myVariates[i];
		spotValues[i] = exp(logSpot);

	}

	return;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
