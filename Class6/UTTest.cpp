/* UTTest.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include<iostream>
#include<fstream>
#include<string>

#include "UTEnum.h"
#include "UTProductSwap.h"
#include "UTModelYieldCurve.h"
#include "UTValuationEngineFactory.h"
#include "UTModelFactory.h"
#include "UTTest.h"

using namespace std;



void yieldCurveCalibration()
{
	vector<double> swapRates{ 0.01, 0.03, 0.05 };
	vector<double> swapMaturities{ 1.0, 3.0, 5.0 };

	//Model generation
	auto yieldCurve = UTModelFactory::newModelYieldCurve(swapMaturities, swapRates);

	// Products
	UTProductSwapVanilla vanillaSwap1(0, 1.0, 0.01, 0.5, 0.5, 10000.0, UT_PayReceive::UT_RECEIVE);
	UTProductSwapVanilla vanillaSwap2(0, 3.0, 0.03, 0.5, 0.5, 10000.0, UT_PayReceive::UT_RECEIVE);
	UTProductSwapVanilla vanillaSwap3(0, 5.0, 0.05, 0.5, 0.5, 10000.0, UT_PayReceive::UT_RECEIVE);


	//Pricing Test
	auto pricer1 = UTValuationEngineFactory::newValuationEngineAnalyticYieldCurve(*yieldCurve, vanillaSwap1);
	double pv = 0.0;
	pricer1->calculatePV(pv);

	cout << "the PV of 1st swap is " << pv << ".\n";

	auto pricer2 = UTValuationEngineFactory::newValuationEngineAnalyticYieldCurve(*yieldCurve, vanillaSwap2);
	pv = 0.0;  //reset of pv
	pricer2->calculatePV(pv);

	cout << "the PV of the 2nd swap is " << pv << ".\n";

	auto pricer3 = UTValuationEngineFactory::newValuationEngineAnalyticYieldCurve(*yieldCurve, vanillaSwap3);
	pv = 0.0;
	pricer3->calculatePV(pv);

	cout << "the PV of the 3rd swap is " << pv << ".\n";


}


void pricingTest()
{
	// Products
	UTProductSwapVanilla vanillaSwap(0, 10, 0.03, 0.5,0.5, 10000.0, UT_PayReceive::UT_RECEIVE);
	UTProductLegVanilla fixedLeg(UT_FixedFloat::UT_FIXED, 0, 10, 0.03, 0.5, 10000.0, UT_PayReceive::UT_RECEIVE);
	UTProductLegVanilla floatLeg(UT_FixedFloat::UT_FLOAT, 0, 10, 0.0, 0.5, 10000.0, UT_PayReceive::UT_PAY);

	//Model
	UTModelYieldCurve yieldCurve;

	//Pricing
	auto pricerFixedLeg(UTValuationEngineFactory::newValuationEngineAnalyticYieldCurve(yieldCurve, fixedLeg));
	double pv = 0.0;
	pricerFixedLeg->calculatePV(pv);

	cout << "the PV of the fixed leg is " << pv << ".\n";

	auto pricerFloatLeg(UTValuationEngineFactory::newValuationEngineAnalyticYieldCurve(yieldCurve, floatLeg));
	pv = 0.0;  //reset of pv
	pricerFloatLeg->calculatePV(pv);

	cout << "the PV of the float leg is " << pv << ".\n";

	auto pricerSwap(UTValuationEngineFactory::newValuationEngineAnalyticYieldCurve(yieldCurve, vanillaSwap));
	pv = 0.0;
	pricerSwap->calculatePV(pv);

	cout << "the PV of the swap is " << pv <<  ".\n";

}


