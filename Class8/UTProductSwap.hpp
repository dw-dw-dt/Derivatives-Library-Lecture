/* UTProductSwap.h
*
* Copyright (c) 2016
* Diva Analytics
*/

#ifndef  UT_PRODUCT_SWAP_H
#define  UT_PRODUCT_SWAP_H

#include <memory>
#include <vector>

#include "UTProductCashflow.hpp"

enum class UT_FixedFloat;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// UTProductLinearBase
//
// Abstract base class for linear products.
//
class UTProductLinearBase : public virtual UTProductBase
{
public:

	// Destructor.
	virtual ~UTProductLinearBase() {}

	// Constructors.
	UTProductLinearBase();
	UTProductLinearBase(const std::vector<std::shared_ptr<const UTProductBase> >& underlyings);

	// Interface to access an (or the list) underlying product (and the number of underlying products)
	virtual const std::vector<std::shared_ptr<const UTProductBase> >& underlyings() const { return myUnderlyings; }
	virtual const std::shared_ptr<const UTProductBase>& underlying(unsigned long lProduct) const { return myUnderlyings[lProduct]; }
	virtual unsigned long size() const { return myUnderlyings.size(); }

	// Functions.
	virtual std::string classTag() const = 0;
	virtual double firstTime() const;
	virtual double lastTime() const;


	// Helper function so that product vectors passed to constuctors of concrete classes can be passed straight through to UTProductLinearBase constructor
	template < typename T>
	static std::vector< std::shared_ptr< const UTProductBase > > convertProductVectorToBase(const std::vector< std::shared_ptr< const T > >& products)
	{
		unsigned long i;
		const unsigned long n = products.size();
		std::vector < std::shared_ptr<const UTProductBase> > convertedProducts;
		for (i = 0; i < n; ++i)
			convertedProducts.push_back(std::shared_ptr< const UTProductBase >(products[i]));
		return convertedProducts;
	}

protected:
	void setLinearProductUnderlyings(const std::vector< std::shared_ptr< const UTProductBase > >& underlyings);
	void setLinearProductUnderlying(std::shared_ptr< const UTProductBase >& underlying);

	// To use with extreme caution
	void clearProductUnderlyings() { myUnderlyings.clear(); }

private:

	std::vector<std::shared_ptr<const UTProductBase> > myUnderlyings;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// UTProductLegVanilla
// 
//Class representing a set of cashflows as a leg.

class UTProductLegVanilla : public UTProductLinearBase
{
public:
	static const std::string ourClassTag;

	virtual ~UTProductLegVanilla() {};

	// Constructors
	UTProductLegVanilla() : UTProductLinearBase() {}

	UTProductLegVanilla(
		const std::vector<std::shared_ptr<const UTProductCashflowBase> >& cashflows);

	UTProductLegVanilla(
		const UT_FixedFloat&					  fixedFloat,							
		double                                    adjStartTime,
		double       			                  adjEndTime,
		double									  couponOrSpread,
		double									  period,
		double                                    notional,
		const UT_PayReceive&					  payReceive);

	// Functions.
	virtual std::string classTag() const { return ourClassTag; }

	// Operators
	const std::shared_ptr<const UTProductCashflowBase>& operator[] (unsigned long index) const;

	// Accessors.
	const std::vector<std::shared_ptr<const UTProductCashflowBase> >& cashflows() const { return myCashflows; }

private:

	// The underlying leg cashflows
	std::vector<std::shared_ptr<const UTProductCashflowBase> > myCashflows;
};

///////////////////////////////////////////////////////////////////////////////
// UTProductSwapVanilla
// 
// Class representing a vanilla swap.

class UTProductSwapVanilla : public UTProductLinearBase
{
public:
	static const std::string ourClassTag;

	virtual ~UTProductSwapVanilla() {};

	// Default Constructor
	UTProductSwapVanilla() : UTProductLinearBase() {}

	// Constructor when complete cashflow sets for both sides are given.
	UTProductSwapVanilla(
		const std::shared_ptr<const UTProductLegVanilla>& fixedLeg,
		const std::shared_ptr<const UTProductLegVanilla>& floatLeg);

	//Constructor when the parameters of the swap are given.
	UTProductSwapVanilla(
		double                                    adjStartTime,
		double       			                  adjEndTime,
		double									  coupon,
		double									  fixedPeriod,
		double									  floatPeriod,
		double                                    notional,
		const UT_PayReceive&					  payReceiveFixed);

	// Functions.
	virtual std::string classTag() const { return ourClassTag; }

	// Accessors.
	const std::shared_ptr<const UTProductLegVanilla> & fixedLeg() const { return myFixedLeg; }
	const std::shared_ptr<const UTProductLegVanilla> & floatLeg() const { return myFloatLeg; }

private:
	// The underlying swap legs
	std::shared_ptr<const UTProductLegVanilla>  myFixedLeg;
	std::shared_ptr<const UTProductLegVanilla>  myFloatLeg;

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#endif // UT_PRODUCT_SWAP_H