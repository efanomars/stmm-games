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
 * File:   fixturevariantLayout.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_VARIANT_LAYOUT_H
#define STMG_TESTING_FIXTURE_VARIANT_LAYOUT_H

#include <memory>

namespace stmg
{

using std::shared_ptr;

namespace testing
{


///////////////////////////////////////////////////////////////////////////////
class FixtureVariantLayoutTeamDistribution_AllTeamsInOneLevel //DEFAULT
{
};
class FixtureVariantLayoutTeamDistribution_OneTeamPerLevel
{
};
class FixtureVariantLayoutTeamDistribution
{
public:
	virtual ~FixtureVariantLayoutTeamDistribution() = default;
protected:
	bool getAllTeamsInOneLevel() const
	{
		if (dynamic_cast<FixtureVariantLayoutTeamDistribution_OneTeamPerLevel const*>(this) != nullptr) {
			return false;
		}
		return true;
	}
};

///////////////////////////////////////////////////////////////////////////////
class FixtureVariantLayoutShowMode_Show //DEFAULT
{
};
class FixtureVariantLayoutShowMode_Subshow
{
};
class FixtureVariantLayoutShowMode
{
public:
	virtual ~FixtureVariantLayoutShowMode() = default;
protected:
	/** Return true for show mode, false for subshow mode.
	 * @return Whether show mode.
	 */
	bool getIsShowMode() const
	{
		if (dynamic_cast<FixtureVariantLayoutShowMode_Subshow const*>(this) != nullptr) {
			return false;
		}
		return true;
	}
};

///////////////////////////////////////////////////////////////////////////////
class FixtureVariantLayoutCreateVarWidgetsFromVariables_False //DEFAULT
{
};
class FixtureVariantLayoutCreateVarWidgetsFromVariables_True
{
};
class FixtureVariantLayoutCreateVarWidgetsFromVariables
{
public:
	virtual ~FixtureVariantLayoutCreateVarWidgetsFromVariables() = default;
protected:
	bool getCreateVarWidgetsFromVariables() const
	{
		if (dynamic_cast<FixtureVariantLayoutCreateVarWidgetsFromVariables_True const*>(this) != nullptr) {
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////////////////////
class FixtureVariantLayoutCreateActionWidgetsFromKeyActions_False //DEFAULT
{
};
class FixtureVariantLayoutCreateActionWidgetsFromKeyActions_True
{
};
class FixtureVariantLayoutCreateActionWidgetsFromKeyActions
{
public:
	virtual ~FixtureVariantLayoutCreateActionWidgetsFromKeyActions() = default;
protected:
	bool getCreateActionWidgetsFromKeyActions() const
	{
		if (dynamic_cast<FixtureVariantLayoutCreateActionWidgetsFromKeyActions_True const*>(this) != nullptr) {
			return true;
		}
		return false;
	}
};


} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_VARIANT_LAYOUT_H */
