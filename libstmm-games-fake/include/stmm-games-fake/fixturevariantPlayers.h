/*
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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
 *
 */
/*
 * File:   fixturevariantPlayers.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_VARIANT_PLAYERS_H
#define STMG_TESTING_FIXTURE_VARIANT_PLAYERS_H

#include <memory>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

namespace testing
{


///////////////////////////////////////////////////////////////////////////////
class FixtureVariantMatesPerTeamMax_One
{
};
class FixtureVariantMatesPerTeamMax_Two
{
};
class FixtureVariantMatesPerTeamMax_Three //DEFAULT
{
};
class FixtureVariantMatesPerTeamMax_Four
{
};
class FixtureVariantMatesPerTeamMax
{
public:
	virtual ~FixtureVariantMatesPerTeamMax() = default;

	int32_t getMatesPerTeamMax() const
	{
		if (dynamic_cast<FixtureVariantMatesPerTeamMax_One const*>(this) != nullptr) {
			return 1;
		} else if (dynamic_cast<FixtureVariantMatesPerTeamMax_Two const*>(this) != nullptr) {
			return 2;
		} else if (dynamic_cast<FixtureVariantMatesPerTeamMax_Four const*>(this) != nullptr) {
			return 4;
		}
		return 3;
	}
};

///////////////////////////////////////////////////////////////////////////////
class FixtureVariantAIMatesPerTeamMax_Zero //DEFAULT
{
};
class FixtureVariantAIMatesPerTeamMax_One
{
};
class FixtureVariantAIMatesPerTeamMax_Two
{
};
class FixtureVariantAIMatesPerTeamMax_Three
{
};
class FixtureVariantAIMatesPerTeamMax_Four
{
};
class FixtureVariantAIMatesPerTeamMax
{
public:
	virtual ~FixtureVariantAIMatesPerTeamMax() = default;

	int32_t getAIMatesPerTeamMax() const
	{
		if (dynamic_cast<FixtureVariantAIMatesPerTeamMax_One const*>(this) != nullptr) {
			return 1;
		} else if (dynamic_cast<FixtureVariantAIMatesPerTeamMax_Two const*>(this) != nullptr) {
			return 2;
		} else if (dynamic_cast<FixtureVariantAIMatesPerTeamMax_Three const*>(this) != nullptr) {
			return 3;
		} else if (dynamic_cast<FixtureVariantAIMatesPerTeamMax_Four const*>(this) != nullptr) {
			return 4;
		}
		return 0;
	}
};

///////////////////////////////////////////////////////////////////////////////
class FixtureVariantAllowMixedAIHumanTeam_False //DEFAULT
{
};
class FixtureVariantAllowMixedAIHumanTeam_True
{
};
class FixtureVariantAllowMixedAIHumanTeam
{
public:
	virtual ~FixtureVariantAllowMixedAIHumanTeam() = default;
protected:
	bool getAllowMixedAIHumanTeam() const
	{
		return (dynamic_cast<FixtureVariantAllowMixedAIHumanTeam_True const*>(this) != nullptr);
	}
};

///////////////////////////////////////////////////////////////////////////////
class FixtureVariantPlayersMax_One
{
};
class FixtureVariantPlayersMax_Two
{
};
class FixtureVariantPlayersMax_Three
{
};
class FixtureVariantPlayersMax_Four
{
};
class FixtureVariantPlayersMax_Five
{
};
class FixtureVariantPlayersMax_Six //DEFAULT
{
};
class FixtureVariantPlayersMax_Seven
{
};
class FixtureVariantPlayersMax_Height
{
};
class FixtureVariantPlayersMax_Nine
{
};
class FixtureVariantPlayersMax_Ten
{
};
class FixtureVariantPlayersMax
{
public:
	virtual ~FixtureVariantPlayersMax() = default;
protected:
	int32_t getPlayersMax() const
	{
		if (dynamic_cast<FixtureVariantPlayersMax_One const*>(this) != nullptr) {
			return 1;
		} else if (dynamic_cast<FixtureVariantPlayersMax_Two const*>(this) != nullptr) {
			return 2;
		} else if (dynamic_cast<FixtureVariantPlayersMax_Three const*>(this) != nullptr) {
			return 3;
		} else if (dynamic_cast<FixtureVariantPlayersMax_Four const*>(this) != nullptr) {
			return 4;
		} else if (dynamic_cast<FixtureVariantPlayersMax_Five const*>(this) != nullptr) {
			return 5;
		} else if (dynamic_cast<FixtureVariantPlayersMax_Seven const*>(this) != nullptr) {
			return 7;
		} else if (dynamic_cast<FixtureVariantPlayersMax_Height const*>(this) != nullptr) {
			return 8;
		} else if (dynamic_cast<FixtureVariantPlayersMax_Nine const*>(this) != nullptr) {
			return 9;
		} else if (dynamic_cast<FixtureVariantPlayersMax_Ten const*>(this) != nullptr) {
			return 10;
		}
		return 6;
	}
};


} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_VARIANT_PLAYERS_H */
