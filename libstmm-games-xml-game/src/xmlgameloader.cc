/*
 * File:   xmlgameloader.cc
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

#include "xmlgameloader.h"

#include "xmlgameparser.h"
#include "xmllayoutparser.h"
#include "xmlgamefiles.h"
#include "xmleventparser.h"
#include "xmlgamewidgetparser.h"

#include <stmm-games-file/gameconstraints.h>
#include <stmm-games-file/gameloader.h>
#include <stmm-games-file/file.h>

#include <stmm-games/apppreferences.h>
#include <stmm-games/highscoresdefinition.h>

#include <libxml++/libxml++.h>

#include <vector>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <exception>
#include <algorithm>
#include <tuple>
#include <type_traits>

#include <stdint.h>

namespace stmg { class Game; }
namespace stmg { class GameOwner; }
namespace stmg { class Named; }

namespace stmg
{

XmlGameLoader::XmlGameLoader(Init&& oInit)
: m_refAppConfig(oInit.m_refAppConfig)
, m_refXmlGameFiles(oInit.m_refXmlGameFiles)
, m_refGameParser(std::make_shared<XmlGameParser>())
, m_bInfosLoaded(false)
, m_sDefaultGameName(std::move(oInit.m_sDefaultGameName))
{
	assert(m_refAppConfig);
	assert(m_refXmlGameFiles);
	for (auto& refXmlEventParser : oInit.m_aEventParsers) {
		m_refGameParser->addXmlEventParser(std::move(refXmlEventParser));
	}
	for (auto& refXmlGameWidgetParser : oInit.m_aGameWidgetParsers) {
		m_refGameParser->getLayoutParser().addXmlGameWidgetParser(std::move(refXmlGameWidgetParser));
	}
}

const std::string& XmlGameLoader::getDefaultGameName() const noexcept
{
	return m_sDefaultGameName;
}
void XmlGameLoader::loadGameInfos()
{
//std::cout << "XmlGameLoader::loadGameInfos()" << '\n';
	const std::vector< File >& aFiles = m_refXmlGameFiles->getGameFiles();
	if (aFiles.empty()) {
		std::cout << "Warning: No games found" << '\n';
	}

	for (const auto& oFile : aFiles) {
		assert(!oFile.isBuffered());
		const std::string& sFullPath = oFile.getFullPath();
		try	{
			xmlpp::DomParser oParser;
			//oParser.set_validate();
			//We just want the text to be resolved/unescaped automatically.
			oParser.set_substitute_entities();
			oParser.parse_file(sFullPath);
//std::cout << "XmlGameLoader::loadGameInfos()  sFullPath=" << sFullPath << '\n';
			if (oParser) {
				const xmlpp::Node* p0Node = oParser.get_document()->get_root_node();
				const xmlpp::Element* p0RootElement = dynamic_cast<const xmlpp::Element*>(p0Node);
				if (p0RootElement == nullptr) {
					throw std::runtime_error("Fatal error: root node is not an element");
				}
				auto oPair = m_refGameParser->parseGameInfo(m_refAppConfig, oFile, p0RootElement);
				GameInfo oGameInfo = std::move(oPair.first);
				const std::string& sName = oPair.second;
				const bool bNotFound = (std::find(m_aGameNames.begin(), m_aGameNames.end(), sName) == m_aGameNames.end());
//std::cout << "XmlGameLoader::loadGameInfos()  bNotFound=" << bNotFound << "  sName=" << sName << '\n';
				if (bNotFound) {
					oGameInfo.m_oThumbnailFile = m_refXmlGameFiles->getGameThumbnailFile(oFile);
					oGameInfo.m_oGameFile = std::move(oFile);
					m_aGameNames.push_back(sName);
					m_oNamedGameInfos[sName] = std::move(oGameInfo);
				} else {
					std::cout << "Discarding game";
					std::cout << " file '" << sFullPath << "'";
					std::cout << ": internal name '" << sName << "' already used" << '\n';
				}
			} else {
				std::cout << "Could not parse";
				std::cout << " file '" << sFullPath << "'";
				std::cout << '\n';
			}

		} catch(const std::exception& ex) {

			std::cout << "Exception caught";
			std::cout << " loading '" << sFullPath << "'";
			std::cout << ": " << ex.what() << '\n';
		}
	}
	std::sort(m_aGameNames.begin(), m_aGameNames.end(), [&](const std::string& sNamesL, const std::string& sNameR)
	{
		return (m_oNamedGameInfos[sNamesL].m_nDifficulty < m_oNamedGameInfos[sNameR].m_nDifficulty);
	});
	m_bInfosLoaded = true;
}

const std::vector<std::string>& XmlGameLoader::getGameNames() noexcept
{
	if (!m_bInfosLoaded) {
		loadGameInfos();
	}
	return m_aGameNames;
}
std::vector<std::string> XmlGameLoader::getGameNames(const AppPreferences& oAppPreferences) noexcept
{
	assert(oAppPreferences.getAppConfig() == m_refAppConfig);
	if (!m_bInfosLoaded) {
		loadGameInfos();
	}
	std::vector<std::string> aSelectedNames;
	for (auto& oNameGameInfoPair : m_oNamedGameInfos) {
		GameInfo& oGameInfo = oNameGameInfoPair.second;
		if (oGameInfo.m_oGameConstraints.isSelectedBy(oAppPreferences)) {
			aSelectedNames.push_back(oNameGameInfoPair.first);
		}
	}
	std::sort(aSelectedNames.begin(), aSelectedNames.end(), [&](const std::string& sNamesL, const std::string& sNameR)
	{
		return (m_oNamedGameInfos[sNamesL].m_nDifficulty < m_oNamedGameInfos[sNameR].m_nDifficulty);
	});
	return aSelectedNames;
}
const GameLoader::GameInfo& XmlGameLoader::getGameInfo(const std::string& sName) noexcept
{
	return getGameInfoPrivate(sName);
}
GameLoader::GameInfo& XmlGameLoader::getGameInfoPrivate(const std::string& sName)
{
	if (!m_bInfosLoaded) {
		loadGameInfos();
	}
	assert(std::find(m_aGameNames.begin(), m_aGameNames.end(), sName) != m_aGameNames.end());
	assert(m_oNamedGameInfos.find(sName) != m_oNamedGameInfos.end());
	return m_oNamedGameInfos[sName];
}

std::pair<shared_ptr<Game>, bool> XmlGameLoader::getNewGame(const std::string& sGameName, GameOwner& oGameOwner
															, const shared_ptr<AppPreferences>& refAppPreferences
															, const Named& oNamed, const shared_ptr<Highscore>& refHighscore) noexcept
{
	assert(refAppPreferences);
	assert(refAppPreferences->getAppConfig() == m_refAppConfig);

	if (!m_bInfosLoaded) {
		loadGameInfos();
	}

	assert(!m_aGameNames.empty());
	const std::string& sName = (sGameName.empty() ? m_aGameNames[0] : sGameName);

	std::pair<shared_ptr<Game>, bool> oPairGame{shared_ptr<Game>{}, false};

	GameInfo& oGameInfo = getGameInfoPrivate(sName);

	std::string& sErrorStr = oGameInfo.m_sGameErrorString;
	//if (!sErrorStr.empty()) {
	//	return refGame; //------------------------------------------------------
	//}

	if (! oGameInfo.m_oGameConstraints.isSelectedBy(*refAppPreferences)) {
		sErrorStr = "";
		return oPairGame; //----------------------------------------------------
	}

	try {
		const auto& refHD = oGameInfo.m_refHighscoresDefinition;
		oPairGame = parseGame(sName, oGameOwner, refAppPreferences, oNamed, refHighscore);
		auto& refGame = oPairGame.first;
		assert(refGame);
		if (refHD) {
			sErrorStr = refHD->isCompatibleWithGame(*refGame);
			if (! sErrorStr.empty()) {
				throw std::runtime_error(sErrorStr);
			}
			const int32_t nMaxScores = refHD->getMaxScores();
			if (nMaxScores < refAppPreferences->getTotTeams()) {
				// This is needed because Game needs to be able to create a Highscore which
				// can fit all teams playing
				sErrorStr = "HighscoresDefinition: the maximum number of scores is too low";
				throw std::runtime_error(sErrorStr);
			}
		}
		sErrorStr = "";
		oGameInfo.m_bLoaded = true;
	} catch(const std::exception& ex) {
		sErrorStr = "Exception caught parsing game '" + sName + "'" + ":\n" + ex.what();
		std::cout << sErrorStr << '\n';
		oGameInfo.m_bLoaded = false;
		oPairGame.first.reset();
	}
	return oPairGame;
}

std::pair<shared_ptr<Game>, bool> XmlGameLoader::parseGame(const std::string& sName, GameOwner& oGameOwner
															, const shared_ptr<AppPreferences>& refAppPreferences
															, const Named& oNamed, const shared_ptr<Highscore>& refHighscore)
{
	assert(!sName.empty());
	xmlpp::DomParser oParser;
	//oParser.set_validate();
	//We just want the text to be resolved/unescaped automatically.
	oParser.set_substitute_entities();

	GameInfo& oCurGameInfo = m_oNamedGameInfos[sName];
	const File& oFile = oCurGameInfo.m_oGameFile;
	assert(!oFile.isBuffered());
	const std::string& sFile = oFile.getFullPath();
	assert(!sFile.empty());
	oParser.parse_file(sFile);
	assert(oParser);

	const xmlpp::Node* p0Node = oParser.get_document()->get_root_node();
	const xmlpp::Element* p0RootElement = dynamic_cast<const xmlpp::Element*>(p0Node);
	if (p0RootElement == nullptr) {
		throw std::runtime_error("Fatal error: root node is not an element");
	}
	auto oTuple = m_refGameParser->parseGame(oFile, p0RootElement, oGameOwner, refAppPreferences, oNamed, refHighscore, oCurGameInfo);
	auto& refGame = std::get<0>(oTuple);
	const bool bHighscoresIgnored = std::get<1>(oTuple);
	if (bHighscoresIgnored) {
		std::cout << "Pre game Highscores ignored" << '\n';
	}
	auto& oNewGameInfo = std::get<2>(oTuple);
	oNewGameInfo.m_oThumbnailFile = oCurGameInfo.m_oThumbnailFile;
	oCurGameInfo = oNewGameInfo;
	return std::make_pair(std::move(refGame), bHighscoresIgnored);
}

} // namespace stmg
