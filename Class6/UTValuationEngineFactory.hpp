/* UTValuationEngineFactory.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_VALUATION_ENGINE_FACTORY_H
#define UT_VALUATION_ENGINE_FACTORY_H

#include <memory>
#include <vector>

#include "UTValuationEngineProduct.hpp"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Create the appropriate valuation engine by model, prodcut and method
class UTValuationEngineFactory
{
public:
	// Generic Valuation Engine for analytic method
	static std::unique_ptr<UTValuationEngineProductBase> newValuationEngineAnalytic(const UTModelBase& model, const UTProductBase& product, bool bThrow = false);

	// Valuation Engine for Analytic + YieldCurveModel
	static std::unique_ptr<UTValuationEngineProductBase> newValuationEngineAnalyticYieldCurve(const UTModelYieldCurve& model, const UTProductBase& product, bool bThrow = false);


};

#endif  // UT_VALUATION_ENGINE_FACTORY_H

