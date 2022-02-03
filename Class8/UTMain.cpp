/* UTMain.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include<iostream>
#include "UTTest.h"
#include "UTProductSwap.h"
#include "UTModelYieldCurve.h"


using namespace std;


int main()
{

	try
	{
		//Put your test routine
		exoticOptionTest();
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
