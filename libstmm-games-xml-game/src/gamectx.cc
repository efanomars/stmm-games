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
 * File:   gamectx.cc
 */

#include "gamectx.h"

#include <stmm-games-file/gameconstraints.h>
#include <stmm-games-file/file.h>

#include <stmm-games/game.h>
#include <stmm-games/apppreferences.h>
#include <stmm-games/block.h>
#include <stmm-games/ownertype.h>

#include <cassert>
#include <cstdint>

namespace stmg { class Level; }

namespace stmg
{

GameCtx::GameCtx(const shared_ptr<AppPreferences>& refAppPreferences, File oGameFile, Game& oGame, const GameConstraints& oGameConstraints)
: ConditionalCtx(refAppPreferences, oGame.getNamed())
, m_oGame(oGame)
, m_oGameFile(std::move(oGameFile))
, m_oGameConstraints(std::move(oGameConstraints))
, m_nLevel(-1)
{
	assert(refAppPreferences.operator->() == &oGame.prefs());
}

Level& GameCtx::level()
{
	assert(m_nLevel >= 0);
	return *(m_oGame.level(m_nLevel));
}

int32_t GameCtx::getContextTeam() const
{
	if (m_oGameConstraints.isAllTeamsInOneLevel()) {
		if (m_oGame.prefs().getTotTeams() == 1) {
			return 0;
		} else {
			return -1;
		}
	}
	return m_nLevel;
}

std::pair<int32_t, OwnerType> GameCtx::getVariableIdAndOwnerTypeFromContext(const std::string& sName, int32_t nTeam, int32_t nMate) const
{
	assert(!sName.empty());
	const auto oPair = m_oGame.variableIdAndOwner(sName);
	int32_t nVarId = oPair.first;
	const OwnerType eOwnerType = oPair.second;
	if (nVarId >= 0) {
		if (eOwnerType == OwnerType::TEAM) {
			if (nTeam < 0) {
				nVarId = -1;
			}
		} else if (eOwnerType == OwnerType::PLAYER) {
			if (nMate < 0) {
				nVarId = -1;
			}
		}
	}
	return std::make_pair(nVarId, eOwnerType);
}

std::string GameCtx::err(const std::string& sErr)
{
	std::string sExtError;
	assert(!m_oGameFile.isBuffered());
	sExtError.append(m_oGameFile.getFullPath());
	sExtError.append(":\n");
	sExtError.append(sErr);
	return ConditionalCtx::err(sExtError);
}

} // namespace stmg
