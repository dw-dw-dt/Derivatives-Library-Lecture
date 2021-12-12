/* UTMainProducts.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include<iostream>
#include<string>

#include "UTEnum.hpp"
#include "UTProductBase.hpp"
#include "UTProductSwap.hpp"

using namespace std;


int main()
{

	try
	{
		// Products
		UTProductSwapVanilla vanillaSwap1(0, 1.0, 0.01, 0.5, 0.5, 10000.0, UT_PayReceive::UT_RECEIVE);
		UTProductSwapVanilla vanillaSwap2(0, 3.0, 0.03, 0.5, 0.5, 10000.0, UT_PayReceive::UT_RECEIVE);
		UTProductSwapVanilla vanillaSwap3(0, 5.0, 0.05, 0.5, 0.5, 10000.0, UT_PayReceive::UT_RECEIVE);

		UTProductSwapVanilla vanillaSwap4 = vanillaSwap1;
		vanillaSwap3 = vanillaSwap4;

		auto spFixedLeg = make_shared<UTProductLegVanilla>(UT_FixedFloat::UT_FIXED, 1.0, 6.0, -1.0, 0.5, 10000.0, UT_PayReceive::UT_RECEIVE);
		auto spFloatLeg = make_shared<UTProductLegVanilla>(UT_FixedFloat::UT_FLOAT, 1.0, 6.0, 0.0, 0.5, 10000.0, UT_PayReceive::UT_PAY);


		UTProductSwapVanilla vanillaSwap5(spFixedLeg, spFloatLeg);

		double startTime = vanillaSwap5.firstTime();
		double endTtime = vanillaSwap5.lastTime();

		cout << "The swap starts at " << startTime << " and ends at " << endTtime << "." << endl;
	

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
