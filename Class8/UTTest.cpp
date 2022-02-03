/* UTTest.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include<iostream>
#include<fstream>
#include<string>

#include "UTEuropeanOptionLogNormal.hpp"
#include "UTEuropeanOptionNormal.hpp"
#include "UTProductSwap.hpp"
#include "UTProductEuropeanOption.hpp"
#include "UTProductPathDependent.hpp"
#include "UTModelYieldCurve.hpp"
#include "UTModelBlackSholesDynamics.hpp"
#include "UTValuationEngineFactory.hpp"
#include "UTRandomParkMiller.hpp"
#include "UTRandomAntitheticVariates.hpp"
#include "UTModelFactory.hpp"

using namespace std;

void testStraddle()
{
	vector<double> impVol{ 0.1, 0.2, 0.25 };
	vector<double> optionMaturities{ 0.5, 1.0, 2.0 };
	double spotPrice = 100.0;

	//Model generation
	shared_ptr<const UTModelYieldCurve> pYieldCurve(new UTModelYieldCurve()); // flat 3% yield curve
	shared_ptr<const UTModelBlackSholesDynamics>volModel(UTModelFactory::newModelBlackSholesDynamics(spotPrice, optionMaturities, impVol, pYieldCurve));

	// Products 
	UTProductEuropeanOptionCall vanillaCall(1.0, 1.0, UT_BuySell::UT_BUY, spotPrice);  //ATM spot
	UTProductEuropeanOptionPut vanillaPut(1.0, 1.0, UT_BuySell::UT_BUY, spotPrice);  //ATM spot
	UTProductEuropeanOptionStraddle vanillaStraddle(1.0, 1.0, UT_BuySell::UT_BUY, spotPrice);  //ATM spot


	//Pricing Test

	// Call Option with analytic method
	auto pricer1(UTValuationEngineFactory::newValuationEngineAnalytic(*volModel, vanillaCall));
	double pv = 0.0;
	pricer1->calculatePV(pv);

	cout << "the PV of call option with analytic method: " << pv << ".\n";

	// Put Option with analytic method
	auto pricer2(UTValuationEngineFactory::newValuationEngineAnalytic(*volModel, vanillaPut));
	pv = 0.0;
	pricer2->calculatePV(pv);

	cout << "the PV of put option with analytic method: " << pv << ".\n";

	// Straddle Option with analytic method
	auto pricer3(UTValuationEngineFactory::newValuationEngineAnalytic(*volModel, vanillaStraddle));
	pv = 0.0;
	pricer3->calculatePV(pv);

	cout << "the PV of straddle option with analytic method: " << pv << ".\n";


	const unsigned long  numbeOfPaths = 10000;
	UTRandomParkMiller generator;
	auto pricer4(UTValuationEngineFactory::newValuationEngineMonteCarlo(*volModel, vanillaCall, generator, numbeOfPaths));
	pv = 0.0;  //reset of pv
	pricer4->calculatePV(pv);

	cout << "the PV of the call option with Monte Carlo " << pv << ".\n";

	auto pricer5(UTValuationEngineFactory::newValuationEngineMonteCarlo(*volModel, vanillaPut, generator, numbeOfPaths));
	pv = 0.0;  //reset of pv
	pricer5->calculatePV(pv);

	cout << "the PV of the put option with Monte Carlo " << pv << ".\n";

	auto pricer6(UTValuationEngineFactory::newValuationEngineMonteCarlo(*volModel, vanillaStraddle, generator, numbeOfPaths));
	pv = 0.0;  //reset of pv
	pricer6->calculatePV(pv);

	cout << "the PV of the straddle option with Monte Carlo " << pv << ".\n";

}

void exoticOptionTest()
{
	vector<double> impVol{ 0.1, 0.2, 0.25 };
	vector<double> optionMaturities{ 0.5, 1.0, 2.0 };
	double spotPrice = 100.0;

	//Model generation
	shared_ptr<const UTModelYieldCurve> pYieldCurve(new UTModelYieldCurve()); // flat 3% yield curve
	shared_ptr<const UTModelBlackSholesDynamics>volModel(UTModelFactory::newModelBlackSholesDynamics(spotPrice, optionMaturities, impVol, pYieldCurve));

	// Products 
	UTProductEuropeanOptionCall vanillaCall(1.0, 1.0, UT_BuySell::UT_BUY, spotPrice);  //ATM spot
	UTProductPathDependentAsian arithmeticOption(0.0, 1.0, 12, 1.0, UT_CallPut::UT_CALL, UT_BuySell::UT_BUY, spotPrice, UT_AverageType::UT_ARITHMETIC);
	UTProductPathDependentAsian geometricOption(0.0, 1.0, 12, 1.0, UT_CallPut::UT_CALL, UT_BuySell::UT_BUY, spotPrice, UT_AverageType::UT_GEOMETRIC);

	//Pricing Test
	// Call Option with analytic method
	auto pricer1(UTValuationEngineFactory::newValuationEngineAnalytic(*volModel, vanillaCall));
	double pv = 0.0;
	pricer1->calculatePV(pv);

	cout << "the PV of call option with analytic method: " << pv << ".\n";

	const unsigned long  numbeOfPaths = 10000;
	UTRandomParkMiller generator;

	auto pricer3(UTValuationEngineFactory::newValuationEngineMonteCarlo(*volModel, arithmeticOption, generator, numbeOfPaths));
	pv = 0.0;  //reset of pv
	pricer3->calculatePV(pv);

	cout << "the PV of the arithmetic Asian call option is " << pv << ".\n";


	auto pricer4(UTValuationEngineFactory::newValuationEngineMonteCarlo(*volModel, geometricOption, generator, numbeOfPaths));
	pv = 0.0;  //reset of pv
	pricer4->calculatePV(pv);

	cout << "the PV of the Asian geometic option by MC is " << pv << ".\n";

	auto pricer5(UTValuationEngineFactory::newValuationEngineAnalytic(*volModel, geometricOption));
	pv = 0.0;  //reset of pv
	pricer5->calculatePV(pv);

	cout << "the PV of the Asian geometic option by closed form is " << pv << ".\n";

}

void testConstWrapper()
{
	UTWrapper<UTModelBlackSholesDynamics> aaa(new UTModelBlackSholesDynamics);
	const UTWrapper<UTModelBlackSholesDynamics> ccc(new UTModelBlackSholesDynamics);

	UTModelYieldCurve bbb;
	aaa->setModelYieldCurve(bbb);

}

void randomNumberTest()
{
	const unsigned long  numberOfDim = 2;
	const unsigned long  numbeOfPaths = 100;

	vector<double> variates(numberOfDim);
	UTRandomParkMiller generator(numberOfDim);

	ofstream ofs("test.txt");


	for (unsigned long i = 0; i < numbeOfPaths; ++i)
	{
		generator.nextUniformVector(variates);
		for (unsigned long j = 0; j < numberOfDim; ++j)
			ofs << variates[j] << " ";
		ofs << "\n";

	}
	ofs << "\n";

	UTRandomAntitheticVariates generator2(generator);

	for (unsigned long i = 0; i < numbeOfPaths; ++i)
	{
		generator2.nextUniformVector(variates);
		for (unsigned long j = 0; j < numberOfDim; ++j)
			ofs << variates[j] << " ";
		ofs << "\n";

	}

}
void volModelCalibration()
{
	vector<double> impVol{ 0.1, 0.2, 0.25 };
	vector<double> optionMaturities{ 0.5, 1.0, 2.0 };
	double spotPrice = 100.0;

	//Model generation
	shared_ptr<const UTModelYieldCurve> pYieldCurve(new UTModelYieldCurve()); // flat 3% yield curve
	shared_ptr<const UTModelBlackSholesDynamics>volModel(UTModelFactory::newModelBlackSholesDynamics(spotPrice, optionMaturities, impVol, pYieldCurve));

	// Products
	double vol1 = volModel->vol(0.5);
	double vol2 = volModel->vol(1.0);
	double vol3 = volModel->vol(2.0);

	cout << "1st vol " << vol1 << ".\n";
	cout << "2nd vol " << vol2 << ".\n";
	cout << "3rd vol " << vol3 << ".\n";

}

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
	auto spFixedLeg = make_shared<UTProductLegVanilla>(UT_FixedFloat::UT_FIXED, 0.0, 10.0, 0.03, 0.5, 10000.0, UT_PayReceive::UT_RECEIVE);
	auto spFloatLeg = make_shared<UTProductLegVanilla>(UT_FixedFloat::UT_FLOAT, 0.0, 10.0, 0.0, 0.5, 10000.0, UT_PayReceive::UT_PAY);
	UTProductSwapVanilla vanillaSwap(spFixedLeg, spFloatLeg);

	//Model
	UTModelYieldCurve yieldCurve;

	//Pricing
	auto pricerFixedLeg(UTValuationEngineFactory::newValuationEngineAnalyticYieldCurve(yieldCurve, *spFixedLeg));
	double pv = 0.0;
	pricerFixedLeg->calculatePV(pv);

	cout << "the PV of the fixed leg is " << pv << ".\n";

	auto pricerFloatLeg(UTValuationEngineFactory::newValuationEngineAnalyticYieldCurve(yieldCurve, *spFloatLeg));
	pv = 0.0;  //reset of pv
	pricerFloatLeg->calculatePV(pv);

	cout << "the PV of the float leg is " << pv << ".\n";

	auto pricerSwap(UTValuationEngineFactory::newValuationEngineAnalyticYieldCurve(yieldCurve, vanillaSwap));
	pv = 0.0;
	pricerSwap->calculatePV(pv);

	cout << "the PV of the swap is " << pv <<  ".\n";

}


