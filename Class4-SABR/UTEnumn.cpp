/* UTEnumn.cpp
*
* Copyright (c) 2016
* Diva Analytics
*/

#include <string>
#include <iostream>
#include "UTEnumn.hpp"

using namespace std;


// Helper Functions

UT_CallPut
toCallPut(const string &strIn) // By value rather than by reference since we need a copy anyway.
{

	switch (strIn.c_str()[0]){
	case 'C':
	case 'c':
		return UT_CALL;
		break;
	case 'P':
	case 'p':
		return UT_PUT;
		break;
	case 'S':
	case 's':
		return UT_STRADDLE;
		break;
	default:
		cerr << "toCallPut(): Unknown call/put type: " + strIn << endl;

		//throw("Unknown option type: " + strIn);
	}

	// To quell compiler complaints.
	return UT_INVALID_CALL_PUT;
}

///////////////////////////////////////////////////////////////////////////////

string
toString(UT_CallPut type)
{
	string str;

	switch (type)
	{
	case UT_CALL:
	{
		str = "Call";
		break;
	}
	case UT_PUT:
	{
		str = "Put";
		break;
	}
	case UT_STRADDLE:
	{
		str = "Straddle";
		break;
	}
	case UT_INVALID_CALL_PUT:
	default:
	{
		str = "Unknown option type.";
		break;
	}
	}

	return str;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////