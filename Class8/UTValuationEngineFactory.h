/* UTValuationEngineFactory.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_VALUATION_ENGINE_FACTORY_H
#define UT_VALUATION_ENGINE_FACTORY_H

#include <memory>
#include <vector>

#include "UTValuationEngine.h"
#include "UTWrapper.h"

class UTValuationEngineBase;
//class UTRandomBase;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Create the appropriate valuation engine by model, prodcut and method
class UTValuationEngineFactory
{
public:
	// Generic Valuation Engine for analytic method
	static std::unique_ptr<UTValuationEngineBase> newValuationEngineAnalytic(const UTModelBase& model, const UTProductBase& product, bool bThrow = false);

	// Valuation Engine for Analytic + YieldCurveModel
	static std::unique_ptr<UTValuationEngineBase> newValuationEngineAnalyticYieldCurve(const UTModelYieldCurve& model, const UTProductBase& product, bool bThrow = false);

	// Analytic + BlackSholes Dynamics Model
	static std::unique_ptr<UTValuationEngineBase> newValuationEngineAnalyticBlackSholesDynamics(const UTModelBlackSholesDynamics& model, const UTProductBase& product, bool bThrow = false);

	// Generic Valuation Engine for Monte Carlo method
	static std::unique_ptr<UTValuationEngineBase> newValuationEngineMonteCarlo(const UTModelBase& model, const UTProductBase& product, const UTWrapper<UTRandomBase> & generator, unsigned long numberOfPaths, bool bThrow = false);

};

#endif  // UT_VALUATION_ENGINE_FACTORY_H

