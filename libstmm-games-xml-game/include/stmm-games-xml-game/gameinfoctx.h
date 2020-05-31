/*
 * File:   gameinfoctx.h
 *
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

#ifndef STMG_GAME_INFO_CTX_H
#define STMG_GAME_INFO_CTX_H

#include "conditionalctx.h"

#include <stmm-games-file/gameloader.h>

#include <stmm-games/ownertype.h>
#include <stmm-games/util/namedobjindex.h>
#include <stmm-games/variable.h>

#include <memory>
#include <string>
#include <utility>
#include <tuple>

#include <stdint.h>

namespace stmg { class AppConfig; }
namespace stmg { class AppPreferences; }
namespace stmg { class AssignableNamed; }
namespace stmg { class File; }
namespace stmg { class GameConstraints; }
namespace stmg { class HighscoresDefinition; }
namespace stmg { class Named; }

namespace stmg
{

class GameInfoCtx : public ConditionalCtx
{
public:
	GameInfoCtx(const shared_ptr<AppConfig>& refAppConfig, const File& oGameFile);
	GameInfoCtx(const shared_ptr<AppPreferences>& refAppPreferences, Named& oNamed, const File& oGameFile);

	const GameConstraints& getGameConstraints() const { return m_oGameInfo.m_oGameConstraints; }
	const shared_ptr<HighscoresDefinition>& getHighscoresDefinition() const { return m_oGameInfo.m_refHighscoresDefinition; }

	inline const NamedObjIndex<Variable::VariableType>& getGameVariableTypes() const { return m_oGameVariableTypes; }
	inline const NamedObjIndex<Variable::VariableType>& getTeamVariableTypes() const { return m_oTeamVariableTypes; }
	inline const NamedObjIndex<Variable::VariableType>& getMateVariableTypes() const { return m_oMateVariableTypes; }
	const NamedObjIndex<Variable::VariableType>& getVariableTypes(OwnerType eOwnerType) const;
	std::pair<int32_t, OwnerType> getVariableIdAndOwnerType(const std::string& sVarName) const;
	/** Get minimum game interval.
	 * @return The interval in milliseconds or -1.0 if not defined.
	 */
	double getMinGameInterval() const { return m_fMinGameInterval; }
	/** Get initial game interval.
	 * @return The interval in milliseconds or -1.0 if not defined.
	 */
	double getInitialGameInterval() const { return m_fInitialGameInterval; }
	/** Get the maximum number of view ticks in a game interval.
	 * @return The maximum or -1 if not defined.
	 */
	int32_t getMaxViewTicks() const { return m_nMaxViewTicks; }
	/** Get additional time in milliseconds for highscores to appear when game ended.
	 * @return The additional time in milliseconds.
	 */
	int32_t getAdditionalHighscoresWait() const { return m_nAdditionalHighscoresWait; }
	/** The sound scales from tiles to sound coordinates.
	 * @return The scales.
	 */
	std::tuple<double,double, double> getSoundScales() const;
protected:
	std::string err(const std::string& sErr) override;

private:
	friend class XmlGameInfoParser;
	GameLoader::GameInfo m_oGameInfo;
	NamedObjIndex<Variable::VariableType> m_oGameVariableTypes;
	NamedObjIndex<Variable::VariableType> m_oTeamVariableTypes;
	NamedObjIndex<Variable::VariableType> m_oMateVariableTypes;
	double m_fMinGameInterval;
	double m_fInitialGameInterval;
	int32_t m_nMaxViewTicks;
	int32_t m_nAdditionalHighscoresWait;
	double m_fSoundScaleX;
	double m_fSoundScaleY;
	double m_fSoundScaleZ;
	static AssignableNamed s_oDummy;
private:
	GameInfoCtx() = delete;
	GameInfoCtx(const GameInfoCtx& oSource) = delete;
	GameInfoCtx& operator=(const GameInfoCtx& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_GAME_INFO_CTX_H */

