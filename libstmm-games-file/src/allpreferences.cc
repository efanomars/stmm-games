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
 * File:   allpreferences.cc
 */

#include "allpreferences.h"

#include <stmm-games/stdconfig.h>
#include <stmm-games/stdpreferences.h>

#include <algorithm>

namespace stmg
{

AllPreferences::AllPreferences(const shared_ptr<StdConfig>& refStdConfig) noexcept
: StdPreferences(refStdConfig)
, m_nGameHistoryMaxSize(refStdConfig->getMaxGamePlayedHistory())
{
}
AllPreferences::AllPreferences(const AllPreferences& oSource) noexcept
: StdPreferences(oSource)
, m_sCurrentGame(oSource.m_sCurrentGame)
, m_sCurrentTheme(oSource.m_sCurrentTheme)
, m_nGameHistoryMaxSize(oSource.m_nGameHistoryMaxSize)
{
}
AllPreferences& AllPreferences::operator=(const AllPreferences& oSource) noexcept
{
	StdPreferences::operator=(oSource);
	m_sCurrentGame = oSource.m_sCurrentGame;
	m_sCurrentTheme = oSource.m_sCurrentTheme;
	m_nGameHistoryMaxSize = oSource.m_nGameHistoryMaxSize;
	return *this;
}

void AllPreferences::addGameToPlayedHistory(const std::string& sGameName) noexcept
{
	if (m_nGameHistoryMaxSize == 0) {
		return;
	}
	auto itFound = std::find(m_aGameHistory.begin(), m_aGameHistory.end(), sGameName);
	if (itFound == m_aGameHistory.end()) {
		if (static_cast<int32_t>(m_aGameHistory.size()) == m_nGameHistoryMaxSize) {
			auto itLast = m_aGameHistory.begin() + m_nGameHistoryMaxSize - 1;
			m_aGameHistory.erase(itLast);
		}
	} else {
		m_aGameHistory.erase(itFound);
	}
	m_aGameHistory.insert(m_aGameHistory.begin(), sGameName);
}

} // namespace stmg
