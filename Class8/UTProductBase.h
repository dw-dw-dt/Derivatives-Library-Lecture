/* UTProductBase.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef  UT_PRODUCT_BASE_H
#define  UT_PRODUCT_BASE_H

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//  Abstract base class for the entire product hierarchy
class  UTProductBase
{
public:

	//define cashflow as alias
	using UTCashflows_t = std::pair<unsigned long, double>;

	// Destructor.
	virtual ~UTProductBase() {}

	// Constructors.
	UTProductBase() {};

	// Pure virtual functions.
	virtual std::string classTag() const = 0;
	virtual double firstTime() const = 0;
	virtual double lastTime() const = 0;

	//Virtual functions.
	virtual const std::vector<double>& timeLine() const;
	virtual const std::vector<double> cashflowPayTimes() const;
	virtual unsigned long payoffs(const std::vector<double> spotPrices, std::vector<UTCashflows_t> &cashflows) const;

};

#endif // UT_PRODUCT_BASE_H

