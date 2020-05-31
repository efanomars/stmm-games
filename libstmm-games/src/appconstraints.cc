/*
 * File:   appconstraints.cc
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

#include "appconstraints.h"

namespace stmg
{

bool AppConstraints::isValid() const noexcept
{
	if (! ((m_nTeamsMin >= 1) && (m_nTeamsMin <= m_nTeamsMax)) ) {
		return false;
	}
	if (! (m_nMatesPerTeamMax >= 1)) {
		return false;
	}
	if (! (m_nPlayersMax >= 1)) {
		return false;
	}
	if (! (m_nAIMatesPerTeamMax >= 0)) {
		return false;
	}
	if (m_bAllowMixedAIHumanTeam && (m_nAIMatesPerTeamMax == 0)) {
		// allow mixed teams while ai is not allowed makes no sense
		return false;
	}
	if (m_nAIMatesPerTeamMax > m_nMatesPerTeamMax) {
		return false;
	}
	if (! (m_nPlayersMax >= m_nTeamsMin)) {
		return false;
	}
	return true;
}

	//bool AppConstraints::operator==(const AppConstraints& oAC) const
	//{
	//	if (oAC.m_nTeamsMin != m_nTeamsMin) {
	//		return false;
	//	}
	//	if (oAC.m_nTeamsMax != m_nTeamsMax) {
	//		return false;
	//	}
	//	if (oAC.m_nMatesPerTeamMax != m_nMatesPerTeamMax) {
	//		return false;
	//	}
	//	if (oAC.m_nPlayersMax != m_nPlayersMax) {
	//		return false;
	//	}
	//	if (oAC.m_nAIMatesPerTeamMax != m_nAIMatesPerTeamMax) {
	//		return false;
	//	}
	//	if (oAC.m_bAllowMixedAIHumanTeam != m_bAllowMixedAIHumanTeam) {
	//		return false;
	//	}
	//	return true;
	//}

} // namespace stmg
