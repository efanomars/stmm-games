/*
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   testSegmentedFunction.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "gtkutil/segmentedfunction.h"

namespace stmg
{

namespace testing
{

using SF = SegmentedFunction;

constexpr double g_fErrMargin = 0.00001;

TEST_CASE("testSegmentedFunction, EmptyConstructor")
{
	SF oSF{};
	REQUIRE(oSF.eval(0.0) == 0.0);
	REQUIRE(oSF.eval(10000.0) == 0.0);
}

TEST_CASE("testSegmentedFunction, Constructor")
{
	{
	SF oSF{1.0, 0.0};
	REQUIRE(oSF.eval(0.0) == 0.0);
	REQUIRE(Approx(oSF.eval(10000.0)).epsilon(g_fErrMargin) == 10000.0);
	}
	{
	SF oSF{0.0, 1.0};
	REQUIRE(oSF.eval(0.0) == 1.0);
	REQUIRE(oSF.eval(10000.0) == 1.0);
	}
	{
	SF oSF{1.0, 1.0};
	REQUIRE(oSF.eval(0.0) == 1.0);
	REQUIRE(Approx(oSF.eval(10000.0)).epsilon(g_fErrMargin) == 10001.0);
	}
}

TEST_CASE("testSegmentedFunction, Max")
{
	{
	SF oSF1{1.0, 0.0};
	SF oSF2{0.0, 1.0};
	SF oSFR = SF::max(oSF1, oSF2);
	REQUIRE(Approx(oSFR.eval(0.0)).epsilon(g_fErrMargin) == 1.0);
	REQUIRE(Approx(oSFR.eval(1.0)).epsilon(g_fErrMargin) == 1.0);
	REQUIRE(Approx(oSFR.eval(2.0)).epsilon(g_fErrMargin) == 2.0);
	REQUIRE(Approx(oSFR.eval(10000.0)).epsilon(g_fErrMargin) == 10000.0);
	}
	{
	SF oSFa1{1.0, 12.0};
	SF oSFa2{0.0, 16.0};
	SF oSFaR = SF::max(oSFa1, oSFa2);
	//
	SF oSFb1{0.0, 18.0};
	SF oSFb2{2.0, 0.0};
	SF oSFbR = SF::max(oSFb1, oSFb2);
	//
	SF oSFR = SF::max(oSFaR, oSFbR);
	REQUIRE(Approx(oSFR.eval(0.0)).epsilon(g_fErrMargin) == 18.0);
	REQUIRE(Approx(oSFR.eval(4.0)).epsilon(g_fErrMargin) == 18.0);
	REQUIRE(Approx(oSFR.eval(5.0)).epsilon(g_fErrMargin) == 18.0);
	REQUIRE(Approx(oSFR.eval(6.0)).epsilon(g_fErrMargin) == 18.0);
	REQUIRE(Approx(oSFR.eval(7.0)).epsilon(g_fErrMargin) == 19.0);
	REQUIRE(Approx(oSFR.eval(9.0)).epsilon(g_fErrMargin) == 21.0);
	REQUIRE(Approx(oSFR.eval(12.0)).epsilon(g_fErrMargin) == 24.0);
	REQUIRE(Approx(oSFR.eval(14.0)).epsilon(g_fErrMargin) == 28.0);
	}
}
TEST_CASE("testSegmentedFunction, Max2")
{
	{
	SF oSF1{0.0, 0.0};
	SF oSF2{16.0, 0.0};
	SF oSFR = SF::max(oSF1, oSF2);
	REQUIRE(Approx(oSFR.eval(1.0)).epsilon(g_fErrMargin) == 16.0);
	REQUIRE(Approx(oSFR.eval(2.0)).epsilon(g_fErrMargin) == 32.0);
	}
}

TEST_CASE("testSegmentedFunction, Add")
{
	{
	SF oSF1{1.0, 0.0};
	SF oSF2{0.0, 1.0};
	SF oSFR = SF::add(oSF1, oSF2);
	REQUIRE(Approx(oSFR.eval(0.0)).epsilon(g_fErrMargin) == 1.0);
	REQUIRE(Approx(oSFR.eval(1.0)).epsilon(g_fErrMargin) == 2.0);
	REQUIRE(Approx(oSFR.eval(2.0)).epsilon(g_fErrMargin) == 3.0);
	REQUIRE(Approx(oSFR.eval(10000.0)).epsilon(g_fErrMargin) == 10001.0);
	}
	{
	SF oSFa1{1.0, 12.0};
	//oSFa1.dump();
	SF oSFa2{0.0, 16.0};
	SF oSFaR = SF::max(oSFa1, oSFa2);
	//
	SF oSFb1{0.0, 18.0};
	SF oSFb2{2.0, 0.0};
	SF oSFbR = SF::max(oSFb1, oSFb2);
	//
	SF oSFR = SF::add(oSFaR, oSFbR);
	REQUIRE(Approx(oSFR.eval(0.0)).epsilon(g_fErrMargin) == 34.0);
	REQUIRE(Approx(oSFR.eval(4.0)).epsilon(g_fErrMargin) == 34.0);
	REQUIRE(Approx(oSFR.eval(5.0)).epsilon(g_fErrMargin) == 35.0);
	REQUIRE(Approx(oSFR.eval(6.0)).epsilon(g_fErrMargin) == 36.0);
	REQUIRE(Approx(oSFR.eval(7.0)).epsilon(g_fErrMargin) == 37.0);
	REQUIRE(Approx(oSFR.eval(9.0)).epsilon(g_fErrMargin) == 39.0);
	REQUIRE(Approx(oSFR.eval(10.0)).epsilon(g_fErrMargin) == 42.0);
	REQUIRE(Approx(oSFR.eval(12.0)).epsilon(g_fErrMargin) == 48.0);
	REQUIRE(Approx(oSFR.eval(14.0)).epsilon(g_fErrMargin) == 54.0);
	}
	{
	SF oSFa1{1.0, 12.0};
	SF oSFa2{0.0, 16.0};
	SF oSFaR = SF::max(oSFa1, oSFa2);
	//
	SF oSFb1{1.0, 12.0};
	SF oSFb2{0.0, 16.0};
	SF oSFbR = SF::max(oSFb1, oSFb2);
	//
	SF oSFR = SF::add(oSFaR, oSFbR);
	REQUIRE(Approx(oSFR.eval(0.0)).epsilon(g_fErrMargin) == 32.0);
	REQUIRE(Approx(oSFR.eval(4.0)).epsilon(g_fErrMargin) == 32.0);
	REQUIRE(Approx(oSFR.eval(5.0)).epsilon(g_fErrMargin) == 34.0);
	REQUIRE(Approx(oSFR.eval(6.0)).epsilon(g_fErrMargin) == 36.0);
	REQUIRE(Approx(oSFR.eval(7.0)).epsilon(g_fErrMargin) == 38.0);
	REQUIRE(Approx(oSFR.eval(9.0)).epsilon(g_fErrMargin) == 42.0);
	}
}

TEST_CASE("testSegmentedFunction, Mul")
{
	{
	SF oSF{1.0, 0.0};
	oSF.mul(3.0);
	REQUIRE(Approx(oSF.eval(0.0)).epsilon(g_fErrMargin) == 0.0);
	REQUIRE(Approx(oSF.eval(1.0)).epsilon(g_fErrMargin) == 3.0);
	REQUIRE(Approx(oSF.eval(2.0)).epsilon(g_fErrMargin) == 6.0);
	REQUIRE(Approx(oSF.eval(10000.0)).epsilon(g_fErrMargin) == 30000.0);
	}
	{
	SF oSF{1.0, 20.0};
	oSF.mul(3.0);
	REQUIRE(Approx(oSF.eval(0.0)).epsilon(g_fErrMargin) == 60.0);
	REQUIRE(Approx(oSF.eval(1.0)).epsilon(g_fErrMargin) == 63.0);
	REQUIRE(Approx(oSF.eval(2.0)).epsilon(g_fErrMargin) == 66.0);
	}
}

TEST_CASE("testSegmentedFunction, Inverse")
{
	{
	SF oSF{0.0, 20.0};
	REQUIRE(Approx(oSF.evalInverse(0.0)).epsilon(g_fErrMargin) == -1.0);
	REQUIRE(Approx(oSF.evalInverse(10.0)).epsilon(g_fErrMargin) == -1.0);
	REQUIRE(oSF.evalInverse(20.0) >= 10000);
	REQUIRE(Approx(oSF.evalInverse(21.0)).epsilon(g_fErrMargin) == -1.0);
	}
	{
	SF oSF{1.0, 20.0};
	REQUIRE(Approx(oSF.evalInverse(0.0)).epsilon(g_fErrMargin) == -1.0);
	REQUIRE(Approx(oSF.evalInverse(10.0)).epsilon(g_fErrMargin) == -1.0);
	REQUIRE(Approx(oSF.evalInverse(20.0)).epsilon(g_fErrMargin) == 0.0);
	REQUIRE(Approx(oSF.evalInverse(21.0)).epsilon(g_fErrMargin) == 1.0);
	}
	{
	SF oSFa1{1.0, 12.0};
	SF oSFa2{0.0, 16.0};
	SF oSFaR = SF::max(oSFa1, oSFa2);
	//
	SF oSFb1{0.0, 18.0};
	SF oSFb2{2.0, 0.0};
	SF oSFbR = SF::max(oSFb1, oSFb2);
	//
	SF oSFR = SF::add(oSFaR, oSFbR);
	//
	REQUIRE(Approx(oSFR.evalInverse(33.0)).epsilon(g_fErrMargin) == -1.0);
	REQUIRE(Approx(oSFR.evalInverse(34.0)).epsilon(g_fErrMargin) == 4.0);
	REQUIRE(Approx(oSFR.evalInverse(35.0)).epsilon(g_fErrMargin) == 5.0);
	REQUIRE(Approx(oSFR.evalInverse(36.0)).epsilon(g_fErrMargin) == 6.0);
	REQUIRE(Approx(oSFR.evalInverse(37.0)).epsilon(g_fErrMargin) == 7.0);
	REQUIRE(Approx(oSFR.evalInverse(39.0)).epsilon(g_fErrMargin) == 9.0);
	REQUIRE(Approx(oSFR.evalInverse(42.0)).epsilon(g_fErrMargin) == 10.0);
	REQUIRE(Approx(oSFR.evalInverse(48.0)).epsilon(g_fErrMargin) == 12.0);
	REQUIRE(Approx(oSFR.evalInverse(54.0)).epsilon(g_fErrMargin) == 14.0);
	}
}

} // namespace testing

} // namespace stmg
