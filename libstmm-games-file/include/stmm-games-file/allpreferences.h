/*
 * File:  allpreferences.h
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

#ifndef STMG_ALL_PREFERENCES_H
#define STMG_ALL_PREFERENCES_H

#include <stmm-games/stdpreferences.h>

#include <memory>
#include <string>
#include <vector>

#include <stdint.h>

namespace stmg { class StdConfig; }

namespace stmg
{

using std::shared_ptr;

class AllPreferences : public StdPreferences
{
public:
	/** Constructor.
	 * @see StdPreferences::StdPreferences(...)
	 * 
	 * The instance is created in edit mode.
	 * @param refStdConfig The configuration. Cannot be null.
	 */
	explicit AllPreferences(const shared_ptr<StdConfig>& refStdConfig) noexcept;
	/** Copy constructor.
	 * @param oSource The source.
	 */
	AllPreferences(const AllPreferences& oSource) noexcept;
	/** Copy assignment.
	 * @param oSource The source.
	 * @return The instance.
	 */
	AllPreferences& operator=(const AllPreferences& oSource) noexcept;

	/** Get the chosen game name.
	 * @return The name or empty if not set.
	 */
	std::string getGameName() const noexcept { return m_sCurrentGame; }
	/** Set game name.
	 * The name must be known to the game loader or empty.
	 * @param sGameName The name of the chosen game. Can be empty.
	 */
	void setGameName(const std::string& sGameName) noexcept
	{
		m_sCurrentGame = sGameName;
	}

	/** Get the chosen theme name.
	 * @return The name or empty if not set.
	 */
	std::string getThemeName() const noexcept { return m_sCurrentTheme; }
	/** Set theme name.
	 * The name must be known to the theme loader or empty.
	 * @param sThemeName The name of the chosen theme. Can be empty.
	 */
	void setThemeName(const std::string& sThemeName) noexcept
	{
		m_sCurrentTheme = sThemeName;
	}
	/** Add game to the played history.
	 * The game names are only checked syntactically.
	 * @param sGameName The game name. Must be a valid game name.
	 */
	void addGameToPlayedHistory(const std::string& sGameName) noexcept;
	/** The current played games history.
	 * @return The game names in last played order.
	 */
	const std::vector<std::string>& getPlayedGameHistory() const noexcept
	{
		return m_aGameHistory;
	}
private:
	std::string m_sCurrentGame;
	std::string m_sCurrentTheme;
	int32_t m_nGameHistoryMaxSize;
	std::vector<std::string> m_aGameHistory;

private:
	AllPreferences() = delete;
};

} // namespace stmg

#endif	/* STMG_ALL_PREFERENCES_H */

