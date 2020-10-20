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
 * File:   gameconstraints.h
 */

#ifndef STMG_GAME_CONSTRAINTS_H
#define STMG_GAME_CONSTRAINTS_H

#include <stmm-games/appconstraints.h>

#include <memory>
#include <algorithm>

#include <stdint.h>

namespace stmg { class AppPreferences; }
namespace stmg { class PrefSelector; }

namespace stmg
{

using std::unique_ptr;
using std::shared_ptr;

/** The game constraints.
 * This class can be used to define the conditions for which a game can be played.
 * This allows game loaders to choose the games that are compatible with
 * the current preferences. For example a game might work only for exactly 2 non AI teams in one level,
 * so a single player (chosen in the preferences) should be prevented from playing the game.
 *
 * GameConstraints is valid only if either all teams play in one level or
 * each team plays in a separate level. Therefore defining a game with constraints
 * (m_nLevelsMax > 1) and (m_nTeamsPerLevelMax > 1) is invalid.
 */
class GameConstraints : public AppConstraints
{
public:
	int32_t m_nLevelsMin = 1; /**< Must be positive. Default is 1. */
	int32_t m_nLevelsMax = 1; /**< Cannot be smaller than m_nLevelsMin. Default is 1. */
	int32_t m_nTeamsPerLevelMin = 1; /**< Must be positive. Default is 1. */
	int32_t m_nTeamsPerLevelMax = 4; /**< Cannot be smaller than m_nTeamsPerLevelMin. Default is 4. */
	int32_t m_nMatesPerTeamMin = 1; /**< Must be positive. Default is 1. */
	int32_t m_nPlayersMin = 1; /**< Must be positive. Default is 1. */
	int32_t m_nAITeamsMin = 0; /**< Cannot be negative. Default is 0. */
	int32_t m_nAITeamsMax = 0; /**< Cannot be smaller than m_nAITeamsMin. Default is 0. */

	shared_ptr<PrefSelector> m_refPrefSelector; /**< Additional constraints. Can be null. */

	/** Initialize this instance values so that it is compatible with the given app constraints.
	 * The app constraints must be valid.
	 * @param oAppConstraints The app constraints.
	 */
	void initFromAppConstraints(const AppConstraints& oAppConstraints) noexcept;

	/** Whether the instance is valid (consistent).
	 * @return Whether valid.
	 */
	bool isValid() const noexcept;

	/** The minimum number of teams that can play.
	 * This can be more restrictive than just m_nTeamsMin.
	 * @return The minimum number of teams.
	 */
	int32_t getMinTeams() const noexcept
	{
		return std::max(m_nTeamsMin, std::max(m_nLevelsMin, m_nTeamsPerLevelMin));
	}
	/** The maximum number of teams that can play.
	 * This can be more restrictive than just AppConstraints::getMaxTeams().
	 * @return The maximum number of teams.
	 */
	int32_t getMaxTeams() const noexcept
	{
		return std::min(AppConstraints::getMaxTeams(), std::max(m_nLevelsMax, m_nTeamsPerLevelMax));
	}
	/** The minimum number of players that can play.
	 * This can be more restrictive than just m_nPlayersMin.
	 * @return The minimum number of players.
	 */
	int32_t getMinPlayers() const noexcept
	{
		return std::max(m_nPlayersMin, getMinTeams());
	}
	/** The maximum number of players that can play.
	 * This can be more restrictive than just AppConstraints::getMaxPlayers().
	 * @return The maximum number of players.
	 */
	int32_t getMaxPlayers() const noexcept
	{
		return std::min(AppConstraints::getMaxPlayers(), m_nMatesPerTeamMax * std::max(m_nLevelsMax, m_nTeamsPerLevelMax));
	}
	/** Whether this constraints are compatible with preferences.
	 * @param oAppPreferences The preferences.
	 * @return Whether compatible.
	 */
	bool isSelectedBy(const AppPreferences& oAppPreferences) const noexcept;
	/** Whether the game constraints are compatible with application constraints.
	 * @param oAppConstraints The app constraints.
	 * @return Whether compatible.
	 */
	bool isCompatibleWith(const AppConstraints& oAppConstraints) const noexcept;
	/** Whether this game's constraints allow more teams to play in the same level.
	 * If the maximum number of levels and of teams per level is both one, this function
	 * returns true.
	 * @return Whether game of type "all teams in one level".
	 */
	inline bool isAllTeamsInOneLevel() const noexcept
	{
		return (m_nLevelsMax == 1);
	}
};

} // namespace stmg

#endif	/* STMG_GAME_CONSTRAINTS_H */

