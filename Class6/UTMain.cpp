/* UTMain.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include<iostream>
#include "UTTest.hpp"
#include "UTProductSwap.hpp"
#include "UTModelYieldCurve.hpp"


using namespace std;


int main()
{

	try
	{
		//Put your test routine
		yieldCurveCalibration();
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
