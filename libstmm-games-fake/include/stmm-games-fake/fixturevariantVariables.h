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
 */
/*
 * File:   fixturevariantVariables.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_VARIANT_VARIABLES_H
#define STMG_TESTING_FIXTURE_VARIANT_VARIABLES_H

#ifdef STMG_TESTING_COPY_FROM_FAKE
#include "variable.h"
#include "util/namedobjindex.h"
#else
#include <stmm-games/variable.h>
#include <stmm-games/util/namedobjindex.h>
#endif

#include <memory>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

namespace testing
{

///////////////////////////////////////////////////////////////////////////////
class FixtureVariantVariablesGame_None //DEFAULT
{
};
class FixtureVariantVariablesGame_Time
{
};
class FixtureVariantVariablesGame // cumulative fixture variant
{
public:
	virtual ~FixtureVariantVariablesGame() = default;
protected:
	virtual NamedObjIndex<Variable::VariableType> getVariablesGame() const
	{
		NamedObjIndex<Variable::VariableType> oGameVariableTypes;
		if (dynamic_cast<FixtureVariantVariablesGame_Time const*>(this) != nullptr) {
			Variable::VariableType oT;
			oT.m_bTimeRelative = true;
			oGameVariableTypes.addNamedObj("Time", oT);
		}
		return oGameVariableTypes;
	}
};

///////////////////////////////////////////////////////////////////////////////
class FixtureVariantVariablesTeam_None //DEFAULT
{
};
class FixtureVariantVariablesTeam_Points_Zero
{
};
class FixtureVariantVariablesTeam_Points_Plus1000
{
};
class FixtureVariantVariablesTeam_Points_Minus1000
{
};
class FixtureVariantVariablesTeam // cumulative fixture variant
{
public:
	virtual ~FixtureVariantVariablesTeam() = default;
protected:
	virtual NamedObjIndex<Variable::VariableType> getVariablesTeam() const
	{
		NamedObjIndex<Variable::VariableType> oTeamVariableTypes;
		if (dynamic_cast<FixtureVariantVariablesTeam_Points_Zero const*>(this) != nullptr) {
			oTeamVariableTypes.addNamedObj("Points", /*Variable::VariableType*/{});
		} else if (dynamic_cast<FixtureVariantVariablesTeam_Points_Plus1000 const*>(this) != nullptr) {
			Variable::VariableType oVarType;
			oVarType.m_nInitialValue = 1000;
			oTeamVariableTypes.addNamedObj("Points", oVarType);
		} else if (dynamic_cast<FixtureVariantVariablesTeam_Points_Minus1000 const*>(this) != nullptr) {
			Variable::VariableType oVarType;
			oVarType.m_nInitialValue = -1000;
			oTeamVariableTypes.addNamedObj("Points", oVarType);
		}
		return oTeamVariableTypes;
	}
};

///////////////////////////////////////////////////////////////////////////////
class FixtureVariantVariablesPlayer_None //DEFAULT
{
};
class FixtureVariantVariablesPlayer_Points
{
};
template<int32_t LIVES>
class FixtureVariantVariablesPlayer_Lives
{
};
class FixtureVariantVariablesPlayer // cumulative fixture variant
{
public:
	virtual ~FixtureVariantVariablesPlayer() = default;
protected:
	virtual NamedObjIndex<Variable::VariableType> getVariablesPlayer() const
	{
		NamedObjIndex<Variable::VariableType> oPlayerVariableTypes;
		if (dynamic_cast<FixtureVariantVariablesPlayer_Points const*>(this) != nullptr) {
			oPlayerVariableTypes.addNamedObj("Points", /*Variable::VariableType*/{});
		}
		{
		bool bInitialLivesSet = true;
		Variable::VariableType oT;
		if (dynamic_cast<FixtureVariantVariablesPlayer_Lives<0> const*>(this) != nullptr) {
			oT.m_nInitialValue = 0;
		} else if (dynamic_cast<FixtureVariantVariablesPlayer_Lives<1> const*>(this) != nullptr) {
			oT.m_nInitialValue = 1;
		} else if (dynamic_cast<FixtureVariantVariablesPlayer_Lives<2> const*>(this) != nullptr) {
			oT.m_nInitialValue = 2;
		} else if (dynamic_cast<FixtureVariantVariablesPlayer_Lives<3> const*>(this) != nullptr) {
			oT.m_nInitialValue = 3;
		} else {
			bInitialLivesSet = false;
		}
		if (bInitialLivesSet) {
			oPlayerVariableTypes.addNamedObj("Lives", oT);
		}
		}
		return oPlayerVariableTypes;
	}
};

} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_VARIANT_VARIABLES_H */
