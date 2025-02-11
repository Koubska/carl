#include <gtest/gtest.h>

#include <carl-arith/core/VariablePool.h>
#include <carl-arith/poly/umvpoly/UnivariatePolynomial.h>
#include <carl-arith/poly/umvpoly/functions/Chebyshev.h>
#include <carl-arith/poly/umvpoly/functions/RootBounds.h>

#include "../Common.h"

using UPoly = carl::UnivariatePolynomial<Rational>;


template<typename T>
class RootBoundTest: public testing::Test {};

TYPED_TEST_CASE(RootBoundTest, RationalTypes); // TODO should use NumberTypes

TYPED_TEST(RootBoundTest, CauchyBound) {
	carl::Variable x = carl::fresh_real_variable("x");
	carl::UnivariatePolynomial<TypeParam> p(x, {12, 4, 9, 5, 0, 3});
	TypeParam bound = carl::cauchyBound(p);
	EXPECT_EQ(bound, TypeParam(5));
}

TYPED_TEST(RootBoundTest, HirstMacey) {
	carl::Variable x = carl::fresh_real_variable("x");
	carl::UnivariatePolynomial<TypeParam> p(x, {12, 4, 9, 5, 0, 3});
	TypeParam bound = carl::hirstMaceyBound(p);
	EXPECT_EQ(bound, TypeParam(10));
}

TYPED_TEST(RootBoundTest, Lagrange) {
	carl::Variable x = carl::fresh_real_variable("x");
	carl::UnivariatePolynomial<TypeParam> p(x, {12, 4, 9, 5, 0, 3});
	TypeParam bound = carl::lagrangeBound(p);
	EXPECT_TRUE(bound <= TypeParam(6));
}

TYPED_TEST(RootBoundTest, Chebyshev) {
	carl::Variable x = carl::fresh_real_variable("x");
	carl::Chebyshev<TypeParam> chebyshev(x);
	auto p = chebyshev(50);
	
	EXPECT_EQ(TypeParam("9371571/4096"), carl::cauchyBound(p));
	EXPECT_EQ(TypeParam("6882064642385021251/562949953421312"), carl::hirstMaceyBound(p));
	EXPECT_TRUE(TypeParam("10") >= carl::lagrangeBound(p));
}
