/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   fixturevariantTeams.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_VARIANT_TEAMS_H
#define STMG_TESTING_FIXTURE_VARIANT_TEAMS_H

#include <memory>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

namespace testing
{


///////////////////////////////////////////////////////////////////////////////
class FixtureVariantTeamsMin_One //DEFAULT
{
};
class FixtureVariantTeamsMin_Two
{
};
class FixtureVariantTeamsMin_Three
{
};
class FixtureVariantTeamsMin
{
public:
	virtual ~FixtureVariantTeamsMin() = default;
protected:
	int32_t getTeamsMin()
	{
		if (dynamic_cast<FixtureVariantTeamsMin_Two*>(this) != nullptr) {
			return 2;
		} else if (dynamic_cast<FixtureVariantTeamsMin_Three*>(this) != nullptr) {
			return 3;
		}
		return 1;
	}
};

///////////////////////////////////////////////////////////////////////////////
class FixtureVariantTeamsMax_One
{
};
class FixtureVariantTeamsMax_Two //DEFAULT
{
};
class FixtureVariantTeamsMax_Three
{
};
class FixtureVariantTeamsMax_Four
{
};
class FixtureVariantTeamsMax
{
public:
	virtual ~FixtureVariantTeamsMax() = default;
protected:
	int32_t getTeamsMax()
	{
		if (dynamic_cast<FixtureVariantTeamsMax_One*>(this) != nullptr) {
			return 1;
		} else if (dynamic_cast<FixtureVariantTeamsMax_Three*>(this) != nullptr) {
			return 3;
		} else if (dynamic_cast<FixtureVariantTeamsMax_Four*>(this) != nullptr) {
			return 4;
		}
		return 2;
	}
};

} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_VARIANT_TEAMS_H */
