/* UTValuationEngineFactory.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include "UTValuationEngineFactory.h"
#include "UTValuationEngine.h"
#include "UTValuationEngineMonteCarlo.h"
#include "UTProductCashflow.h"
#include "UTProductSwap.h"
#include "UTProductEuropeanOption.h"
#include "UTProductPathDependent.h"
#include "UTRandomBase.h"
#include "UTModelYieldCurve.h"
#include "UTModelBlackSholesDynamics.h"
#include <stdexcept>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
unique_ptr<UTValuationEngineBase> UTValuationEngineFactory::newValuationEngineAnalytic(const UTModelBase& model, const UTProductBase& product, bool bThrow)
{
	if (model.classTag() == "Yield Curve Model")
	{
		return newValuationEngineAnalyticYieldCurve(dynamic_cast<const UTModelYieldCurve&>(model), product, bThrow);
	}
	else if (model.classTag() == "Black Sholes Dynamics Model")
	{
		return newValuationEngineAnalyticBlackSholesDynamics(dynamic_cast<const UTModelBlackSholesDynamics&>(model), product, bThrow);
	}
	else
	{
		if (bThrow)
		{
			throw runtime_error("UTValuationEngineFactory::The input model cannnot value the product analytically.");
		}

	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
unique_ptr<UTValuationEngineBase> UTValuationEngineFactory::newValuationEngineMonteCarlo(const UTModelBase& model, const UTProductBase& product, const UTWrapper<UTRandomBase> & generator, unsigned long numberOfPaths, bool bThrow)
{
	unique_ptr<UTValuationEngineBase> pValuationEngine(nullptr);

	if (model.classTag() == "Black Sholes Dynamics Model")
	{
		pValuationEngine.reset(new UTValuationEngineMonteCarloBlackSholesDynamics(dynamic_cast<const UTModelBlackSholesDynamics&>(model), product, generator, numberOfPaths));
	}
	else
	{
		if (bThrow)
		{
			throw runtime_error("UTValuationEngineFactory::The input model cannnot value the product by Monte Carlo.");

		}

	}

	return pValuationEngine;
}


///////////////////////////////////////////////////////////////////////////////
unique_ptr<UTValuationEngineBase> UTValuationEngineFactory::newValuationEngineAnalyticYieldCurve(const UTModelYieldCurve& model, const UTProductBase& product, bool bThrow)
{
	unique_ptr<UTValuationEngineBase> pValuationEngine(nullptr);

	if (product.classTag() == "Vanilla Leg")
	{
		pValuationEngine.reset(new UTValuationEngineAnalyticLinearBase(model, dynamic_cast<const UTProductLinearBase&>(product)));
	}
	else if (product.classTag() == "Vanilla Swap")
	{
		pValuationEngine.reset(new UTValuationEngineAnalyticLinearBase(model, dynamic_cast<const UTProductLinearBase&>(product)));
	}
	else if (product.classTag() == "Product Cashflow Bullet")
	{
		pValuationEngine.reset(new UTValuationEngineAnalyticYieldCurveCashflowBullet(model, dynamic_cast<const UTProductCashflowBullet&>(product)));
	}
	else if (product.classTag() == "Product Cashflow Rate Fixed")
	{
		pValuationEngine.reset(new UTValuationEngineAnalyticYieldCurveCashflowRateFixed(model, dynamic_cast<const UTProductCashflowRateFixed&> (product)));
	}
	else if (product.classTag() == "Product Cashflow Rate Float")
	{
		pValuationEngine.reset(new UTValuationEngineAnalyticYieldCurveCashflowRateFloat(model, dynamic_cast<const UTProductCashflowRateFloat&> (product)));
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

//////////////////////////////////////////////////////////////////////////
unique_ptr<UTValuationEngineBase> UTValuationEngineFactory::newValuationEngineAnalyticBlackSholesDynamics(const UTModelBlackSholesDynamics& model, const UTProductBase& product, bool bThrow)
{
	unique_ptr<UTValuationEngineBase> pValuationEngine(nullptr);

	if (product.classTag() == "European Call Option")
	{
		pValuationEngine.reset(new UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionCall(model, dynamic_cast<const UTProductEuropeanOptionCall&>(product)));
	}
	else if (product.classTag() == "European Put Option")
	{
		pValuationEngine.reset(new UTValuationEngineAnalyticBlackSholesDynamicsEuropeanOptionPut(model, dynamic_cast<const UTProductEuropeanOptionPut&>(product)));
	}
	else if (product.classTag() == "European Straddle Option")
	{
		pValuationEngine.reset(new UTValuationEngineAnalyticLinearBase(model, dynamic_cast<const UTProductEuropeanOptionStraddle&>(product)));
	}
	else if (product.classTag() == "Asian Option")
	{
		pValuationEngine.reset( new UTValuationEngineAnalyticBlackSholesDynamicsPathDependentAsianGeometric(model, dynamic_cast<const UTProductPathDependentAsian&>(product)));
	}
	else
	{
		if (bThrow)
		{
			throw runtime_error("UTValuationEngineFactory::The input product cannnot be valued analytically.");

		}

	}

	return pValuationEngine;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
