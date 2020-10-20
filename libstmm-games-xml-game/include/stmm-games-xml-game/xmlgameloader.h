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
 * File:   xmlgameloader.h
 */

#ifndef STMG_XML_GAME_LOADER_H
#define STMG_XML_GAME_LOADER_H

#include "xmleventparser.h"
#include "xmlgamewidgetparser.h"

#include <stmm-games-file/gameloader.h>

#include <vector>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace stmg { class AppConfig; }
namespace stmg { class AppPreferences; }
namespace stmg { class Game; }
namespace stmg { class GameOwner; }
namespace stmg { class Named; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class Highscore;
class XmlGameFiles;
class XmlGameParser;

class XmlGameLoader : public GameLoader
{
public:
	struct Init
	{
		shared_ptr<AppConfig> m_refAppConfig; /**< The app configuration data. Cannot be null. */
		shared_ptr<XmlGameFiles> m_refXmlGameFiles; /**< The game files. Cannot be null. */
		std::string m_sDefaultGameName; /**< The default game name. Can be empty. */
		std::vector<unique_ptr<XmlEventParser>> m_aEventParsers; /**< The event parsers. Cannot contain nulls. */
		std::vector<unique_ptr<XmlGameWidgetParser>> m_aGameWidgetParsers; /**< The game widget parsers. Cannot contain nulls. */
	};
	/** Constructor.
	 * @param oInit Inizialization data..
	 */
	explicit XmlGameLoader(Init&& oInit);

	const std::vector<std::string>& getGameNames() noexcept override;
	std::vector<std::string> getGameNames(const AppPreferences& oAppPreferences) noexcept override;
	const GameInfo& getGameInfo(const std::string& sGameName) noexcept override;

	const std::string& getDefaultGameName() const noexcept override;

	std::pair<shared_ptr<Game>, bool> getNewGame(const std::string& sGameName, GameOwner& oGameOwner
												, const shared_ptr<AppPreferences>& refAppPreferences
												, const Named& oNamed, const shared_ptr<Highscore>& refHighscore) noexcept override;

private:
	void loadGameInfos();
	GameInfo& getGameInfoPrivate(const std::string& sGameName);
	// The game and whether the highscores was ignored
	std::pair<shared_ptr<Game>, bool> parseGame(const std::string& sName, GameOwner& oGameOwner
												, const shared_ptr<AppPreferences>& refAppPreferences
												, const Named& oNamed, const shared_ptr<Highscore>& refHighscore);
private:
	const shared_ptr<AppConfig> m_refAppConfig;
	const shared_ptr<XmlGameFiles> m_refXmlGameFiles;
	shared_ptr<XmlGameParser> m_refGameParser;

	bool m_bInfosLoaded;
	std::vector<std::string> m_aGameNames;
	std::map<std::string, GameInfo> m_oNamedGameInfos;

	const std::string m_sDefaultGameName;
private:
	XmlGameLoader() = delete;
	XmlGameLoader(const XmlGameLoader& oSource) = delete;
	XmlGameLoader& operator=(const XmlGameLoader& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_GAME_LOADER_H */

