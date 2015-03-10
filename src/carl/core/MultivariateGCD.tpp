/** 
 * @file:   MultivariateGCD.tpp
 * @author: Sebastian Junges
 *
 * @since January 19, 2014
 */

#pragma once

#include "MultivariateGCD.h"

#include "UnivariatePolynomial.h"

#include "MultivariatePolynomial.h"
#include "VariablesInformation.h"

namespace carl
{

template<typename GCDCalculation, typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> MultivariateGCD<GCDCalculation, C, O, P>::calculate() 
{
	assert(!mp1.isZero());
	assert(!mp2.isZero());
	// We start with some trivial cases.
	if(mp1.isOne() || mp2.isOne())
    {
        return Polynomial(1);
    }
	if(is_field<C>::value && mp1.isConstant())
	{
		return Polynomial(carl::gcd( mp1.constantPart(), carl::constant_one<C>().get()/mp2.coprimeFactor() ));
	}
    if(is_field<C>::value && mp2.isConstant())
    {
		return Polynomial(carl::gcd( mp2.constantPart(), carl::constant_one<C>().get()/mp1.coprimeFactor() ));
    }
	if(mp1.nrTerms() == 1 && mp2.nrTerms() == 1)
	{
		return Polynomial(Term<C>::gcd(mp1.lterm(), mp2.lterm()));
	}

	// And we do some simplifications for the input.
	// In order to do so, we gather some information about the polynomials, as we most certainly need them later on.
	
	// We check for mutual trivial factorizations.

	// And we check for linearly appearing variables. Notice that ay + b is irreducible and thus,
	// gcd(p, ay + b) is either ay + b or 1.
    
    #ifdef COMPARE_WITH_GINAC
    typedef MultivariatePolynomial<C,O,P> PolyT;
    return ginacGcd<PolyT>( mp1, mp2 );
    #else 
	Variable x = getMainVar(mp1, mp2);
	if(x == Variable::NO_VARIABLE)
	{
		return Polynomial(1);
	}
	UnivReprPol A = mp1.toUnivariatePolynomial(x);
	UnivReprPol B = mp2.toUnivariatePolynomial(x);
	UnivReprPol GCD = (*static_cast<GCDCalculation*>(this))(A.normalized(),B.normalized());
	
	return Polynomial(GCD);
	//return Result()
//		return Result(GCD, A/GCD, B/GCD);
    #endif

}	

template<typename C, typename O, typename P>
Term<C> gcd(const MultivariatePolynomial<C,O,P>& a, const Term<C>& b)
{
	static_assert(is_field<C>::value, "Only implemented for field coefficients");
	assert(!a.isZero());
	assert(!b.isZero());
	if(b.isConstant()) return Term<C>(C(1));
	return Term<C>(C(1), gcd(a,*(b.monomial())));
}


template<typename C, typename O, typename P>
std::shared_ptr<const Monomial> gcd(const MultivariatePolynomial<C,O,P>& a, std::shared_ptr<const Monomial> b)
{
    if( !b )
        return nullptr;
	assert(!a.isZero());
	VariablesInformation<false, MultivariatePolynomial<C,O,P>> varinfo = a.getVarInfo();
	std::vector<std::pair<Variable, exponent>> vepairs;
	for(const auto& ve : *b)
	{
		if(varinfo.getVarInfo(ve.first)->occurence() == a.nrTerms())
		{
			vepairs.push_back(ve.first, std::min(varinfo.getVarInfo(ve.first)->minDegree(), ve.second));
		}
	}
	return createMonomial( std::move(vepairs) );
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> gcd(const MultivariatePolynomial<C,O,P>& a, const MultivariatePolynomial<C,O,P>& b)
{
	MultivariateGCD<PrimitiveEuclidean, C, O, P> gcd_calc(a,b);
    #ifdef COMPARE_WITH_GINAC
    assert( gcd_calc.checkCorrectnessWithGinac() );
    #endif 
	return gcd_calc.calculate();
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> lcm(const MultivariatePolynomial<C,O,P>& a, const MultivariatePolynomial<C,O,P>& b)
{
	assert(!a.isZero());
	assert(!b.isZero());
	if(a == b) return a;
	//if(is_field<C>::value && a.isConstant() && b.isConstant()) return MultivariatePolynomial<C,O,P>(1);
	//if(is_field<C>::value && a.isConstant()) return b;
	//if(is_field<C>::value && b.isConstant()) return a;
	
	
	return quotient((a*b),gcd(a,b));
}

}