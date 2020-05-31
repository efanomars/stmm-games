/*
 * File:   gameloader.h
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

#ifndef STMG_GAME_LOADER_H
#define STMG_GAME_LOADER_H

#include "file.h"
#include "gameconstraints.h"

#include <vector>
#include <string>
#include <memory>
#include <utility>

namespace stmg { class AppPreferences; }
namespace stmg { class Game; }
namespace stmg { class GameOwner; }
namespace stmg { class Highscore; }
namespace stmg { class HighscoresDefinition; }
namespace stmg { class Named; }

namespace stmg
{

class GameLoader
{
public:
	virtual ~GameLoader() noexcept = default;

	struct GameInfo
	{
		File m_oGameFile; /**< The file where the game is defined, if available. Can be undefined. */
		struct Author
		{
			std::string m_sName; /**< Author name. */
			std::string m_sEmail; /**< Author email. */
			std::string m_sWebSite; /**< Author web site. */
			std::string m_sRole; /**< Author role. */
		};
		std::vector<Author> m_aAuthors; /**< All the authors. */
		std::string m_sGameDescription; /**< Possibly multiline description of the game. */
		shared_ptr<HighscoresDefinition> m_refHighscoresDefinition; /**< The highscores associated with the game. Can be null. */
		File m_oThumbnailFile; /**< The location of the thumbnail image of the game. Can be undefined. */
		//std::vector<File> m_aInstructionImages; // additional images supposed to explain how to play the game.
		GameConstraints m_oGameConstraints; /**< The game constraints (on top of the app constraints) to the preferences. */
		int32_t m_nDifficulty = 0; /**< Difficulty hint. Value 0 very easy, value 100 very difficult. Can be used to order games. */
		bool m_bTesting = false; /**< Whether the game should be hidden when not testing (see StdConfig). */
		bool m_bLoaded = false; /**< Whether the game was successfuly loaded. */
		std::string m_sGameErrorString; /**< The error if loading failed or empty. Is empty if m_bLoaded is `true` or the game isn't compatible with the preferences. */
		//std::string m_sChallengeName;
		//bool m_bLocked = false;
		//File m_oLockedOverlayImageFile;
	};
	/** The default game name.
	 * @return The default game name. Can be empty.
	 */
	virtual const std::string& getDefaultGameName() const noexcept = 0;
	/** The game names supported by this loader.
	 * @return The (unique) non-empty names of the games.
	 */
	virtual const std::vector<std::string>& getGameNames() noexcept = 0;
	/** The game names supported by this loader that satisfy the preferences.
	 * @param oAppPreferences The preferences.
	 * @return The (unique) non-empty names of the games.
	 */
	virtual std::vector<std::string> getGameNames(const AppPreferences& oAppPreferences) noexcept = 0;
	/** The game information by name.
	 * @param sGameName The name. The game must exist. Cannot be empty.
	 * @return The game information.
	 */
	virtual const GameInfo& getGameInfo(const std::string& sGameName) noexcept = 0;
	/** Get a new game.
	 * If null is returned and error string in GameInfo is empty, it means
	 * the game is not compatible with the given preferences.
	 *
	 * If parameter refHighscore is not null, it must have been created by
	 * the GameInfo::m_refHighscoresDefinition of the game.
	 * @param sGameName The name of the game (which must exist) or empty if loader should choose.
	 * @param oGameOwner The owner of the game.
	 * @param refAppPreferences The preferences. Cannot be null.
	 * @param oThemeNamed The theme named from which game makes a copy.
	 * @param refHighscore The highscores for the game (and current preferences). Can be null,
	 * @return The game and whether the highscores were ignored or null if could not load.
	 */
	virtual std::pair<shared_ptr<Game>, bool> getNewGame(const std::string& sGameName, GameOwner& oGameOwner
														, const shared_ptr<AppPreferences>& refAppPreferences, const Named& oThemeNamed
														, const shared_ptr<Highscore>& refHighscore) noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_GAME_LOADER_H */

