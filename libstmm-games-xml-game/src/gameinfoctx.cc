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
 * File:   gameinfoctx.cc
 */

#include "gameinfoctx.h"

#include <stmm-games-file/gameloader.h>
#include <stmm-games-file/file.h>

#include <stmm-games/named.h>
#include <stmm-games/ownertype.h>
#include <stmm-games/util/namedobjindex.h>
#include <stmm-games/variable.h>

#include <cassert>
#include <cstdint>
#include <tuple>

namespace stmg { class AppConfig; }
namespace stmg { class AppPreferences; }

namespace stmg
{

GameInfoCtx::GameInfoCtx(const shared_ptr<AppConfig>& refAppConfig, const File& oGameFile)
: ConditionalCtx(refAppConfig, s_oDummy)
{
	m_oGameInfo.m_oGameFile = oGameFile;
	s_oDummy.clear();
}

GameInfoCtx::GameInfoCtx(const shared_ptr<AppPreferences>& refAppPreferences, Named& oNamed, const File& oGameFile)
: ConditionalCtx(refAppPreferences, oNamed)
{
	m_oGameInfo.m_oGameFile = oGameFile;
}

const NamedObjIndex<Variable::VariableType>& GameInfoCtx::getVariableTypes(OwnerType eOwnerType) const
{
	if (eOwnerType == OwnerType::GAME) {
		//
	} else if (eOwnerType == OwnerType::TEAM) {
		return m_oTeamVariableTypes;
	} else if (eOwnerType == OwnerType::PLAYER) {
		return m_oMateVariableTypes;
	} else {
		assert(false);
	}
	return m_oGameVariableTypes;
}

std::pair<int32_t, OwnerType> GameInfoCtx::getVariableIdAndOwnerType(const std::string& sVarName) const
{
	OwnerType eOwnerType = OwnerType::GAME;
	int32_t nIdxVar = m_oGameVariableTypes.getIndex(sVarName);
	if (nIdxVar >= 0) {
	} else {
		nIdxVar = m_oTeamVariableTypes.getIndex(sVarName);
		if (nIdxVar >= 0) {
			eOwnerType = OwnerType::TEAM;
		} else {
			nIdxVar = m_oMateVariableTypes.getIndex(sVarName);
			if (nIdxVar >= 0) {
				eOwnerType = OwnerType::PLAYER;
			} else {
				assert(nIdxVar == -1);
			}
		}
	}
	return std::make_pair(nIdxVar, eOwnerType);
}
std::tuple<double,double, double> GameInfoCtx::getSoundScales() const
{
	return std::make_tuple(m_fSoundScaleX, m_fSoundScaleY, m_fSoundScaleZ);
}

std::string GameInfoCtx::err(const std::string& sErr)
{
	std::string sExtError;
	assert(!m_oGameInfo.m_oGameFile.isBuffered());
	sExtError.append(m_oGameInfo.m_oGameFile.getFullPath());
	sExtError.append(": ");
	sExtError.append(sErr);
	return ConditionalCtx::err(sExtError);
}

} // namespace stmg
