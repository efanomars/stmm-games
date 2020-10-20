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
 * File:   gamectx.h
 */

#ifndef STMG_GAME_CTX_H
#define STMG_GAME_CTX_H

#include <stmm-games-xml-base/conditionalctx.h>

#include <stmm-games-file/file.h>
#include <stmm-games-file/gameconstraints.h>

#include <stmm-games/block.h>
#include <stmm-games/ownertype.h>
#include <stmm-games/util/namedobjindex.h>

#include <memory>
#include <string>
#include <utility>

#include <stdint.h>

namespace stmg { class AppPreferences; }
namespace stmg { class Game; }
namespace stmg { class Level; }

namespace stmg
{

class GameCtx : public ConditionalCtx
{
public:
	GameCtx(const shared_ptr<AppPreferences>& refAppPreferences, File oGameFile, Game& oGame, const GameConstraints& oGameConstraints);
	Game& game() { return m_oGame; }
	NamedObjIndex<Block>& getNamedBlocks() { return m_oBlocks; }
	Level& level();

	const GameConstraints& getGameConstraints() const { return m_oGameConstraints; }

	/** Returns the context team.
	 * In "one team per level" games the level is returned.
	 * In "all teams in one level" games 0 is returned if only one team is playing,
	 * -1 otherwise.
	 * @return The pref team or -1 if undefined.
	 */
	int32_t getContextTeam() const override;
	/** Returns the variable id and owner type from the team and mate context.
	 * Example: if a team variable named "Points" exists and the parameter nTeam
	 * is -1, false is returned because can't determine what team it belongs to.
	 * @param sName The variable name. Cannot be empty.
	 * @param nTeam The context team. Must be >= 0 (and exist) or -1 if not defined.
	 * @param nMate The context mate. Must be >= 0 (and exist) or -1 if not defined. Must be -1 if nTeam is -1.
	 * @return If it exists and its context is defined, the variable's id and the owner type, -1 and undefined otherwise.
	 */
	std::pair<int32_t, OwnerType> getVariableIdAndOwnerTypeFromContext(const std::string& sName, int32_t nTeam, int32_t nMate) const;
protected:
	std::string err(const std::string& sErr) override;

private:
	friend class XmlGameParser;
	Game& m_oGame;
	File m_oGameFile;
	GameConstraints m_oGameConstraints;
	NamedObjIndex<Block> m_oBlocks;
	int32_t m_nLevel = -1;
private:
	GameCtx() = delete;
	GameCtx(const GameCtx& oSource) = delete;
	GameCtx& operator=(const GameCtx& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_GAME_CTX_H */

