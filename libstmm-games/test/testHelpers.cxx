/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   testHelpers.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "util/helpers.h"

namespace stmg
{

// using std::shared_ptr;

namespace testing
{

TEST_CASE("testHelpers, boardInsertRemove")
{
	NRect oRect;
	oRect.m_nX = 2;
	oRect.m_nY = 3;
	oRect.m_nW = 10;
	oRect.m_nH = 5;
	NRect oRes = Helpers::boardInsertRemove(oRect, Direction::UP);
	REQUIRE(oRes == NRect{2,3,10,1});

	oRes = Helpers::boardInsertRemove(oRect, Direction::DOWN);
	REQUIRE(oRes == NRect{2,3 + 5 -1,10,1});

	oRes = Helpers::boardInsertRemove(oRect, Direction::LEFT);
	REQUIRE(oRes == NRect{2,3,1,5});

	oRes = Helpers::boardInsertRemove(oRect, Direction::RIGHT);
	REQUIRE(oRes == NRect{2 + 10 - 1,3,1,5});
}

TEST_CASE("testHelpers, boardInsertMovingPre")
{
	NRect oRect;
	oRect.m_nX = 2;
	oRect.m_nY = 3;
	oRect.m_nW = 10;
	oRect.m_nH = 5;
	NRect oRes = Helpers::boardInsertMovingPre(oRect, Direction::UP);
	REQUIRE(oRes == NRect{2,3 + 1,10,5 - 1});

	oRes = Helpers::boardInsertMovingPre(oRect, Direction::DOWN);
	REQUIRE(oRes == NRect{2,3,10,5 - 1});

	oRes = Helpers::boardInsertMovingPre(oRect, Direction::LEFT);
	REQUIRE(oRes == NRect{2 + 1,3,10 - 1,5});

	oRes = Helpers::boardInsertMovingPre(oRect, Direction::RIGHT);
	REQUIRE(oRes == NRect{2,3,10 - 1,5});
}

TEST_CASE("testHelpers, boardInsertMovingPost")
{
	NRect oRect;
	oRect.m_nX = 2;
	oRect.m_nY = 3;
	oRect.m_nW = 10;
	oRect.m_nH = 5;
	NRect oRes = Helpers::boardInsertMovingPost(oRect, Direction::UP);
	REQUIRE(oRes == NRect{2,3,10,5 - 1});

	oRes = Helpers::boardInsertMovingPost(oRect, Direction::DOWN);
	REQUIRE(oRes == NRect{2,3 + 1,10,5 - 1});

	oRes = Helpers::boardInsertMovingPost(oRect, Direction::LEFT);
	REQUIRE(oRes == NRect{2,3,10 - 1,5});

	oRes = Helpers::boardInsertMovingPost(oRect, Direction::RIGHT);
	REQUIRE(oRes == NRect{2 + 1,3,10 - 1,5});
}

TEST_CASE("testHelpers, boardInsertAdd")
{
	NRect oRect;
	oRect.m_nX = 2;
	oRect.m_nY = 3;
	oRect.m_nW = 10;
	oRect.m_nH = 5;
	NRect oRes = Helpers::boardInsertAdd(oRect, Direction::UP);
	REQUIRE(oRes == NRect{2,3 + 5 -1,10,1});

	oRes = Helpers::boardInsertAdd(oRect, Direction::DOWN);
	REQUIRE(oRes == NRect{2,3,10,1});

	oRes = Helpers::boardInsertAdd(oRect, Direction::LEFT);
	REQUIRE(oRes == NRect{2 + 10 - 1,3,1,5});

	oRes = Helpers::boardInsertAdd(oRect, Direction::RIGHT);
	REQUIRE(oRes == NRect{2,3,1,5});
}

} // namespace testing

} // namespace stmg
