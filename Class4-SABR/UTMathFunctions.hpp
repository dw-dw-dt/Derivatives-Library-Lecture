/* UTMathFunctions.h
*
* Copyright(c) 2016
* Diva Analytics
*/

#ifndef UT_MATH_FUNCTIONS_H
#define UT_MATH_FUNCTIONS_H

#include <cmath>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Undefine max and min

#ifdef max
# undef max
#endif
#ifdef min
# undef min
#endif


class UTMathFunctions
{
public:
	// Destructor.
	~UTMathFunctions() {}

	// Constructor.
	UTMathFunctions() {}

	////
	// Maths functions.

	//
	// The inlined square of a number
	static double square(double a){ return a*a; };

	//
	// The inlined cube of a number
	static double cube(double a){ return a*a*a; };

	// The sign of a variable x is -1 if x<0, +1 if x>0, and 0 if x==0.
	static double sign(double x);

	// The larger of two.
	static double        max(double x, double y)               { return x > y ? x : y; };
	static int           max(int  x, int y)                 { return x > y ? x : y; };
	static long          max(long x, long y)                 { return x > y ? x : y; };
	static unsigned long max(unsigned long x, unsigned long y) { return x > y ? x : y; };

	// The smaller of two.
	static double min(double x, double y){ return x < y ? x : y; };
	static long   min(long x, long y)  { return x < y ? x : y; };
	static int    min(int x, int y)   { return x < y ? x : y; };

	static unsigned long pow(unsigned long base, unsigned long exponent){ unsigned long result = 1; while (exponent--) result *= base; return result; }


	static double normalDensity(double x);

	static double cumulativeNormal(double x);

	static double inverseCumulativeNormal(double x);

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// inline functions

inline double
square(double a){
	return a*a;
}

inline double
cube(double a){
	return a*a*a;
}

inline double
UTMathFunctions::sign(double x)
{
	if (x == 0.) return 0.;
	if (x<0.) return -1;
	return 1.;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif // UT_MATH_FUNCTIONS_H
