#ifndef UT_PayOff2_H
#define UT_PayOff2_H

class UTPayOff
{
public:

	UTPayOff(){};
	virtual double operator()(double Spot) const = 0;
	virtual ~UTPayOff(){}

private:

};


class UTPayOffCall : public UTPayOff
{
public:

	UTPayOffCall(double Strike_);
	virtual double operator()(double Spot) const;
	virtual ~UTPayOffCall(){}

private:

	double Strike;

};


class UTPayOffPut : public UTPayOff
{
public:

	UTPayOffPut(double Strike_);
	virtual double operator()(double Spot) const;
	virtual ~UTPayOffPut(){}

private:

	double Strike;

};

#endif

/*
*
* Copyright (c) 2002
* Mark Joshi
*
* Permission to use, copy, modify, distribute and sell this
* software for any purpose is hereby
* granted without fee, provided that the above copyright notice
* appear in all copies and that both that copyright notice and
* this permission notice appear in supporting documentation.
* Mark Joshi makes no representations about the
* suitability of this software for any purpose. It is provided
* "as is" without express or implied warranty.
*/