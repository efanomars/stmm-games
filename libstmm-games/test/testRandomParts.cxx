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
 * File:   testRandomParts.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "util/randomparts.h"

namespace stmg
{

namespace testing
{

TEST_CASE("testRandomParts, Constructor")
{
	RandomParts<int32_t> oRP{};
	REQUIRE(oRP.getTotRandomParts() == 0);
	REQUIRE(oRP.getRandomRange() == 0);
}

TEST_CASE("testRandomParts, AddPartsClear")
{
	RandomParts<int32_t> oRP{};
	oRP.addRandomPart(10, 77);
	REQUIRE(oRP.getTotRandomParts() == 1);
	REQUIRE(oRP.getRandomPart(0) == 77);
	{
	int32_t nProb;
	REQUIRE(oRP.getRandomPart(0, nProb) == 77);
	REQUIRE(nProb == 10);
	}
	{
	int32_t nProb;
	int32_t nPrecProb;
	REQUIRE(oRP.getRandomPart(0, nProb, nPrecProb) == 77);
	REQUIRE(nProb == 10);
	REQUIRE(nPrecProb == 0);
	}
	REQUIRE(oRP.getRandomRange() == 10);

	oRP.addRandomPart(11, 88);
	REQUIRE(oRP.getTotRandomParts() == 2);
	REQUIRE(oRP.getRandomPart(0) == 77);
	REQUIRE(oRP.getRandomPart(1) == 88);
	{
	int32_t nProb;
	REQUIRE(oRP.getRandomPart(0, nProb) == 77);
	REQUIRE(nProb == 10);
	REQUIRE(oRP.getRandomPart(1, nProb) == 88);
	REQUIRE(nProb == 11);
	}
	{
	int32_t nProb;
	int32_t nPrecProb;
	REQUIRE(oRP.getRandomPart(0, nProb, nPrecProb) == 77);
	REQUIRE(nProb == 10);
	REQUIRE(nPrecProb == 0);
	REQUIRE(oRP.getRandomPart(1, nProb, nPrecProb) == 88);
	REQUIRE(nProb == 11);
	REQUIRE(nPrecProb == 10);
	}
	REQUIRE(oRP.getRandomRange() == 21);

	oRP.clear();
	REQUIRE(oRP.getTotRandomParts() == 0);
	REQUIRE(oRP.getRandomRange() == 0);
}

TEST_CASE("testRandomParts, GetPartFromRandom")
{
	RandomParts<int32_t> oRP{};
	oRP.addRandomPart(10, 77);
	oRP.addRandomPart(0, 88);
	oRP.addRandomPart(25, 99);
	REQUIRE(oRP.getRandomPartProb(0) == 77);
	REQUIRE(oRP.getRandomPartProb(9) == 77);
	REQUIRE(oRP.getRandomPartProb(10) == 99);
	REQUIRE(oRP.getRandomPartProb(34) == 99);
	int32_t nIdx;
	int32_t nRel;
	REQUIRE(oRP.getRandomPartProb(0, nIdx, nRel) == 77);
	REQUIRE(nIdx == 0);
	REQUIRE(nRel == 0);
	REQUIRE(oRP.getRandomPartProb(9, nIdx, nRel) == 77);
	REQUIRE(nIdx == 0);
	REQUIRE(nRel == 9);
	REQUIRE(oRP.getRandomPartProb(10, nIdx, nRel) == 99);
	REQUIRE(nIdx == 2);
	REQUIRE(nRel == 0);
	REQUIRE(oRP.getRandomPartProb(34, nIdx, nRel) == 99);
	REQUIRE(nIdx == 2);
	REQUIRE(nRel == 24);
}

} // namespace testing

} // namespace stmg
