#include <cmath>

double callPayoff(double spot, double strike)
{
	double rtn = spot - strike;
	return rtn >0 ? rtn : 0;
}

double putPayoff(double spot, double strike)
{
	double rtn = strike - spot;
	return rtn >0 ? rtn : 0;
}

double straddlePayoff(double spot, double strike)
{
	return  fabs(spot - strike);
}

/*
*
* Copyright (c) 2016
* Diva Analytics
*/