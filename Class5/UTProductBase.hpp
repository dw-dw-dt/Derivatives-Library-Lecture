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

	// Destructor.
	virtual ~UTProductBase() {}

	// Constructors.
	UTProductBase() {};

	// Pure virtual functions.
	virtual const std::string &classTag() const = 0;
	virtual double firstTime() const = 0;
	virtual double lastTime() const = 0;

};

#endif // UT_PRODUCT_BASE_H

