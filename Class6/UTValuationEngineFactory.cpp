/* UTValuationEngineFactory.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTValuationEngineFactory.hpp"
#include "UTValuationEngineProduct.hpp"
#include "UTProductCashflow.hpp"
#include "UTProductSwap.hpp"
#include "UTModelYieldCurve.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
unique_ptr<UTValuationEngineProductBase> UTValuationEngineFactory::newValuationEngineAnalytic(const UTModelBase& model, const UTProductBase& product, bool bThrow)
{
	if (model.classTag() == "Yield Curve Model")
	{
		return newValuationEngineAnalyticYieldCurve(dynamic_cast<const UTModelYieldCurve&>(model), product, bThrow);
	}
	else
	{
		if (bThrow)
		{
			throw runtime_error("UTValuationEngineRegistry::The input model cannnot value the product analytically.");
		}

	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
unique_ptr<UTValuationEngineProductBase> UTValuationEngineFactory::newValuationEngineAnalyticYieldCurve(const UTModelYieldCurve& model, const UTProductBase& product, bool bThrow)
{
	unique_ptr<UTValuationEngineProductBase> pValuationEngine(nullptr);

	if (product.classTag() == "Vanilla Leg")
	{
		pValuationEngine.reset(new UTValuationEngineAnalyticProductLinear(model, dynamic_cast<const UTProductLinearBase&>(product)));
	}
	else if (product.classTag() == "Vanilla Swap")
	{
		pValuationEngine.reset(new UTValuationEngineAnalyticProductLinear(model, dynamic_cast<const UTProductLinearBase&>(product)));
	}
	else if (product.classTag() == "Product Cashflow Bullet")
	{
		pValuationEngine.reset(new UTValuationEngineAnalyticYieldCurveProductCashflowBullet(model, dynamic_cast<const UTProductCashflowBullet&>(product)));
	}
	else if (product.classTag() == "Product Cashflow Rate Fixed")
	{
		pValuationEngine.reset(new UTValuationEngineAnalyticYieldCurveProductCashflowRateFixed(model, dynamic_cast<const UTProductCashflowRateFixed&> (product)));
	}
	else if (product.classTag() == "Product Cashflow Rate Float")
	{
		pValuationEngine.reset(new UTValuationEngineAnalyticYieldCurveProductCashflowRateFloat(model, dynamic_cast<const UTProductCashflowRateFloat&> (product)));
	}
	else
	{
		if (bThrow)
		{
			throw runtime_error("UTValuationEngineFactory::The input product cannnot be valued by Yield curve model analytically.");

		}

	}

	return pValuationEngine;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

