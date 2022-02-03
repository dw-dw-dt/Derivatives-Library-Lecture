/* UTValuationEngineMonteCarlo.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_VALUATION_ENGINE_MONTE_CARLO_H
#define UT_VALUATION_ENGINE_MONTE_CARLO_H

#include <memory>
#include <vector>
#include "UTWrapper.hpp"
#include "UTValuationEngine.hpp"

//forward declarations
class UTProductBase;
class UTRandomBase;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UTValuationEngineMonteCarlo : public UTValuationEngineBase
{
public:

	//define cashflow as alias
	using UTCashflows_t = std::pair<unsigned long, double>;

	// Destructor.
	virtual ~UTValuationEngineMonteCarlo() {};

	// Constructor.
	UTValuationEngineMonteCarlo(
		const UTModelBase & model,
		const UTProductBase & product,
		const UTWrapper<UTRandomBase> & generator,
		unsigned long myNumberOfPaths );


	// Calculates the PV of the Product and accumulate it in the ResultPV object.
	virtual void calculatePV(double& result);

	// Calculate spot price path of single 
	virtual void getSinglePath(std::vector<double> &spotValues) = 0;

	void run();

protected:

	// Accessors from derived classes
	const UTProductBase & product() const { return myProductBase; }
	const UTWrapper<UTRandomBase> & generator() const { return myGenerator; }

private:

	double pvFromSinglePath(const std::vector<double> &spotValues) const;

	const UTProductBase & myProductBase;
	UTWrapper<UTRandomBase>  myGenerator;
	unsigned long myNumberOfPaths;
	std::vector<double>myTimeLine;
	std::vector<double>myDf;

	mutable std::vector<UTCashflows_t>myCashflows;  //workspace

	// Calculated PV.
	double                                          myValue;

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UTValuationEngineMonteCarloBlackSholesDynamics : public UTValuationEngineMonteCarlo
{
public:

	//define cashflow
	typedef std::pair<unsigned long, double> UTCashflows_t;

	// Destructor.
	virtual ~UTValuationEngineMonteCarloBlackSholesDynamics() {};

	// Constructor.
	UTValuationEngineMonteCarloBlackSholesDynamics(
		const UTModelBlackSholesDynamics & model,
		const UTProductBase & product,
		const UTWrapper<UTRandomBase> & generator,
		unsigned long myNumberOfPaths);

	// Calculate spot price path of single 
	virtual void getSinglePath(std::vector<double> &spotValues);

protected:


private:

	const UTModelBlackSholesDynamics & myModel;

	std::vector<double> myDrifts;
	std::vector<double> myStandardDeviations;
	double myLogSpot;
	unsigned long myNumberOfTimes;
	std::vector<double> myVariates;

};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif // UT_VALUATION_ENGINE_PRODUCT_H