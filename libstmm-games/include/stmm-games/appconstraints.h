/*
 * File:   appconstraints.h
 *
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

#ifndef STMG_APP_CONSTRAINTS_H
#define STMG_APP_CONSTRAINTS_H

#include <algorithm>

#include <stdint.h>

namespace stmg
{

class AppConstraints
{
public:
	int32_t m_nTeamsMin = 1; /**< Must be positive. Default is 1. */
	int32_t m_nTeamsMax = 4; /**< Default is 4. */
	int32_t m_nMatesPerTeamMax = 3; /**< Must be positive. Default is 3. */
	int32_t m_nPlayersMax = 12; /**< Must be positive. Default is 12. */
	int32_t m_nAIMatesPerTeamMax = 1; /**< Cannot be negative. Default is 1. */
	bool m_bAllowMixedAIHumanTeam = false; /**< Default is false. */

	bool isValid() const noexcept;

	/** The real max number of teams.
	 * Takes also max number of players into account.
	 * @return The max number of teams.
	 */
	int32_t getMaxTeams() const noexcept
	{
		return std::min(m_nTeamsMax, m_nPlayersMax);
	}
	/** The real max number of teammates.
	 * Takes also max number of players into account.
	 * @return The max number of team mates.
	 */
	int32_t getMaxTeammates() const noexcept
	{
		return std::min(m_nMatesPerTeamMax, m_nPlayersMax);
	}
	/** The real max number of players.
	 * Takes also max number of teams and teammates into account.
	 * @return The max number of players.
	 */
	int32_t getMaxPlayers() const noexcept
	{
		return std::min(m_nTeamsMax * m_nMatesPerTeamMax, m_nPlayersMax);
	}
	/** Whether AI players are allowed.
	 * @return Whether a team can have an AI mate.
	 */
	bool allowsAI() const noexcept
	{
		return (m_nAIMatesPerTeamMax > 0);
	}

	//bool operator==(const AppConstraints& oOther) const;
};

} // namespace stmg

#endif	/* STMG_APP_CONSTRAINTS_H */

