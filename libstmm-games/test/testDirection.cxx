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
 * File:   testDirection.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "util/direction.h"

namespace stmg
{

// using std::shared_ptr;

namespace testing
{

TEST_CASE("testDirection, All")
{
	REQUIRE(Direction::fromDelta(-1,0) == Direction::LEFT);
	REQUIRE(Direction::fromDelta(+1,0) == Direction::RIGHT);
	REQUIRE(Direction::fromDelta(0,-1) == Direction::UP);
	REQUIRE(Direction::fromDelta(0,+1) == Direction::DOWN);

	REQUIRE(Direction::deltaX(Direction::LEFT) == -1);
	REQUIRE(Direction::deltaX(Direction::RIGHT) == +1);
	REQUIRE(Direction::deltaX(Direction::UP) == 0);
	REQUIRE(Direction::deltaX(Direction::DOWN) == 0);

	REQUIRE(Direction::deltaY(Direction::LEFT) == 0);
	REQUIRE(Direction::deltaY(Direction::RIGHT) == 0);
	REQUIRE(Direction::deltaY(Direction::UP) == -1);
	REQUIRE(Direction::deltaY(Direction::DOWN) == +1);

	REQUIRE(Direction::opposite(Direction::LEFT) == Direction::RIGHT);
	REQUIRE(Direction::opposite(Direction::RIGHT) == Direction::LEFT);
	REQUIRE(Direction::opposite(Direction::UP) == Direction::DOWN);
	REQUIRE(Direction::opposite(Direction::DOWN) == Direction::UP);

	REQUIRE(Direction::rotateCW(Direction::LEFT) == Direction::UP);
	REQUIRE(Direction::rotateCW(Direction::RIGHT) == Direction::DOWN);
	REQUIRE(Direction::rotateCW(Direction::UP) == Direction::RIGHT);
	REQUIRE(Direction::rotateCW(Direction::DOWN) == Direction::LEFT);

	REQUIRE(Direction::rotateCCW(Direction::LEFT) == Direction::DOWN);
	REQUIRE(Direction::rotateCCW(Direction::RIGHT) == Direction::UP);
	REQUIRE(Direction::rotateCCW(Direction::UP) == Direction::LEFT);
	REQUIRE(Direction::rotateCCW(Direction::DOWN) == Direction::RIGHT);

	REQUIRE(((Direction::LEFT >= 0) && (Direction::LEFT < 4)));
	REQUIRE(((Direction::RIGHT >= 0) && (Direction::RIGHT < 4)));
	REQUIRE(((Direction::UP >= 0) && (Direction::UP < 4)));
	REQUIRE(((Direction::DOWN >= 0) && (Direction::DOWN < 4)));
}

} // namespace testing

} // namespace stmg
