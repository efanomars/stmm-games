/*
 * File:   highscoresloader.h
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

#ifndef STMG_HIGHSCORES_LOADER_H
#define STMG_HIGHSCORES_LOADER_H

#include <stmm-games/highscoresdefinition.h>
#include <stmm-games/highscore.h>

#include <vector>
#include <string>
#include <memory>

namespace stmg
{

using std::shared_ptr;

class HighscoresLoader
{
public:
	virtual ~HighscoresLoader() noexcept = default;

	/** Get all the currently available highscores for a game.
	 * @param sGameName The game name. Cannot be empty.
	 * @param refHighscoresDefinition The highscores definition. Cannot be null.
	 * @return The highscores for the game.
	 */
	virtual std::vector<shared_ptr<Highscore>> getHighscores(const std::string& sGameName
															, const shared_ptr<HighscoresDefinition>& refHighscoresDefinition) const noexcept = 0;
	/** Get the highscore for the game (prefs) and the highscores definition.
	 * @param sGameName The game name. Cannot be empty.
	 * @param oPreferences The preferences.
	 * @param refHighscoresDefinition The highscores definition. Cannot be null.
	 * @return The highscores or null if there's no highscore for the game (type).
	 */
	virtual shared_ptr<Highscore> getHighscore(const std::string& sGameName, const AppPreferences& oPreferences
												, const shared_ptr<HighscoresDefinition>& refHighscoresDefinition) const noexcept = 0;
	/** Persist the highscores for a game.
	 * @param sGameName The game name. Cannot be empty.
	 * @param oPreferences The preferences.
	 * @param oHighscore The highscore.
	 * @return Whether the update was successful.
	 */
	virtual bool updateHighscore(const std::string& sGameName, const AppPreferences& oPreferences, const Highscore& oHighscore) noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_HIGHSCORES_LOADER_H */

