/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   testGameConstraints.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "gameconstraints.h"

namespace stmg
{

namespace testing
{

TEST_CASE("testGameConstraints, Constructor")
{
//std::cout << "testGameConstraints::Constructor()" << '\n';
	GameConstraints oGameConstraints;

	REQUIRE( oGameConstraints.isValid() );
}


} // namespace testing

} // namespace stmg
