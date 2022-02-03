/* UTModelBase.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef UT_MODEL_BASE_H
#define UT_MODEL_BASE_H

#include <string>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// UTModelBase 
//
// The base class for all models.
//
class UTModelBase
{
public:

	// Make UTSolveForModelComponent a friend so that model can be calibrated.
	friend class UTSolveForModelComponent;
	friend class UTSolveForModelComponent2;

	static std::string const ourClassTag;

	// Destructor.
	virtual ~UTModelBase() {}

	// Constructors.
	UTModelBase() {}

	// Clone.
	virtual UTModelBase* clone() const = 0;

	// Functions.
	virtual  std::string classTag() const = 0;
	virtual double df(double time) const = 0;
	virtual double forwardRate(double startTime, double endTime) const = 0;


	// Return a reference to the ith sub-model
	virtual const UTModelBase* subModel(unsigned long i) const;

private:
	// solver in the calibration only use this.
	virtual void setComponent(unsigned int i, double component) = 0;

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif // UT_MODEL_BASE_H
