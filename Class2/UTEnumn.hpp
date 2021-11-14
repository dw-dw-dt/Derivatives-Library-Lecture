#ifndef UT_ENUM_H
#define UT_ENUM_H

enum UT_CallPut
{
	UT_INVALID_CALL_PUT = 0,
	UT_CALL = 1,
	UT_PUT = -1,
	UT_STRADDLE = 2
};

//Helper functions
UT_CallPut toCallPut(const std::string &strIn);
std::string toString(UT_CallPut type);

#endif // UT_ENUM_H

/*
*
* Copyright (c) 2016
* Diva Analytics
*/