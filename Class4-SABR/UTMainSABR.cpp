/* UTMainSABR.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include<iostream>
#include<string>

#include "UTEnumn.hpp"
#include "UTEuropeanOptionLogNormal.hpp"
#include "UTEuropeanOptionNormal.hpp"

using namespace std;


int main()
{

	try
	{
		double dTimeToExpiry;
		double dStrike;
		double dForward;
		double dVol;
		double dPremium;
		string sCallPut;

		cout << "\nEnter expiry: ";
		cin >> dTimeToExpiry;

		cout << "\nEnter strike: ";
		cin >> dStrike;

		cout << "\nEnter spot: ";
		cin >> dForward;

		cout << "\nEnter vol: ";
		cin >> dVol;

		cout << "\nEnter callPut: ";
		cin >> sCallPut;

		UT_CallPut callPut = toCallPut(sCallPut);

		UTEuropeanOptionLogNormal option(dForward,
			dStrike,
			dTimeToExpiry,
			dVol);

		double optionPremium = option.premium(callPut);
		double optionDelta = option.delta(callPut);


		cout << "the price is " << optionPremium << " and delta is "
			<< optionDelta << ".\n";

		cout << "\nEnter premium: ";
		cin >> dPremium;

		UTSolveForImpliedVolatility solver(option, callPut, dPremium);
		double impVol = solver.root(dVol);

		option.overwriteSigma(impVol);
		double optionPremium2 = option.premium(callPut);

		cout << "imp vol is " << impVol << " and price is "
			<< optionPremium2 << ".\n";

		double dNormalVol = UTEuropeanOption::logNormalToNormal(dForward, dStrike, dTimeToExpiry, impVol);

		cout << "Equivalent normal vol is " << dNormalVol << "\n";

		UTEuropeanOptionNormal normalOption(dForward,
			dStrike,
			dTimeToExpiry,
			dNormalVol);

		double normalOptionPremium = normalOption.premium(callPut);
		double normalOptionDelta = normalOption.delta(callPut);

		cout << "the price is " << normalOptionPremium << " and delta is "
			<< normalOptionDelta << ".\n";

		cout << "\nEnter premium: ";
		cin >> dPremium;

		UTSolveForImpliedVolatility solver2(normalOption, callPut, dPremium);
		double normalImpVol = solver2.root(dNormalVol);

		normalOption.overwriteSigma(normalImpVol);
		double optionPremium3 = normalOption.premium(callPut);

		cout << "imp vol is " << normalImpVol << " and price is "
			<< optionPremium3 << ".";
	}
	catch (runtime_error err)
	{
		cout << err.what();
	}

	// The final input to stop the routine
	double tmp;
	cin >> tmp;

	return 0;

}
